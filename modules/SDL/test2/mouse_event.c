/*
* @Author: bingo
* @Date:   2021-11-21 16:40:02
* @Last Modified by:   bingo
* @Last Modified time: 2021-11-21 16:44:13
*/
#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define WIDTH  480
#define HEIGHT 270
#define BPP    16

SDL_Surface *screen;
SDL_Surface *image ;
SDL_Surface *left, *right;


SDL_Surface *load_image(const char *filename)
{
    return SDL_DisplayFormat(SDL_LoadBMP(filename));
}

void show_bmp(const char *filename)
{
    // 将屏幕填充为黑色
    uint32_t black_pixel = SDL_MapRGB(screen->format, 0, 0, 0);
    SDL_FillRect(screen, &screen->clip_rect, black_pixel);

    // 加载图片
    image = load_image(filename);

    // 设置图片的位置
    static SDL_Rect rect = {0, 0};
    SDL_BlitSurface(image, NULL, screen, &rect);

    // 设置左右两个小箭头的位置
    static SDL_Rect left_pos = {100, 200};
    static SDL_Rect right_pos = {700, 200};
    SDL_BlitSurface(left, NULL, screen, &left_pos);
    SDL_BlitSurface(right, NULL, screen, &right_pos);

    // 刷新screen，显示其上的图像信息
    SDL_Flip(screen);
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <bmp directories>\n", argv[0]);
        exit(0);
    }


    SDL_Init(SDL_INIT_EVERYTHING);

    screen = SDL_SetVideoMode(WIDTH, HEIGHT,
                              BPP, SDL_SWSURFACE);

    const char *bmp_files[] = {"1.bmp", "2.bmp",
                               "3.bmp", "4.bmp"
                              };

    chdir(argv[1]);
    left = load_image("left.bmp");
    right = load_image("right.bmp");


    // 将image blit到screen上
    SDL_Rect rect = {0, 0};
    SDL_BlitSurface(image, NULL, screen, &rect);

    SDL_Rect left_pos = {100, HEIGHT / 2};
    SDL_Rect right_pos = {WIDTH - 100, HEIGHT / 2};

    // 设置白色为透明
    int32_t key = SDL_MapRGB(screen->format,
                             0xff /* 红色 */,
                             0xff /* 绿色 */,
                             0xff /* 蓝色 */);
    SDL_SetColorKey(left , SDL_SRCCOLORKEY, key);
    SDL_SetColorKey(right, SDL_SRCCOLORKEY, key);

    // 显示第一张图片
    show_bmp(bmp_files[0]);

    // 阻塞等待鼠标的点击
    int i = 0;
    SDL_Event event;
    while (1)
    {
        SDL_WaitEvent(&event);

        // 按下向左小箭头，切换上一张图片
        if (event.button.type == SDL_MOUSEBUTTONUP &&
                event.button.button == SDL_BUTTON_LEFT &&
                event.button.y >= HEIGHT / 2 &&
                event.button.y <= (HEIGHT / 2 + 30))
        {
            if (event.button.x >= 100 &&
                    event.button.x <= 160)
            {
                i = (i == 0) ? 3 : (i - 1);
            }
            if (event.button.x >= (WIDTH - 100) &&
                    event.button.x <= (WIDTH - 100 + 60))
            {
                i = (i + 1) % 4;
            }

            SDL_FreeSurface(image);
            show_bmp(bmp_files[i]);
        }

        // 按右键退出程序
        if (event.button.type == SDL_MOUSEBUTTONUP &&
                event.button.button == SDL_BUTTON_RIGHT)
        {
            break;
        }
    }

    return 0;
}
