#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "uvc_gadget.h"
#include "uvc_video.h"


//#define UVC_BUILD_TEST	1
#ifndef UVC_BUILD_TEST
	#include "hal_stream_api.h"
	#include "ringbuf.h"
#endif

int uvc_video_fill_buffer_userptr(struct uvc_device* dev, struct v4l2_buffer* buf)
{
    switch (dev->pix_fmt)
    {
		case V4L2_PIX_FMT_MJPEG:
		case V4L2_PIX_FMT_H264:
		case V4L2_PIX_FMT_H265:
		case V4L2_PIX_FMT_YUYV:
		case V4L2_PIX_FMT_YUV420:
		{
			// TODO
			#ifndef UVC_BUILD_TEST
			int retry_count = 0;
			int i, ret;
			
			buf->bytesused = 0;
			hal_stream_rhandle_t reader_handle;
			hal_stream_frame_info_t *pkg;
			
			//printf("func = %s, line = %d\n", __FUNCTION__, __LINE__);
			reader_handle = (hal_stream_rhandle_t)dev->video_handle;
			retry_count = 5;
			
			for (i = 0; i < retry_count; i++)
			{
				ret = hal_stream_get_frame(reader_handle, &pkg);
				if (ret != 0) {
					usleep(100000);
					continue;
				}

				buf->bytesused = pkg->data_len;
				memcpy(dev->mem[buf->index], pkg->data, pkg->data_len);
				//buf->m.userptr = (unsigned long)pkg->data;
				buf->m.userptr = (unsigned long)dev->mem[buf->index];
				
				hal_stream_release_frame(reader_handle);
				break;
			}
			#endif
			// END
		} 
			break;
		default:
			printf("what up pix_fmt %d.\n", dev->pix_fmt);
			break;
    }	
	
	return 0;
}



int uvc_video_init(struct uvc_device* dev)
{
	// TODO
	printf("func = %s, line = %d\n", __FUNCTION__, __LINE__);
	#ifndef UVC_BUILD_TEST
	hal_stream_rhandle_t reader_handle;
    unsigned int page_size, buffer_size;
	int i;
	
	reader_handle = hal_stream_read_init(1);
	if (reader_handle == NULL) {
		return -1;
	}
	dev->video_handle = (void *)reader_handle;

	// 初始化V4L2_MEMORY_USERPTR视频缓冲区
    page_size = getpagesize();
    buffer_size = (dev->image_size + page_size - 1) & ~(page_size - 1);
    for (i = 0; i < dev->nbufs; i++) 
	{
		dev->mem[i] = (void *)memalign(page_size, buffer_size);
        if (NULL == dev->mem[i])
        {
            printf("failed to malloc dev->mem field\n");
			return -1;
        }
		dev->length[i] = buffer_size;
    }
	#endif
	// END
	
	return 0;
}


int uvc_video_deinit(struct uvc_device* dev)
{
	// TODO
	#ifndef UVC_BUILD_TEST
	int i;
	hal_stream_read_deinit((hal_stream_rhandle_t)dev->video_handle);

    for (i = 0; i < dev->nbufs; i++) 
	{
		if (dev->mem[i]) {
			free(dev->mem[i]);
		}
    }
	#endif
	// END
	
	return 0;
}


