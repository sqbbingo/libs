/*
* @Author: bingo
* @Date:   2021-11-13 12:51:08
* @Last Modified by:   bingo
* @Last Modified time: 2021-11-13 13:00:39
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

#define SCREEN_SIZE 480*270

void write_lcd(char *p, int picfd)
{
    memset(p, 0, SCREEN_SIZE * 2);

    int n, offset = 0;

    while (1) //循环是以防万一不能一次将图片全部读出
    {
        n = read(picfd, p + offset, SCREEN_SIZE * 2);
        if (n <= 0)
            break;
        offset += n;
    }
}

int main(int argc, char const *argv[])
{
    int lcd = open("/dev/fb0", O_RDWR); //打开LCD设备节点文件
    if (lcd == -1)
    {
        perror("open()");
        exit(1);
    }

    //将一块适当大小的内存映射为lcd设备的frame-buffer
    char *p = mmap(NULL, SCREEN_SIZE * 2,
                   PROT_READ | PROT_WRITE,
                   MAP_SHARED, lcd, 0);
    int picfd = open("girl.bin", O_RDONLY); //打开图片文件
    if (picfd == -1)
    {
        perror("open()");
        exit(-1);
    }

    write_lcd(p, picfd); //将文件刷进lcd设备对应的frame-buffer中

    return 0;
}