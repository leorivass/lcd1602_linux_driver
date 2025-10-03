/*  
 *                  Project: LCD1602 Clock
 *                   Author: Carlos Rivas 
 *                     Date: 10-3-2025
 *
 *                      * Description *
 *      Small program that prints the local time on the LCD.
 *  Updates the display once per minute, showing a welcome message 
 *    on the first line and the time (HH:MM) on the second one.       
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

int main() {
    char msg[32];
    int fd, last_minute = -1;
    time_t t;
    struct tm *info;

    fd = open("/dev/lcd1602_1", O_WRONLY);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    while(1) {
        time(&t);
        info = localtime(&t);
        
        if (info->tm_min != last_minute) {
            snprintf(msg, sizeof(msg), "Welcome, user!\nHora: %02d:%02d", info->tm_hour, info->tm_min);
            
            if (write(fd, msg, strlen(msg)) < 0) {
                perror("write");
                close(fd);
                return -1;
            }
            
            last_minute = info->tm_min;
        }

        sleep(1);
    }

    close(fd);
    return 0;
}