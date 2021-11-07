/*
* @Author: bingo
* @Date:   2021-11-06 00:18:24
* @Last Modified by:   bingo
* @Last Modified time: 2021-11-07 15:10:58
*/
#include "audio_wav.h"
#include "playback.h"

void get_bits_per_sample(wav_format *wav, snd_pcm_format_t *format)
{
    if (wav->format.bits_per_sample == LE_SHORT(16))
    {
        printf("bits_per_sample is:LE_SHORT(16)\n");
        *format = SND_PCM_FORMAT_S16_LE;
    }
    else
    {
        printf("unknown format\n");
    }
}

int set_params(pcm_container *pcm, wav_format *wav)
{
    snd_pcm_hw_params_t *hwparams;
    // uint32_t buffer_time, period_time;

    // A)分配参数空间
    //  以PCM设备能支持的所有配置范围初始化该参数空间
    snd_pcm_hw_params_alloca(&hwparams);
    snd_pcm_hw_params_any(pcm->handle, hwparams);

    //B)设置访问方式为“帧连续交错方式”
    snd_pcm_hw_params_set_access(pcm->handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);

    //C)根据WAV文件的格式信息，设置量化参数
    snd_pcm_format_t format;
    get_bits_per_sample(wav, &format);
    snd_pcm_hw_params_set_format(pcm->handle, hwparams, format);
    pcm->format = format;

    //D)根据WAV文件的格式信息，设置声道数
    snd_pcm_hw_params_set_channels(pcm->handle, hwparams, LE_SHORT(wav->format.channels));
    pcm->channels = LE_SHORT(wav->format.channels);

    //E)根据WAV文件的格式信息，设置采样频率
    //如果声卡不支持WAV文件的采样频率，则选择一个最接近的频率
    uint32_t exact_rate = LE_INT(wav->format.sample_rate);
    snd_pcm_hw_params_set_rate_near(pcm->handle, hwparams, &exact_rate, 0);

    //F)设置buffer大小为声卡支持的最大值
    //并将处理周期设置为buffer的1/4的大小
    snd_pcm_hw_params_get_buffer_size_max(hwparams, &pcm->frames_per_buffer);
    snd_pcm_hw_params_set_buffer_size_near(pcm->handle, hwparams, &pcm->frames_per_buffer);
    pcm->frames_per_period = pcm->frames_per_buffer / 4;
    snd_pcm_hw_params_set_period_size(pcm->handle, hwparams, pcm->frames_per_period, 0);
    snd_pcm_hw_params_get_period_size(hwparams, &pcm->frames_per_period, 0);

    //G)将所设置的参数安装到PCM设备中
    snd_pcm_hw_params(pcm->handle, hwparams);

    //H)由所设置的buffer时间和周期分配相应的大小缓冲区
    pcm->bits_per_sample = snd_pcm_format_physical_width(format);
    pcm->bytes_per_frame = pcm->bits_per_sample / 8 * LE_SHORT(wav->format.channels);
    pcm->period_buf = (uint8_t *)malloc(pcm->frames_per_period * pcm->bytes_per_frame);

    return 0;
}

int check_wav_format(wav_format *wav)//判断音频格式是否合法
{
    if (wav->head.id != RIFF ||
            wav->head.format != WAVE ||
            wav->format.fmt_id != FMT ||
            wav->format.fmt_size != LE_INT(16) ||
            (wav->format.channels != LE_SHORT(1) &&
             wav->format.channels != LE_SHORT(2)) ||
            wav->data.data_id != DATA)
    {
        fprintf(stderr, "not standard wav file.\n");
        return -1;
    }
    return 0;
}

int get_wav_header_info(int fd, wav_format *wav) //获取格式信息
{
    int n1 = read(fd, &wav->head, sizeof(wav->head));
    int n2 = read(fd, &wav->format, sizeof(wav->format));
    int n3 = read(fd, &wav->data, sizeof(wav->data));

    if (n1 != sizeof(wav->head) ||
            n2 != sizeof(wav->format) ||
            n3 != sizeof(wav->data))
    {
        fprintf(stderr, "get_wav_header_info() failed\n");
        return -1;
    }

    if (check_wav_format(wav) < 0)
        return -1;
    printf("the input file is wav\n");
    return 0;
}

ssize_t read_pcm_from_wav(int fd, void *buf, size_t count)
{
    ssize_t result = 0, res;

    while (count > 0)
    {
        if ((res = read(fd, buf, count)) == 0)
            break;
        if (res < 0)
            return result > 0 ? result : res;
        count -= res;
        result += res;
        buf = (char *)buf + res;
    }
    return result;
}

int write_pcm_to_device(pcm_container *pcm, int load)
{
    int rc;

    rc = snd_pcm_writei(pcm->handle, pcm->period_buf, load);
    if (rc == -EPIPE) {
        printf("underrun occured\n");
    }
    else if (rc < 0) {
        printf("error from writei: %s\n", snd_strerror(rc));
    }
    return rc;
}

void play_wav(pcm_container *pcm, wav_format *wav, int fd)
{
    int load, ret;
    off64_t written = 0;
    off64_t c;
    off64_t total_bytes = LE_INT(wav->data.data_size);

    uint32_t period_bytes = pcm->frames_per_period * pcm->bytes_per_frame;

    printf("total_bytes=%lld period_bytes=%d\n", total_bytes, period_bytes);
    load = 0;
    while (written < total_bytes)
    {
        printf("read ing data:len=%lld %.2f%%\n", written, ((float)written / total_bytes) * 100.0);
        //一次循环读取一个完整的周期数据
        do
        {
            c = total_bytes - written;
            if (c > period_bytes)
                c = period_bytes;
            c -= load;

            if (c == 0)
                break;
            ret = read_pcm_from_wav(fd, pcm->period_buf + load, c);

            if (ret < 0)
            {
                fprintf(stderr, "read() failed\n");
                exit(-1);
            }

            if (ret == 0)
                break;
            load += ret;
        } while ((size_t)load < period_bytes);

        /* Transfer to size frame */
        load = load / pcm->bytes_per_frame;
        ret = write_pcm_to_device(pcm, load);
        if (ret != load)
            break;

        ret = ret * pcm->bytes_per_frame;
        written += ret;
        load = 0;
    }
}

int play(const char * file)
{
    //准备好保持文件信息及处理音频设备的结构体
    wav_format *wav = calloc(1, sizeof(wav_format));
    pcm_container *playback = calloc(1, sizeof(pcm_container));

    //获取音频文件的格式信息
    int fd = open(file, O_RDONLY);
    get_wav_header_info(fd, wav);

    //设置音频设备
    snd_pcm_open(&playback->handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    set_params(playback, wav);

    //将音频中除文件格式信息之外的音频数据读出，并写入音频设备
    play_wav(playback, wav, fd);

    //结束写入操作 并释放相关的内存资源
    snd_pcm_drain(playback->handle);
    snd_pcm_close(playback->handle);

    free(playback->period_buf);
    free(playback);
    free(wav);
    close(fd);

    return 0;
}
