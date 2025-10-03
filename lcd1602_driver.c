#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>  
#include <linux/device.h> 
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/uaccess.h>

#define EN_OFF_MASK 0xFB
#define HIGH_NIBBLE_MASK 0xF0
#define LOW_NIBBLE_MASK 0x0F
#define WRITE_INSTRUCTION_MASK 0xC
#define WRITE_DATA_MASK 0xD

#define DEVICE_NAME "lcd1602_1"
#define CLASS_NAME "lcd1602"

static struct i2c_client *lcd1602_client;

static dev_t devt;
static struct cdev lcd1602_cdev;
static struct class *lcd1602_class;
static struct device *lcd1602_device;

/* probe and remove functions declaration */
static int lcd1602_probe(struct i2c_client *client);
static void lcd1602_remove(struct i2c_client *client);

/* list of compatible devices with this driver */
static struct of_device_id lcd1602_of_match_table[] = {
    { .compatible = "generic,lcd1602" },
    { /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, lcd1602_of_match_table);

/* driver information */
static struct i2c_driver lcd1602_driver_info = {
    .probe = lcd1602_probe,
    .remove = lcd1602_remove,
    .driver = {
        .name = "lcd1602_driver",
        .of_match_table = lcd1602_of_match_table
    }
};

static void lcd_send_byte(u8 RW_RS_MASK, u8 command) {

    u8 data = RW_RS_MASK;
    data |= (command & HIGH_NIBBLE_MASK);

    i2c_smbus_write_byte(lcd1602_client, data);
    i2c_smbus_write_byte(lcd1602_client, data & EN_OFF_MASK); 

    data = RW_RS_MASK;
    data |= (command & LOW_NIBBLE_MASK) << 4;

    i2c_smbus_write_byte(lcd1602_client, data);
    i2c_smbus_write_byte(lcd1602_client, data & EN_OFF_MASK);
    udelay(40);

    return;
}

static void lcd_send_command(u8 command) {
    lcd_send_byte(WRITE_INSTRUCTION_MASK, command);
}

static void lcd_write_character(u8 command) {
    lcd_send_byte(WRITE_DATA_MASK, command);
}

static ssize_t lcd_write_messages(struct file *filp, const char __user *user_buffer, size_t len, loff_t *off) {
    
    char kernel_buffer[32];
    bool next_line = false;
    int copied, not_copied, to_copy = min(len, sizeof(kernel_buffer));

    /* Clear display */
    lcd_send_command(0x01);
    mdelay(2);

    not_copied = copy_from_user(kernel_buffer, user_buffer, to_copy);
    copied = to_copy - not_copied;

    if (not_copied > 0) {
        pr_warn("lcd1602_driver - %d bytes were not copied\n", not_copied);
    }

    for(int i = 0; i < copied; i++) {

        if (i == 16 && next_line == false) {
            lcd_send_command(0xC0);
            next_line = true;
        }
        
        if (kernel_buffer[i] == '\n' && next_line == false) {
            lcd_send_command(0xC0);
            next_line = true;
            continue;
        }

        lcd_write_character(kernel_buffer[i]);

    }

    return copied;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = lcd_write_messages
};

static void lcd1602_initialization(void) {

    i2c_smbus_write_byte(lcd1602_client, 0x3C);
    i2c_smbus_write_byte(lcd1602_client, 0x3C & EN_OFF_MASK);
    mdelay(5);
    i2c_smbus_write_byte(lcd1602_client, 0x3C);
    i2c_smbus_write_byte(lcd1602_client, 0x3C & EN_OFF_MASK);
    udelay(200);
    i2c_smbus_write_byte(lcd1602_client, 0x3C);
    i2c_smbus_write_byte(lcd1602_client, 0x3C & EN_OFF_MASK);
    udelay(40);

    /* Set interface data length to 4-bit */
    i2c_smbus_write_byte(lcd1602_client, 0x2C); 
    i2c_smbus_write_byte(lcd1602_client, 0x2C & EN_OFF_MASK);
    udelay(40);

    /* Set number of display lines to 2 and character font to 5x8 */
    lcd_send_command(0x28);

    /* Set display on, cursor on and blinking cursor off */
    lcd_send_command(0xE);

    /* Clear display */
    lcd_send_command(0x01);
    mdelay(2);

    /* Set entry mode: increment cursor, no display shift */
    lcd_send_command(0x06);

    return;
}

static int lcd1602_probe(struct i2c_client *client) {

    int ret;

    pr_info("lcd1602_driver - Driver initialization...\n");

    if (client->addr != 0x27) {
        pr_err("lcd1602_driver - Client address does not match!\n");
        return -ENODEV;
    }

    lcd1602_client = client;

    /*
	 *	The alloc_chrdev_region() function allocates the major and minor number for the new device. It takes four parameters: (dev_t *dev, unsigned baseminor, unsigned count, const char *name))
	 *		- *dev: the kernel uses this pointer to return the major and minor number reserved
	 *		-  baseminor: specifies the first minor number in the range of devices to be reserved
	 *		-  count: is for the amount of devices that we want to add
	 *		- *name: takes the macro we declared at the beginning of the code
	 */

    ret = alloc_chrdev_region(&devt, 0, 1, DEVICE_NAME);

    if (ret < 0) {
        pr_err("lcd1602_driver - Error reserving major and minor numbers for the character device\n");
        return ret;
    }

    /*
	 * 	Initialize the character device. These functions do not create the /dev file, but register the device with the kernel.
	 *  	- cdev_init(): links the cdev instance with our file_operations, so the kernel knows which operations are available.
	 *   	- cdev_add(): registers the character device with the kernel, but does not create the device node in /dev.
	 */

    cdev_init(&lcd1602_cdev, &fops);
    ret = cdev_add(&lcd1602_cdev, devt, 1);

    if (ret < 0) {
        pr_err("lcd1602_driver - The character device file could not be registered\n");
        goto err_unregister_chrdev_region;
    }

    /*
	 *	The class_create() function creates a class to organize devices and provide a framework for device management in /sys/class
	 */

    lcd1602_class = class_create(CLASS_NAME);

    if (IS_ERR(lcd1602_class)) {
        pr_err("lcd1602_driver - Error creating a class for the device\n");
        ret = PTR_ERR(lcd1602_class);
        goto err_cdev_del;
    }

    /*
	 *	The device_create() function creates the node in /dev taking five parameters: (struct class *cls, struct device *parent, dev_t devt, void *drvdata, const char *fmt, ...);
	 *		*cls: this pointer stores our previous created class, grouping multiple devices in sysfs (/sys/class/<classname>)
	 *		*parent: pointer to parent device (NULL in this case)
	 *		 devt: major and minor numbers reserved
	 *		*drvdata: private data associated to the device (NULL in this case)
	 *		*fmt, ...: name of the character device file that will be shown in /dev 
	 */

    lcd1602_device = device_create(lcd1602_class, NULL, devt, NULL, DEVICE_NAME);
    
    if(IS_ERR(lcd1602_device)) {
		pr_err("lcd1602_driver - Error creating the character device file\n");
		ret = PTR_ERR(lcd1602_device);
        goto err_class_destroy;
	}

    mdelay(16);
    lcd1602_initialization();

    pr_info("lcd1602_driver - Driver initialized successfully with major number = %d\n", MAJOR(devt));

    return 0;

/* ~ Tags for handling errors ~ */
	
err_unregister_chrdev_region:
	unregister_chrdev_region(devt, 1);
	return ret;
err_cdev_del:
	cdev_del(&lcd1602_cdev);
	unregister_chrdev_region(devt, 1);
	return ret;
err_class_destroy:
	class_destroy(lcd1602_class);
	cdev_del(&lcd1602_cdev);
	unregister_chrdev_region(devt, 1);
	return ret;	
        
}

static void lcd1602_remove(struct i2c_client *client) {

	device_destroy(lcd1602_class, devt);
	class_destroy(lcd1602_class);
	cdev_del(&lcd1602_cdev);
	unregister_chrdev_region(devt, 1);

    pr_info("lcd1602_driver - Driver removed\n");

    return;
}

MODULE_AUTHOR("Carlos Rivas <leorivas1805@gmail.com>");
MODULE_DESCRIPTION("I2C LCD1602 Linux Kernel driver");
MODULE_LICENSE("GPL");

module_i2c_driver(lcd1602_driver_info);