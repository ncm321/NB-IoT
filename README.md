# NB-IoT
#项目名称
基于NB-IoT即智慧农业项目，通过检测环境温湿度，对农作物进行自动灌溉。
##1.克隆仓库
git clone https://giyhub.com/ncm321/NB-IoT.git
##2.下载需要的库（gpiod）
我需要学着写到./bash里面
##3.进入src源文件夹，执行makefile生成库文件，可在../lib文件夹中看见
cd src
make
ls ../lib
##4.退出src执行makefile生成nbiot的可执行文件
cd ..
make
ls nbiot
##5.执行nbiot，注意添加参数,-h打印帮助信息，-n设备路径，-t采样温湿度间隔
./nbiot -n "/dev/ttyUSB0" -t 10
