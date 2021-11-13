/*
* @Author: bingo
* @Date:   2021-11-13 10:44:32
* @Last Modified by:   bingo
* @Last Modified time: 2021-11-13 12:38:57
*/
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/fb.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

enum color {red, green, blue};

//根据fb_var_screeninfo的色彩构成一个颜色像素数据
unsigned long *create_pixel(struct fb_var_screeninfo *pinfo,
                            enum color c)
{
    unsigned long *pixel = calloc(1, pinfo->bits_per_pixel / 8);
    unsigned long *mask = calloc(1, pinfo->bits_per_pixel / 8);
    *mask |= 0x1;

    int i;
    switch (c)
    {
    case red:
        for (i = 0; i < pinfo->red.length - 1; i++)
        {
            *mask <<= 1;
            *mask |= 0x1;
        }
        *pixel |= *mask << pinfo->red.offset;
        break;
    case green:
        for (i = 0; i < pinfo->green.length - 1; i++)
        {
            *mask <<= 1;
            *mask |= 0x1;
        }
        *pixel |= *mask << pinfo->green.offset;
        break;
    case blue:
        for (i = 0; i < pinfo->blue.length - 1; i++)
        {
            *mask <<= 1;
            *mask |= 0x1;
        }
        *pixel |= *mask << pinfo->blue.offset;
        break;
    }

    return pixel;
}

int main(int argc, char const *argv[])
{
    int lcd = open("/dev/fb0", O_RDWR);
    if (lcd == -1)
    {
        perror("open(\"/dev/fb0\")");
        exit(1);
    }

    //获取显示设备相关信息
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;
    ioctl(lcd, FBIOGET_FSCREENINFO, &finfo);
    ioctl(lcd, FBIOGET_VSCREENINFO, &vinfo);

    //初始化可见区偏移量
    vinfo.xoffset = 0;
    vinfo.yoffset = 0;
    ioctl(lcd, FBIOPAN_DISPLAY, &vinfo);

    unsigned long bpp = vinfo.bits_per_pixel;
    printf("bpp = %lu\n", bpp);

    //创建三原色像素点
    unsigned long *pixel[3] = {0};
    pixel[0] = create_pixel(&vinfo, red);
    pixel[1] = create_pixel(&vinfo, green);
    pixel[2] = create_pixel(&vinfo, blue);

    //申请一块对于lcd设备的映射内存
    char *FB = mmap(NULL, vinfo.xres * vinfo.yres * bpp / 8,
                    PROT_READ | PROT_WRITE, MAP_SHARED,
                    lcd, 0);

    int k;
    for (k = 0; ; k++)
    {
        int i;
        for (i = 0; i < vinfo.xres * vinfo.yres; i++)
        {
            memcpy(FB + i * bpp / 8, pixel[k % 3], bpp / 8);
        }
        sleep(1);//每隔一秒刷一次屏
    }

    return 0;
}