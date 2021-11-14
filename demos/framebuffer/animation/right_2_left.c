/*
* @Author: bingo
* @Date:   2021-11-14 13:56:20
* @Last Modified by:   bingo
* @Last Modified time: 2021-11-14 14:23:10
*/

#include "head4animation.h"
void right2left_in(int lcd, unsigned long (*image)[WIDTH])

{
  unsigned long (*FB)[WIDTH] = mmap(NULL, SCREEN_SIZE * 2,
                                    PROT_READ | PROT_WRITE,
                                    MAP_SHARED, lcd, 0);
  int i, j;
  for (i = 0; i < WIDTH; i++)
  {
    for (j = 0; j < HEIGHT; j++)
    {
      memcpy(&FB[j][WIDTH - i - 1], &image[j][0],
             2 * (i + 1));
    }
    usleep(100);
  }
}

void right2left_out(int lcd, unsigned long (*image)[WIDTH])
{
  unsigned long (*FB)[WIDTH] = mmap(NULL, SCREEN_SIZE * 2,
                                    PROT_READ | PROT_WRITE,
                                    MAP_SHARED, lcd, 0);
  int i, j;
  for (i = 0; i < WIDTH; i++)
  {
    for (j = 0; j < HEIGHT; j++)
    {
      memcpy(&FB[j][0], &image[j][i + 1],
             (WIDTH - 1 - i) * 2);
      memset(&FB[j][WIDTH - 1 - i], 0, (1) * 2);
    }
    usleep(100);
  }
}
void right_2_left(int lcd, unsigned long (*image)[WIDTH])
{

  right2left_in(lcd, image);

  right2left_out(lcd, image);

}