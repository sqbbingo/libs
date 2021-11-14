/*
* @Author: bingo
* @Date:   2021-11-14 13:51:32
* @Last Modified by:   bingo
* @Last Modified time: 2021-11-14 14:22:24
*/
#include "head4animation.h"
void floating_up_in(int lcd, unsigned long (*image)[WIDTH])
{
	unsigned long (*FB)[WIDTH] = mmap(NULL, SCREEN_SIZE * 2,
	                                  PROT_READ | PROT_WRITE,
	                                  MAP_SHARED, lcd, 0);
	int i = 0;
	while (1)
	{
		memcpy(&FB[HEIGHT - i - 1][0], &image[0][0],
		       WIDTH * 2 * (i + 1));
		if (i >= HEIGHT - 1)
			break;
		usleep(1000);
		i++;
	}
}
void floating_up_out(int lcd, unsigned long (*image)[WIDTH])
{
	unsigned long (*FB)[WIDTH] = mmap(NULL, SCREEN_SIZE * 2,
	                                  PROT_READ | PROT_WRITE,
	                                  MAP_SHARED, lcd, 0);
	int i;
	for (i = 0; i <= HEIGHT; i++)
	{
		memset(&FB[HEIGHT - i][0], 0, WIDTH * 2 * i);
		memcpy(&FB[0][0], &image[i][0],
		       WIDTH * 2 * (HEIGHT - i));

		usleep(1000);
	}
}
void floating_up(int lcd, unsigned long (*image)[WIDTH])
{
	floating_up_in(lcd, image);
	floating_up_out(lcd, image);
}