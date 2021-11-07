#include "audio_wav.h"
#include "playback.h"

//准备WAV格式参数
void prepare_wav_params(wav_format *wav)
{
    wav->format.fmt_id = FMT;
    wav->format.fmt_size = LE_INT(16);
    wav->format.fmt = LE_SHORT(WAV_FMT_PCM);
    wav->format.channels = LE_SHORT(2); //音轨数目
    wav->format.sample_rate = LE_INT(44100);    //采样频率
    wav->format.bits_per_sample = LE_SHORT(16); //量化位数
    wav->format.block_align = LE_SHORT(wav->format.channels * wav->format.bits_per_sample / 8);
    wav->format.byte_rate = LE_INT(wav->format.sample_rate * wav->format.block_align);
    wav->data.data_id = DATA;
    wav->data.data_size = LE_INT(DURATION_TIME * wav->format.byte_rate);
    wav->head.id = RIFF;
    wav->head.format = WAVE;
    wav->head.size = LE_INT(36 + wav->data.data_size);
}

//设置wav格式参数
void set_wav_params(pcm_container *sound, wav_format *wav)
{
    //1:定义并分配一个硬件参数空间
    snd_pcm_hw_params_t *hwparams;//声明一个指向采样频率、量化级、音轨数目等配置空间的指针
    snd_pcm_hw_params_alloca(&hwparams);//给参数配置分配空间

    //2:初始化硬件参数空间
    snd_pcm_hw_params_any(sound->handle, hwparams); //根据当前PCM设备的情况初始化

    //3:设置访问模式位交错模式（即帧连续模式）
    snd_pcm_hw_params_set_access(sound->handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED); //设置访问模式为交错模式


    //4:设置量化参数
    snd_pcm_format_t pcm_format = SND_PCM_FORMAT_S16_LE;
    snd_pcm_hw_params_set_format(sound->handle, hwparams, pcm_format);
    sound->format = pcm_format;

    //5:设置音轨数目
    snd_pcm_hw_params_set_channels(sound->handle, hwparams, LE_SHORT(wav->format.channels));
    sound->channels = LE_SHORT(wav->format.channels);

    //6:设置采样频率
    //注意：最终被设置的频率被存放在exact_rate中
    uint32_t exact_rate = LE_INT(wav->format.sample_rate);
    snd_pcm_hw_params_set_rate_near(sound->handle, hwparams, &exact_rate, 0);

    //7:设置buffer_size为声卡支持最大值
    snd_pcm_uframes_t buffer_size;
    snd_pcm_hw_params_get_buffer_size_max(hwparams, &buffer_size);
    snd_pcm_hw_params_set_buffer_size_near(sound->handle, hwparams, &buffer_size);

    //8:根据buffer_size设置period size
    snd_pcm_uframes_t period_size = buffer_size / 4;
    snd_pcm_hw_params_set_period_size_near(sound->handle, hwparams, &period_size, 0);

    //9:安装pcm设备参数
    snd_pcm_hw_params(sound->handle, hwparams);

    //10:获取buffer size 和period size
    //注意:他们均以frame为单位（frame = 音轨数 * 量化级）
    snd_pcm_hw_params_get_buffer_size(hwparams, &sound->frames_per_buffer);
    snd_pcm_hw_params_get_period_size(hwparams, &sound->frames_per_period, 0);

    //11:保存一些参数
    sound->bits_per_sample = snd_pcm_format_physical_width(pcm_format);
    sound->bytes_per_frame = sound->bits_per_sample / 8 * wav->format.channels;

    //12:分配一个周期数据空间
    sound->period_buf = (uint8_t *)calloc(1, sound->frames_per_period * sound->bytes_per_frame);
}

snd_pcm_uframes_t read_pcm_data(pcm_container *sound, snd_pcm_uframes_t frames)
{
    snd_pcm_uframes_t exact_frames = 0;
    snd_pcm_uframes_t n = 0;

    uint8_t *p = sound->period_buf;
    while (frames > 0)
    {
        n = snd_pcm_readi(sound->handle, p, frames);

        frames -= n;
        exact_frames += n;
        p += (n * sound->bytes_per_frame);
    }

    return exact_frames;
}

//从PCM设备录取音频数据，并写入fd中
void recorder(int fd, pcm_container *sound, wav_format *wav)
{
    //1:写WAV格式的文件头
    write(fd, &wav->head, sizeof(wav->head));
    write(fd, &wav->format, sizeof(wav->format));
    write(fd, &wav->data, sizeof(wav->data));

    //2:写PCM数据
    uint32_t total_bytes = wav->data.data_size;

    while (total_bytes > 0)
    {
        uint32_t total_frames = total_bytes / (sound->bytes_per_frame);
        snd_pcm_uframes_t n = MIN(total_frames, sound->frames_per_period);

        uint32_t frames_read = read_pcm_data(sound, n);
        write(fd, sound->period_buf, frames_read * sound->bytes_per_frame);
        total_bytes -= (frames_read * sound->bytes_per_frame);
    }
}

//录制音频
int record(const char *file)
{
    //1:打开WAV格式文件
    int fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0777);
    if (fd < 0)
    {
        perror("open faild!");
    }

    //2:打开PCM设备文件
    pcm_container *sound = calloc(1, sizeof(pcm_container));
    snd_pcm_open(&sound->handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (sound->handle == NULL)
    {
        printf("sound handle create faild\n");
        close(fd);
        return 0;
    }

    //3:准备并设置WAV个数参数
    wav_format *wav = calloc(1, sizeof(wav_format));
    prepare_wav_params(wav);
    set_wav_params(sound, wav);

    //4:开始从PCM设备"plughw:0,0"录制音频数据
    //并且以WAV格式写到fd中
    recorder(fd, sound, wav);

    //5:释放相关资源
    snd_pcm_drain(sound->handle);
    close(fd);
    snd_pcm_close(sound->handle);
    free(sound->period_buf);
    free(sound);
    free(wav);

    return 0;
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("Usage:%s <1:record 2:play> <wav-file>\n", argv[0]);
        exit(1);
    }

    if (1 == atoi(argv[1]))
    {
        record(argv[2]);
    }
    else if (2 == atoi(argv[1]))
    {
        play(argv[2]);
    }
    else
    {
        printf("unknow action:%d\n", atoi(argv[1]));
    }

    return 0;
}