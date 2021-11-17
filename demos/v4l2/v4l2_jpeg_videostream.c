/*
* @Author: bingo
* @Date:   2021-11-14 15:38:20
* @Last Modified by:   bingo
* @Last Modified time: 2021-11-17 23:54:11
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include <linux/fb.h>
#include <linux/videodev2.h>
#include <linux/input.h>
#include <sys/ioctl.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <fcntl.h>
#include <setjmp.h>

int shooting(unsigned char *jpegdata, int size, unsigned int *fb_mem)
{
	struct timeval tv;
	char buf[64];

	gettimeofday(&tv, NULL);
	sprintf(buf, "./picture/capture-%ld-%ld.jpg", tv.tv_sec, tv.tv_usec);
	FILE *fp = fopen(buf, "w");
	if (fp == NULL)
	{
		perror("fopen()");
		return 0;
	}
	size_t len  = fwrite(jpegdata, size, 1, fp);
	printf("shooting size = %d write len = %lu\n", size, len);
	fclose(fp);

	return 0;
}

void show_camfmt(struct v4l2_format *fmt)
{
	printf("camera width : %d \n", fmt->fmt.pix.width);
	printf("camera height: %d \n", fmt->fmt.pix.height);

	switch (fmt->fmt.pix.pixelformat)
	{
	case V4L2_PIX_FMT_JPEG:
		printf("camera pixelformat: V4L2_PIX_FMT_JPEG\n");
		break;
	case V4L2_PIX_FMT_YUYV:
		printf("camera pixelformat: V4L2_PIX_FMT_YUYV\n");
		break;
	case V4L2_PIX_FMT_MJPEG:
		printf("camera pixelformat: V4L2_PIX_FMT_MJPEG\n");
		break;
	default:
		printf("camera pixelformat: %d\n", fmt->fmt.pix.pixelformat);
	}
}

int main(int argc, char const *argv[])
{
	//打开lcd设备
	int lcd = open("/dev/fb0", O_RDWR);

	//获取lcd显示器的设备参数
	struct fb_var_screeninfo lcdinfo;
	ioctl(lcd, FBIOGET_VSCREENINFO, &lcdinfo);

	//申请一块适当跟lcd尺寸一样大小的显存
	unsigned int *fb_mem = mmap(NULL, lcdinfo.xres * lcdinfo.yres * lcdinfo.bits_per_pixel / 8,
	                            PROT_READ | PROT_WRITE, MAP_SHARED, lcd, 0);

	//将屏幕刷成黑色
	// unsigned long black = 0x0;
	unsigned int i;
	// for (i = 0; i < lcdinfo.xres * lcdinfo.yres; i++)
	// {
	//     memcpy(fb_mem + i, &black, sizeof(unsigned long));
	// }

	//1，打开摄像头设备文件
	int cam_fd = open("/dev/video0", O_RDWR);

	struct v4l2_fmtdesc *a = calloc(1, sizeof(*a));
	a->index = 0;
	a->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	int ret;
	while ((ret = ioctl(cam_fd, VIDIOC_ENUM_FMT, a)) == 0)
	{
		a->index++;
		printf("pixelformat: \"%c%c%c%c\"\n",
		       (a->pixelformat >> 0) & 0XFF,
		       (a->pixelformat >> 8) & 0XFF,
		       (a->pixelformat >> 16) & 0XFF,
		       (a->pixelformat >> 24) & 0XFF);

		printf("description: %s\n\n", a->description);
	}
	printf("\n");

	//2，获取摄像头当前的采集格式
	struct v4l2_format *fmt = calloc(1, sizeof(*fmt));
	fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(cam_fd, VIDIOC_G_FMT, fmt);
	show_camfmt(fmt);//显示具体参数

	//3,配置摄像头的采集格式为JPEG
	bzero(fmt, sizeof(*fmt));
	fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt->fmt.pix.width = 640;//lcdinfo.xres;
	fmt->fmt.pix.height = 480;//lcdinfo.yres;
	fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	fmt->fmt.pix.field = V4L2_FIELD_INTERLACED;
	ioctl(cam_fd, VIDIOC_S_FMT, fmt);

	//2，获取摄像头当前的采集格式
	bzero(fmt, sizeof(*fmt));
	fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(cam_fd, VIDIOC_G_FMT, fmt);
	show_camfmt(fmt);//显示具体参数
	if (fmt->fmt.pix.pixelformat != V4L2_PIX_FMT_MJPEG)
	{
		printf("\nthe pixel format is NOT MJPEG.\n\n");
		exit(1);
	}

	//4,设置即将要申请的摄像头缓存的参数
	int nbuf = 3;
	struct v4l2_requestbuffers reqbuf;
	bzero(&reqbuf, sizeof(reqbuf));
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	reqbuf.count = nbuf;

	//5，使用该参数reqbuf来申请缓存
	ioctl(cam_fd, VIDIOC_REQBUFS, &reqbuf);

	//6,根据刚设置的reqbuf.count的值，来定义相应数量的struct v4l2_buffer
	//每一个struct v4l2_buffer对应内摄像头驱动中的一个缓存
	struct v4l2_buffer buffer[nbuf];
	int length[nbuf];
	unsigned char *start[nbuf];

	system("mkdir ./picture");
	for (i = 0; i < nbuf; i++)
	{
		bzero(&buffer[i], sizeof(buffer[i]));
		buffer[i].type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buffer[i].memory = V4L2_MEMORY_MMAP;
		buffer[i].index = i;
		ioctl(cam_fd, VIDIOC_QUERYBUF, &buffer[i]);

		length[i] = buffer[i].length;
		start[i] = mmap(NULL, buffer[i].length,
		                PROT_READ | PROT_WRITE,
		                MAP_SHARED, cam_fd, buffer[i].m.offset);
		ioctl(cam_fd, VIDIOC_QBUF, &buffer[i]);
	}

	//7,启动摄像头数据采集
	enum v4l2_buf_type vtype = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(cam_fd, VIDIOC_STREAMON, &vtype);

	struct v4l2_buffer v4lbuf;
	bzero(&v4lbuf, sizeof(v4lbuf));
	v4lbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4lbuf.memory = V4L2_MEMORY_MMAP;

	//循环读取摄像头是数据
	i = 0;
	while (1)
	{
		//从队列中取出填满数据的缓存
		v4lbuf.index = i % nbuf;
		//VIDIOC_DQBUF在摄像头没数据的时候会阻塞
		printf("waiting data\n");
		ioctl(cam_fd, VIDIOC_DQBUF, &v4lbuf);
		printf("get data\n");
		shooting(start[i % nbuf], length[i % nbuf], fb_mem); //显示到LCD

		//将已经读取过的数据的缓存块重新置入队列中
		v4lbuf.index = i % nbuf;
		ioctl(cam_fd, VIDIOC_QBUF, &v4lbuf);

		i++;
	}

	close(cam_fd);

	return 0;
}