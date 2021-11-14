/*
* @Author: bingo
* @Date:   2021-11-14 14:02:42
* @Last Modified by:   bingo
* @Last Modified time: 2021-11-14 14:54:45
*/

#include "head4animation.h"
void get_image(const char *filename, unsigned long (*buf)[WIDTH])
{
	int fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		perror("open()");
		exit(1);
	}
	int n, offset = 0;
	while (1)
	{
		n = read(fd, buf, SCREEN_SIZE * 2);
		if (n <= 0)
			break;
		offset += n;
	}
}
int main(void)
{
	int lcd = open("/dev/fb0", O_RDWR);
	if (lcd < 0)
	{
		perror("open()");
		exit(1);
	}
	unsigned long (*buf)[WIDTH] = calloc(SCREEN_SIZE, 2);
	get_image("../girl.bin", buf);
	falling_down(lcd, buf); // 测试掉落效果
	// floating_up(lcd, buf); // 测试上升效果
	// left_2_right(lcd, buf); // 测试从左往右飞效果
	// right_2_left(lcd, buf); // 测试从右往左飞效果
	// blind_window(lcd, buf); // 测试百叶窗效果
	return 0;
}