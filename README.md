# STM32-RT_Thread-link_H750
RT-Thread x STM32 全连接创意创客大赛

## 作品设计要求

### 模块类

1.  语言识别模块 LD3320（包括麦克风和扬声器）
2.  OV5640摄像头模块（ATK）
3.  ATK-7 RGB TFTLCD （分辨率：1024*600）（ATK）
4.  光电传感器（三对以上）
5.  移动端上位机（小程序或者安卓）
6.  web端网页
7.  物联网平台（阿里云）
8.  红外自学习（[对接互联网平台](https://irext.net/)）

### 功能类

1.  语言识别与回应系统，可离线识别语言并产生对话
2.  摄像头可拍摄照片，并将其上传到服务器（阿里云）内，供使用者远程查看
3.  光电传感器，可以检测目前是否有人经过
4.  可以通过移动端上位机获取单片机数据
5.  通过web端网页配置单片机设置，并获取数据
6.  红外自学习，语音控制空调等红外设备
7.  小型存储管理器，可以进行一些小型的私密文件备份
8.  邮箱系统，每天工作的日志等文件都会通过邮件系统统一发送到使用者邮箱

## 资料

1.  官方[SDK](https://github.com/RT-Thread-Studio/sdk-bsp-stm32h750-realthread-artpi)
2.  LD3320 （位于/document/module/LD3320）
3.  ATK-7 RGB TFTLCD （分辨率：1024*600）（ATK）（位于/document/module/【正点原子】7寸RGBLCD电容触摸屏模块1024600资料）
4.  OV5640摄像头模块（ATK）（位于/document/module/【正点原子】OV5640单目摄像头模块资料）
5.  [在ART-Pi H750上移植TouchGFX](https://blog.csdn.net/sinat_31039061/article/details/108599356)

## 提高项

1.  openmv
2.  语音与物联网对接，可以进行语音交互