
#ifndef _HEAD4ANIMATION_H_
#define _HEAD4ANIMATION_H_ 3
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#define SCREEN_SIZE 480*270
#define WIDTH 480
#define HEIGHT 270
#define BLIND 2


struct argument
{
    unsigned long (*FB)[WIDTH];
    unsigned long (*image)[WIDTH];
    int offset;
    int flag;
};

void falling_down_in(int lcd, unsigned long (*image)[WIDTH]);
void falling_down_out(int lcd, unsigned long (*image)[WIDTH]);

void falling_down(int lcd, unsigned long (*image)[WIDTH]);
void floating_up_in(int lcd, unsigned long (*image)[WIDTH]);
void floating_up_out(int lcd, unsigned long (*image)[WIDTH]);
void floating_up(int lcd, unsigned long (*image)[WIDTH]);
void left2right_in(int lcd, unsigned long (*image)[WIDTH]);
void left2right_out(int lcd, unsigned long (*image)[WIDTH]);
void left_2_right(int lcd, unsigned long (*image)[WIDTH]);

void right2left_in(int lcd, unsigned long (*image)[WIDTH]);
void right2left_out(int lcd, unsigned long (*image)[WIDTH]);
void right2left(int lcd, unsigned long (*image)[WIDTH]);
void blind_window_in(int lcd, unsigned long (*image)[WIDTH]);
void blind_window_out(int lcd, unsigned long (*image)[WIDTH]);
void blind_window(int lcd, unsigned long (*image)[WIDTH]);

#endif