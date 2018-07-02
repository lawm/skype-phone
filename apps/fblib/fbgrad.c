/* fbgrad: draw gradient using framebuffer.
   run in console, X11 would overwrite everything immediatelly.

   (c) Lev, 2018, MIT licence
*/
// based off from https://github.com/BartekLew/fblib/blob/fb-example/fbgrad.c

#include "fblib.h"

#include <stdio.h>

#include <fcntl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define fbdev "/dev/fb0"
#define ttydev "/dev/tty"

typedef struct {
    uint_fast8_t    r, g, b, a;
} Color;

#define Die(Msg, ...) { \
    fprintf (stderr, "fbgrad: " Msg ".\n", __VA_ARGS__); \
    exit(1); \
}\

#define Assumption(Cond, Msg) \
    if (!(Cond)) { \
        fprintf (stderr, "fbgrad: failed assumption: %s\n", Msg);\
        exit(2);\
    }

void dhex(void* x, size_t len) {
    int i;
    uint8_t *p = (uint8_t*)x;
    for (i=0; i<len; i++) {
        if ((i % 32) == 0) printf("\n");
        printf("%02x", p[i]);
    }
    printf("\n");
}

static void refresh(int lcdfd) {
    int r;
    unsigned int dirtyrows[2] = {0, 159}; // LCD_DirtyRows_t
    if (lcdfd < 0) {
        printf("lcdfd err \n");
        return;
    }
    r = ioctl(lcdfd, 0x80084c8c, &dirtyrows);
    if (r)
        printf("ioctl r=%d\n", r);
}

int main (int argc, char **argv) {
    //int ttyfd = open (ttydev, O_RDWR);
    //if (ttyfd < 0)
        //Die ("cannot open \"%s\"", ttydev);

    //if (ioctl (ttyfd, KDSETMODE, KD_GRAPHICS) == -1)
     //   Die ("cannot set tty into graphics mode on \"%s\"", ttydev);
     //

    printf("argv 0 = %s\n", argv[0]);
    printf("env _= %s\n", getenv("_"));

    int lcdfd = open("/dev/lcd", O_RDWR);
    if (lcdfd < 0)
        printf("cannot open lcd\n");

    int fbfd = open (fbdev, O_RDWR);
    if (fbfd < 0)
        Die ("cannot open \"%s\"", fbdev);

    struct fb_var_screeninfo vinf;
    struct fb_fix_screeninfo finf;

    if (ioctl (fbfd, FBIOGET_FSCREENINFO, &finf) == -1)
        Die ("cannot open fixed screen info for \"%s\"", fbdev);

    if (ioctl (fbfd, FBIOGET_VSCREENINFO, &vinf) == -1)
        Die ("cannot open variable screen info for \"%s\"", fbdev);

    /*
    Assumption ((vinf.red.offset%8) == 0 && (vinf.red.length == 8) &&
                (vinf.green.offset%8) == 0 && (vinf.green.length == 8) &&
                (vinf.blue.offset%8) == 0 && (vinf.blue.length == 8) &&
                (vinf.transp.offset) == 0 && (vinf.transp.length == 0) &&
                vinf.xoffset == 0 && vinf.yoffset == 0 &&
                vinf.red.msb_right == 0 &&
                vinf.green.msb_right == 0 &&
                vinf.blue.msb_right == 0,
                "Color masks are 8bit, byte aligned, little endian, no transparency."
    );
    */
    printf("f line_length=%u smem start/len=%lu %u\n",
            finf.line_length, finf.smem_start, finf.smem_len);
    printf("v xres/yres/virt=%u %u %u %u off=%u %u, bits/pixel=%u act=%u\n",
            vinf.xres,
            vinf.yres,
            vinf.xres_virtual,
            vinf.yres_virtual,
            vinf.xoffset,
            vinf.yoffset,
            vinf.bits_per_pixel,
            vinf.activate);

    Screen s = {
        .size            = finf.line_length * vinf.yres,
        .bytes_per_pixel = vinf.bits_per_pixel / 8,
        .bytes_per_line  = finf.line_length,
        .red             = vinf.red.offset/8,
        .green           = vinf.green.offset/8,
        .blue            = vinf.blue.offset/8,
        .width           = vinf.xres,
        .height          = vinf.yres
    };

    s.buffer = mmap (0, s.size, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);

    if (s.buffer == MAP_FAILED)
        Die ("cannot map frame buffer \"%s\"", fbdev);
    printf("mapped size %u to %p\n", s.size, s.buffer);

    int time_start = time (NULL);

#if 0
    for (uint t = 0; t < 255; t++) {
        for (uint y = 0; y < vinf.yres; y++) {
            for (uint x = 0; x < vinf.xres; x++) {
                //uint pix_offset = x * s.bytes_per_pixel + y * s.bytes_per_line;
                uint pix_offset = x + y;
                s.buffer[pix_offset + 0] = x * 255 / 160;//s.width;
                s.buffer[pix_offset + 1] = y * 255 / 120;//s.height;
                //s.buffer[pix_offset + s.blue] = t;
            }
        }
    }
#endif
    printf("buf before =");
    dhex(s.buffer, s.size > 256 ? 256 : s.size);

    if (argc > 1) {
        if (strcmp(argv[1], "dump") == 0) {
            printf("dump\n");
            int fd = open("/tmp/fb", O_CREAT|O_WRONLY);
            if (fd > 0) {
                write(fd, s.buffer, s.size);
                close(fd);
                printf("dumped\n");
            }
        } else if (strcmp(argv[1], "refresh") == 0) {
            printf("refresh\n");
            // will be done below
        } else if (strcmp(argv[1], "rand") == 0) {
            printf("rand\n");
            for (uint i=0; i< s.size; i++) {
                s.buffer[i] = rand() % 0xff;
            }
        } else if (strcmp(argv[1], "zero") == 0) {
            printf("zero\n");
            memset(s.buffer, 0, s.size);
        } else if (strcmp(argv[1], "ff") == 0) {
            printf("ff\n");
            memset(s.buffer, 0xff, s.size);
        } else if (strcmp(argv[1], "7f") == 0) {
            printf("7f\n");
            //memset(s.buffer, 0x7f, s.size);
            int i;
                unsigned int r, g, b;
                r = 0x00;
                g = 0x00;
                b = 0xff;
                uint16_t p = ((r & 0x1f) << 11) |
                             ((g & 0x3f) << 5) |
                             ((b & 0x1f) << 0);
                printf("r/g/b=%x/%x/%x p=%x\n", r, g, b, p);
            for (i=0; i<900; i+=2) {
                *((uint16_t*)(s.buffer + i)) = p;
            }
        } else {
            int fd = open(argv[1], O_RDONLY);
            if (fd < 0) printf("unable to open file\n");
            else {
                int r = read(fd, s.buffer, s.size);
                printf("read %d into buffer\n", r);
                close(fd);
            }
        }
    } else {
        printf("default\n");
        for (uint i=0; i< s.size; i++) {
            s.buffer[i] = rand() % 0xff;
        }
    }

    int time_end = time(NULL);

    printf("buf after=");
    dhex(s.buffer, s.size > 256 ? 256 : s.size);

    refresh(lcdfd);

    munmap (s.buffer, s.size);

    //if (ioctl (ttyfd, KDSETMODE, KD_TEXT) == -1)
     //   Die ("cannot set tty into text mode on \"%s\"", ttydev);

    if (lcdfd >= 0) close (lcdfd);
    close (fbfd);
    //close (ttyfd);

    printf ("FPS: %.2f.\n", 255.0 / (time_end - time_start));

    return EXIT_SUCCESS;
}
