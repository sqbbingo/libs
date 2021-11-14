/*
* @Author: bingo
* @Date:   2021-11-14 13:58:36
* @Last Modified by:   bingo
* @Last Modified time: 2021-11-14 14:23:36
*/

#include "head4animation.h"
#define IN 1
#define OUT 0
void *routine(void *p)
{
    struct argument *arg = (struct argument *)p;
    int i;
    for (i = 0; i < HEIGHT / BLIND; i++)
    {
        if (arg->flag == IN)
        {
            memcpy(&(arg->FB)[arg->offset + i][0],
                   &(arg->image)[arg->offset + i][0],
                   WIDTH * 2);
        }
        if (arg->flag == OUT)
        {
            memset(&(arg->FB)[arg->offset + i][0],
                   0, WIDTH * 2);
        }
        usleep(10000);
    }
    pthread_exit(NULL);
}
void __write_lcd(int lcd, unsigned long (*image)[WIDTH], int flag)
{
    unsigned long (*p)[WIDTH] = mmap(NULL, SCREEN_SIZE * 2,
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED, lcd, 0);
    int i;
    pthread_t tid[BLIND];
    for (i = 0; i < BLIND; i++)
    {
        struct argument *arg =
            malloc(sizeof(struct argument));
        arg->FB = p; // frame-buffer 指针
        arg->image = image; // 图片数据缓冲区指针
        arg->offset = i * (HEIGHT / BLIND); // 第 i 条线程负责区域的偏移量
        arg->flag = flag; // IN 为显示图片，OUT 为消除图片
        // 创建一条线程并将参数 arg 传过去，详情参见第五章多线程部分。
        pthread_create(&tid[i], NULL, routine, (void *)arg);
    }
    for (i = 0; i < BLIND; i++)
    {
        pthread_join(tid[i], NULL);
    }
}
void blind_window_in(int lcd, unsigned long (*image)[WIDTH])
{
    __write_lcd(lcd, image, IN);
}
void blind_window_out(int lcd, unsigned long (*image)[WIDTH])
{
    __write_lcd(lcd, image, OUT);
}
// 以百叶窗形式显示图片，再以百叶窗形式消除图片
void blind_window(int lcd, unsigned long (*image)[WIDTH])
{
    blind_window_in(lcd, image);
    blind_window_out(lcd, image);
}