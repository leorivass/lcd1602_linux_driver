#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main() {

    const char msg[20] = "Welcome, Leo! :)";

    int fd = open("/dev/lcd1602_1", O_WRONLY);

    if (fd < 0) {
        perror("open");
        return -1;
    }

    write(fd, msg, strlen(msg));

    close(fd);

    return 0;
}
