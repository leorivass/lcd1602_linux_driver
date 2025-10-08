# I2C LCD 16x2 Linux Kernel Driver

An I2C Linux Kernel driver for the 16x2 LCD display module, implementing device tree and character device interface.

## Features

- Character device interface for writing messages on the display.
- Device tree integration for better portability.
- ioctl commands to interact directly with the device.
- Use of the `\n` character to jump to the second line of the LCD.

## Hardware Requirements

- LCD 16x2 display module with a PCF8574 I2C expander
- Linux-compatible board (tested on a Raspberry PI 3B+)

## Hardware Connections

The following image shows how the connections must be made

<img width="1189" height="751" alt="image" src="https://github.com/user-attachments/assets/263b1420-b406-49fd-a804-6d221e83dbce" />

## How to use

### 1. Check if the LCD is being detected
```
i2cdetect -y 1
```
> This command should display the address of the LCD you connected.

### 2. Configure the device tree for your respective board
- Open lcd1602_overlay.dts
- If you have a different small board computer, make sure to change `compatible = "brcm,bcm2837"` for your respective board
- Make sure `reg = <>` contains the address you got from step one
- Save the file

### 3. Compile the driver
```
make
```

### 4. Compile the device tree source
```
dtc -@ -I dts -O dtb -o lcd1602_overlay.dtbo lcd1602_overlay.dts
```

### 5. Load the dtbo file
```
dtoverlay lcd1602_overlay.dtbo
```

### 6. Check if the dtbo file was loaded correctly
```
dtoverlay -l
```
> Note: The command must display something like this:
> ```
> Overlays (in load order):
> 0:  lcd1602_overlay
> ```

### 7. Load the driver module
```
sudo insmod lcd1602_driver.ko
```

### 8. Check if the driver module was loaded correctly
```
sudo dmesg | tail
```
> Note: dmesg must display something like this:
> ```
> lcd1602_driver - Driver initialization...
> lcd1602_driver - Driver initialized successfully with major number = {random_number}
> ```

### 9. Compile test.c 
```
cd src
gcc test.c
```

### 10. Execute test.c
```
sudo ./a.out
```
