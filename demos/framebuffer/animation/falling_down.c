/*
* @Author: bingo
* @Date:   2021-11-14 13:47:10
* @Last Modified by:   bingo
* @Last Modified time: 2021-11-14 15:16:32
*/

#include "head4animation.h"

// 从上而下显示图片
void falling_down_in(int lcd, unsigned long (*image)[WIDTH])
{
// 申请一块适当大小的 frame-buffer，映射到 LCD
	unsigned long (*FB)[WIDTH] = mmap(NULL, SCREEN_SIZE * 2,
	                                  PROT_READ | PROT_WRITE,
	                                  MAP_SHARED, lcd, 0);
	int i;
	for (i = 0; i < HEIGHT; i++)
	{
		memcpy(&FB[0][0], &image[HEIGHT - i - 1][0],
		       WIDTH * 2 * (i + 1));
		usleep(1000);
	}
}
// 从上而下消除图片
void falling_down_out(int lcd, unsigned long (*image)[WIDTH])
{
	unsigned long (*FB)[WIDTH] = mmap(NULL, SCREEN_SIZE * 2,
	                                  PROT_READ | PROT_WRITE,
	                                  MAP_SHARED, lcd, 0);
	int i;
	printf("%s %d\n", __FUNCTION__, __LINE__);
	printf("fb_addr=%d %d %d\n", &FB , &FB[0][0], &FB[50][0]);
	for (i = 0; i <= HEIGHT; i++)
	{
		printf("%s %d i=%d\n", __FUNCTION__, __LINE__, i);
		memset(&FB[0][0], 0, WIDTH * 2 * i);
		printf("%s %d\n", __FUNCTION__, __LINE__);
		memcpy(&FB[i][0], &image[0][0],
		       WIDTH * 2 * (HEIGHT - i));
		printf("%s %d\n", __FUNCTION__, __LINE__);
		usleep(1000);
	}
	printf("%s %d\n", __FUNCTION__, __LINE__);
}
// 图片从上而下掉落，穿过显示屏。
void falling_down(int lcd, unsigned long (*image)[WIDTH])
{
	falling_down_in(lcd, image);
	falling_down_out(lcd, image);
	printf("%s %d\n", __FUNCTION__, __LINE__);
}