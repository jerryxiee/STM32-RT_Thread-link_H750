#include <ld3320.h>

VoiceRecognition Voice;                         //声明一个语音识别对象

#define Led 8                                   //定义LED控制引脚

void setup() 
{
    pinMode(Led,OUTPUT);                        //初始化LED引脚为输出模式
    digitalWrite(Led,LOW);                      //LED引脚低电平
    Serial.begin(9600);                        //配置9600
    Serial.print("Uart start!");                
    Voice.init();                               //初始化VoiceRecognition模块   
    Voice.addCommand("kai deng",0);             //添加指令，参数（指令内容，指令标签（可重复））
    Voice.addCommand("guan deng",1);            //添加指令，参数（指令内容，指令标签（可重复））
    Voice.addCommand("ni hao",2);               //添加垃圾词汇
    Voice.addCommand("wei wei",3);              //添加垃圾词汇
    Voice.start();//开始识别
    Serial.print("Initialized\n");
}
void loop() {
  switch(Voice.read())                          //判断识别
  {
    case 0:                                     //若是指令“kai deng”
	digitalWrite(Led,HIGH);                 //点亮LED
        Serial.print("LED ON");
        break;
    case 1:                                     //若是指令“guan deng”
	digitalWrite(Led,LOW);//熄灭LED
        Serial.print("LED OFF");
        break;   
    case 2:
        Serial.print("ni hao");
        break;   
    case 3:
        Serial.print("wei wei");
        break;   
    default:
        break;
  }
}
