#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <stdint.h>

void dhex(void* x, size_t len) {
    int i;
    uint8_t *p = (uint8_t*)x;
    for (i=0; i<len; i++) {
        if ((i % 32) == 0) printf("\n");
        printf(" %02x", p[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    int fd;
    fd = open("/dev/keypad0", O_RDONLY|O_NONBLOCK);
    //fd = open("/dev/keypad0", O_RDONLY);
    if (fd < 0) {
        printf("failed to open keypad0\n");
        goto exit;
    }

    fd_set rfds, wfds, xfds;
    struct timeval tv;
    int retval;
    int r;
    uint8_t buf[32] = {0};

#if 0
    printf("flush read?\n");
    do {
        r = read(fd, buf, 1);
        if (r > 0)
            printf("r=%d buf=%02x\n", r, buf[0]);
    } while (r > 0);
#endif

    while (1) {
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_ZERO(&xfds);
        FD_SET(fd, &rfds);

        //printf("fd=%d rfds:\n", fd);
        //dhex(&rfds, sizeof(rfds));

        //printf("fd set? %d\n", FD_ISSET(fd, &rfds));

       /* Wait up to five seconds. */
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        retval = select(fd + 1/*num fds*/, &rfds, NULL, NULL, &tv);
        //retval = select(fd + 1/*num fds*/, &rfds, &wfds, &xfds, &tv);
        //retval = select(1 /*num fds*/, &myfds, &wfds, &xfds, &tv);

        if (retval == -1) {
            perror("select()");
        } else if (retval) {
            printf("Data available\n");
            /* FD_ISSET(0, &rfds) will be true. */
            do {
                r = read(fd, buf, 1);
                printf("r=%d buf=%02x %c\n", r, buf[0], buf[0]);
            } while (r > 0);
        } else {
            printf("timed out\n");
        }
    }
#if 0
    printf("loop read..\n");
    do {
        r = read(fd, buf, 1);
        if (r > 0)
            printf("r=%d buf=%02x\n", r, buf[0]);
    } while (r > 0);
#endif

exit:
    if (fd >= 0) {
        close(fd);
    }

    return 0;
}
