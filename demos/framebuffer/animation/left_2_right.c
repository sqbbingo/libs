/*
* @Author: bingo
* @Date:   2021-11-14 13:54:02
* @Last Modified by:   bingo
* @Last Modified time: 2021-11-14 14:22:49
*/

#include "head4animation.h"
void left2right_in(int lcd, unsigned long (*image)[WIDTH])
{
  unsigned long (*FB)[WIDTH] = mmap(NULL, SCREEN_SIZE * 2,
                                    PROT_READ | PROT_WRITE,
                                    MAP_SHARED, lcd, 0);
  int i, j;
  for (i = 0; i < WIDTH; i++)
  {
    for (j = 0; j < HEIGHT; j++)
    {
      memcpy(&FB[j][0],
             &image[j][WIDTH - 1 - i], 2 * (i + 1));
    }
  }

}
void left2right_out(int lcd, unsigned long (*image)[WIDTH])
{
  unsigned long (*FB)[WIDTH] = mmap(NULL, SCREEN_SIZE * 2,
                                    PROT_READ | PROT_WRITE,
                                    MAP_SHARED, lcd, 0);
  int i, j;
  for (i = 0; i < WIDTH; i++)
  {
    for (j = 0; j < HEIGHT; j++)
    {
      memset(&FB[j][0], 0, 2 * (i + 1));
      memcpy(&FB[j][i + 1],
             &image[j][0], (WIDTH - 1 - i) * 2);
    }
  }
}
void left_2_right(int lcd, unsigned long (*image)[WIDTH])
{
  left2right_in(lcd, image);
  left2right_out(lcd, image);
}