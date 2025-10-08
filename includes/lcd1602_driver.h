#ifndef LCD1602_DRIVER_H
#define LCD1602_DRIVER_H

#define EN_OFF_MASK                 0xFB
#define HIGH_NIBBLE_MASK            0xF0
#define LOW_NIBBLE_MASK             0x0F
#define WRITE_INSTRUCTION_MASK      0x0C
#define WRITE_DATA_MASK             0x0D

/* LCD initialization values */
#define INTERFACE_8BIT_MODE         0x3C 
#define INTERFACE_4BIT_MODE         0x2C

/* LCD instructions */
#define LCD_CMD_CLEAR_DISPLAY       0x01
#define LCD_CMD_RETURN_HOME         0x02
#define LCD_CMD_ENTRY_MODE_SET      0x04
#define LCD_CMD_DISPLAY_CONTROL     0x08
#define LCD_CMD_FUNCTION_SET        0x20
#define LCD_CMD_DDRAM_ADDRESS       0x80   

/* Entry Mode Set flags */
#define LCD_ENTRY_INCREMENT         0x02
#define LCD_ENTRY_SHIFT             0x01

/* Display Control flags */
#define LCD_DISPLAY_ON              0x04                
#define LCD_CURSOR_ON               0x02
#define LCD_BLINK_ON                0x01

/* Function Set flags */                     
#define LCD_2LINE                   0x08
#define LCD_5x11_DOTS               0x04

/* DDRAM Addresses */
#define START_NEW_LINE              0x40

/* Extra commands */
#define LCD_BACKLIGHT_ON            0x0C
#define LCD_BACKLIGHT_OFF           0x04

#endif