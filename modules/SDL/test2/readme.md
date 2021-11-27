#hiding_picture.c
    使用触摸屏lcd显示图片

#ps
#sdl库的安装使用
    以 SDL-1.2 为例，要在嵌入式环境中使用它，只需几步： 
    1，下载：http://www.libsdl.org/release/SDL-1.2.15.tar.gz 
    2，安装三部曲： ./configure --host=arm-none-Linux-gnueabi --prefix=/usr/local/sdl make make install 
    注意，--host 是指定交叉编译器的前缀，--prefix 是指定 SDL 的安装目录，两者都要根据你的具体情况来写
    3，将编译后的目录/usr/local/sdl 全部拷贝到开发板中，设置好库目录的环境变量，即 可使用了。假设将此目录拷贝到开发板的/usr/local/sdl 中，那么环境变量的设置方法是：在 开发板中执行如下命令： export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/sdl/lib
    4, SDL的运行时必须指定MOUSE设备.如使用tslib,则用如下命令设置SDL使用tslib，
        export SDL_MOUSEDRV=TSLIB；如果没有任何输入设备,则环境变量 export SDL_NOMOUSE=1 ,否则初始化会提示Couldn't initialize SDL:Unable to open mouse

#tslib库
    1，从https://sourceforge.net/projects/tslib.berlios/下载源码
    2，编译需要依赖工具automake libtool，系统没有则需要先安装
    3，解压后运行 ./autogen.sh 该脚本将会自动检查当前系统的编译环境信息，最终生成一个 configure 文件
    4，为了避免编译时找不到 ac_cv_func_malloc_0_nonnull 而报错先执行
        echo "ac_cv_func_malloc_0_nonnull=yes" > arm-Linux.cache
    5，执行./configure --host=arm-Linux --cache-file=arm-Linux.cache --prefix=/usr/local/tslib  --host 指明当前系统所使用的交叉编译工具的前缀，根据具体情况而变。 --cache-file 指明运行 configure 脚本时的缓存文件。 --prefix 指明 TSLIB 的安装路径，根据具体情况而变。
    接下来执行 make 和 make instal
    6，编译错误：undefined reference to `rpl_malloc'
    解决方法：执行# ./configure  --host=mipsel-linux  --prefix=/TSLIB后，在config.h中将“#define malloc  rpl_malloc malloc”注释掉，编译通过
    7，编译错误：error: call to ‘__open_missing_mode’ declared with attribute error: open with O_CREAT
    解决方法：
    sudo vim /home/loongson/workspace/tslib/tests/ts_calibrate.c +227将cal_fd = open (calfile, O_CREAT | O_RDWR);改为
    cal_fd = open (calfile, O_CREAT | O_RDWR,0666);
    229行
    将cal_fd = open ("/etc/pointercal", O_CREAT | O_RDWR);
     = open ("/etc/pointercal", O_CREAT | O_RDWR,0666);