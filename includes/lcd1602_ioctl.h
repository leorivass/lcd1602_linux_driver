#ifndef LCD1602_IOCTL_H
#define LCD1602_IOCTL_H

#ifdef __KERNEL__
    #include <linux/ioctl.h>
#else
    #include <sys/ioctl.h>
#endif

#define LCD1602_IOC_MAGIC 'L'

#define CLEAR_DISPLAY           _IO(LCD1602_IOC_MAGIC, 0)
#define SET_CURSOR_ON           _IO(LCD1602_IOC_MAGIC, 1)
#define SET_CURSOR_OFF          _IO(LCD1602_IOC_MAGIC, 2)
#define SET_BLINKY_CURSOR_ON    _IO(LCD1602_IOC_MAGIC, 3)
#define SET_BLINKY_CURSOR_OFF   _IO(LCD1602_IOC_MAGIC, 4)
#define SET_BACKLIGHT_ON        _IO(LCD1602_IOC_MAGIC, 5)  
#define SET_BACKLIGHT_OFF       _IO(LCD1602_IOC_MAGIC, 6)   

#endif