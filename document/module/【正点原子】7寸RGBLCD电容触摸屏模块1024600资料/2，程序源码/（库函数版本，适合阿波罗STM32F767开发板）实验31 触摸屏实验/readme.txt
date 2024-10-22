实验器材:
	阿波罗STM32F7开发板
	
实验目的:
	学习电阻式/电容式触摸屏的使用.
	
硬件资源:
	1,DS0(连接在PB1)
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面)
	3,ALIENTEK 2.8/3.5/4.3/7寸LCD模块(包括MCU屏和RGB屏,都支持) 
	4,按键KEY0(PH3)
	5,触摸屏(电阻式/电容式) 

	
实验现象:
	本实验代码,开机的时候先通过LCD ID检测是电容屏（ALIENTEK 4.3寸/7寸电容触摸屏）还是电阻屏（ALIENTEK 2.4/2.8/3.5
	寸电阻屏），然后进入相关的测试。
	如果是电阻屏，则通过24C02的数据判断触摸屏是否已经校准过，如果没有校准，则执行校准程序，校准过后再进入手写程序。如
	果已经校准了，就直接进入手写程序，此时可以通过按动屏幕来实现手写输入。屏幕上会有一个清空的操作区域（RST），点击这
	个地方就会将输入全部清除，恢复白板状态。程序会设置一个强制校准，就是通过按KEY0来实现，只要按下KEY0就会进入强制校准
	程序。
	如果是电容屏，则直接进入手写测试程序，电容触摸屏支持4.3/7寸屏模块（包括MCU屏和RGB屏模块,7寸V1版本模块除外），支持
	5点触摸。电容触摸屏不需要校准，其他操作同电阻触摸屏的测试。 
	
	电阻屏校准方法：
	1，下载本代码。
	2，按KEY0。
	3，依次点击屏幕上的4个十字架（十字+小圈圈）的最中央（最好用笔尖）
	4，直到屏幕提示："Touch Screen Adjust OK!"字样。
	5，校准完成。
	
注意事项:
	1,4.3寸和7寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).
	2,本例程在LCD_Init函数里面(在lcd.c),用到了printf,如果不初始化串口1,将导致液晶无法显示!!  
	3,本例程除不支持ALIENTEK CPLD方案的7寸电容触摸屏（V1版本），其他所有ALIENTEK的液晶模块都支持！！
	 
	 
参考资料：阿波罗STM32F7开发指南-库函数版本.pdf 第三十六章


-------------------------------------------------------------------------------------------

◆其他重要连接：
  开发板光盘资料下载地址（视频+文档+源码等）：http://www.openedv.com/posts/list/13912.htm


◆友情提示：如果您想以后及时免费的收到正点原子所有开发板资料更新增加通知，请关注微信公众平台：
 2种添加方法：（动动手提升您的学习效率，惊喜不断哦）
（1）打开微信->添加朋友->公众号->输入“正点原子”->点击关注
（2）打开微信->添加朋友->输入“alientek_stm32"->点击关注
 具体微信添加方法，请参考帖子：http://www.openedv.com/posts/list/45157.htm
 


						

						淘宝店铺： http://openedv.taobao.com
						           http://eboard.taobao.com
						公司网站：www.alientek.com
						技术论坛：www.openedv.com
                                                微信公众平台：正点原子
						电话：020-38271790
						传真：020-36773971
						广州市星翼电子科技有限公司
						正点原子@ALIENTEK
						     2016-6月