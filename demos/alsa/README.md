#alsa
    音频录制和播放例程
    ./alsa_test 1 test.wav 录制音频，貌似虚拟机不支持，还未测试成功
    ./alsa_test 2 zhanggerong.wav 播放音频

#Waring
    需要先安装alsa
    1，下载最新版 ALSA 源代码：ftp://ftp.alsa-project.org/pub/lib/ 2，解压缩，进入源码目录中并依次执行./configure、make 和 make install 
    3，将安装之后的 ALSA 库所在路径（缺省是/usr/lib/i386-Linux-gnu/）添加到环境变量 LD_LIBRARY_PATH 中。 
    4，编译音频程序的时候，包含头文件<alsa/asoundlib.h>，并且链接 alsa 库： 比如： gcc example.c -o example -lasound