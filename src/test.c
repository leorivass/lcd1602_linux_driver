#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include "../includes/lcd1602_ioctl.h"

int main() {

    const char msg[32] = "Welcome, user!";

    int fd = open("/dev/lcd1602_1", O_WRONLY);

    if (fd < 0) {
        perror("open");
        return -1;
    }

    write(fd, msg, strlen(msg));
    sleep(5);
    ioctl(fd, SET_BACKLIGHT_OFF);
    sleep(5);
    ioctl(fd, SET_BACKLIGHT_ON);
    sleep(5);
    ioctl(fd, SET_BACKLIGHT_OFF);
    sleep(5);
    ioctl(fd, SET_BACKLIGHT_ON);
    sleep(5);
    ioctl(fd, CLEAR_DISPLAY);
    sleep(5);
    write(fd, msg, strlen(msg));
    sleep(5);
    ioctl(fd, SET_CURSOR_OFF);
    sleep(5);
    ioctl(fd, SET_CURSOR_ON);
    sleep(5);
    ioctl(fd, SET_BLINKY_CURSOR_ON);

    close(fd);

    return 0;
}
