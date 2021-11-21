/*
* @Author: bingo
* @Date:   2021-11-21 13:33:18
* @Description: 使用tslib库API捕获触摸屏坐标，控制SDL显示一张图片的对应位置。
* @Last Modified by:   bingo
* @Last Modified time: 2021-11-21 13:59:46
*/

#include <stdio.h>
#include <signal.h>
#include <stdbool.h>

#include "tslib.h"
#include "SDL.h"

#define DEBUG_HERE printf("%s %d\n",__FUNCTION__,__LINE__)
#define LCD_WIDTH  480
#define LCD_HEIGHT 270

SDL_Surface *screen = NULL;
SDL_Surface *image  = NULL;

struct tsdev *init_ts(void)
{
    char *tsdevice = getenv("TSLIB_TSDEVICE");
    if (tsdevice == NULL)
    {
        tsdevice = "/dev/input/event0";
    }
    printf("tsdevice = %s\n", tsdevice);
    struct tsdev *ts = ts_open(tsdevice, 0);
    if (!ts)
    {
        perror("ts_open");
        exit(1);
    }
    ts_config(ts);

    return ts;
}

void quit(int sig)
{
    SDL_FreeSurface(screen);
    SDL_FreeSurface(image);
    SDL_Quit();

    exit(0);
}

int main(int argc, char const *argv[])
{
    int ret;

    if (argc != 2)
    {
        printf("Usage: %s <bmp-file>\n", argv[0]);
        exit(0);
    }

    signal(SIGINT, quit);

    // 初始化视频子系统
    ret = SDL_Init(SDL_INIT_VIDEO);
    if (ret < 0)
    {
        printf("SDL_Init failed\n");
        printf("%s\n", SDL_GetError() );
        exit(0);
    }

    // 根据当前LCD的参数，创建视窗surface
    screen = SDL_SetVideoMode(LCD_WIDTH, LCD_HEIGHT,
                              0, SDL_ANYFORMAT | SDL_SWSURFACE);
    if (screen == NULL)
    {
        printf("SDL_SetVideoMode failed\n");
        exit(0);
    }

    // 装载一张bmp图像，并用一个surface来表示他
    image = SDL_LoadBMP(argv[1]);
    if (image == NULL)
    {
        printf("SDL_LoadBMP failed\n");
        exit(0);
    }

    // 1, image_offset规定了图片要显示的矩形部分
    // 2, background_offset规定了图像要显示在视窗的位置
    SDL_Rect image_offset;
    SDL_Rect backgroud_offset;

    // 初始化触摸屏
    struct tsdev *ts = init_ts();
    struct ts_sample samp;

    // 产生一个RGB值为000（黑色）的像素
    // uint32_t black_pixel = SDL_MapRGB(screen->format, 0, 0, 0);

    // 将屏幕刷成黑色
    // SDL_FillRect(screen, &screen->clip_rect, black_pixel);

    printf("press ctrl+c to quit.\n");

#if 1
    // 1, x和y规定了图像要显示的矩形的左上角坐标
    // 2, w和h规定了以(x,y)为左上角的矩形的宽和高
    image_offset.x = 0;
    image_offset.y = 0;
    image_offset.w = LCD_WIDTH;
    image_offset.h = LCD_HEIGHT;

    // 1，x和y规定了图像surface放在视窗的左上角坐标
    // 2，w和h都是作废的。
    backgroud_offset.x = 0;
    backgroud_offset.y = 0;

    // 将图像（image）blit到屏幕上（screen）
    SDL_BlitSurface(image, &image_offset, screen, &backgroud_offset);

    // 显示screen上的元素
    SDL_Flip(screen);
#endif

    while (1)
    {
        ts_read(ts, &samp, 1);

        // 1, x和y规定了图像要显示的矩形的左上角坐标
        // 2, w和h规定了以(x,y)为左上角的矩形的宽和高
        image_offset.x = samp.x - 20;
        image_offset.y = samp.y - 20;
        image_offset.w = 40;
        image_offset.h = 40;

        // 1，x和y规定了图像surface放在视窗的左上角坐标
        // 2，w和h都是作废的。
        backgroud_offset.x = samp.x - 20;
        backgroud_offset.y = samp.y - 20;

        // 将图像（image）blit到屏幕上（screen）
        SDL_BlitSurface(image, &image_offset, screen, &backgroud_offset);

        // 显示screen上的元素
        SDL_Flip(screen);
    }

    return 0;
}