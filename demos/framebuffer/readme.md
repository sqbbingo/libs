#get_screeninfo.c
    获取lcd显示屏的参数，并显示图片
#check_lcd.c
    间隔显示红、蓝、绿三色检查屏幕是否有坏点
#pic_show.c
    显示二进制格式的图片
#jpeg_show.c
    显示jpeg图片

#ps
    get_screeninfo.c 和pic_show.c显示的图片需要使用软件image2lcd转换
    使用这个工具需要注意的几点： 
    1，输出数据类型选择“二进制”，使得生成一个包含纯图片像素数据的*.bin 文件。 
    2，扫描模式选择“水平扫描”。 
    3，输出灰度选择“32 位真色彩”，使得图片中的每一个像素用 32 位数据来表示。 
    4，最大宽度和高度分别填写“800”和“480”（以群创 AT070TN92-7 英寸液晶显示 屏为例）。 
    5，取消勾选“包含图像头数据”。

    对 jpeg 压缩格式的解码库 API，简单地讲有这么几个步 骤：
    1，下载 jpeg 库：http://www.ijg.org/files/jpegsrc.v9d.tar.gz
    2，安装 jpeg 库：
        2.1 进入解压后的目录：cd jpeg-9a/ 
        2.2 配置交叉环境：./configure --host=arm-none-Linux-gnueabi 
        2.3 编译并安装：make && make install 
    3，将/usr/local/include 和/usr/local/lib 下关于 jpeg 的头文件和库拷贝到开发板。 
    4，在程序中加入对 jpeg 压缩图片的解压代码