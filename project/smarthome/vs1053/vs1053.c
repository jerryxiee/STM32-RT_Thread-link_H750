/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-17     xqyjlj       the first version
 */
#include <board.h>
#include "vs1053.h"

#define DBG_COLOR
#define DBG_TAG "vs1053"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

SPI_HandleTypeDef SPI2_Handler;  //SPI2¾ä±ú
_vs10xx_obj vsset = { 250,    //音量:220
        6,      //低音上线 60Hz
        15,     //低音提升 15dB
        10,     //高音下限 10Khz
        15,     //高音提升 10.5dB
        0,      //空间效果
        1,      //板载喇叭默认打开.
        };

////////////////////////////////////////////////////////////////////////////////
//移植时候的接口
//data:要写入的数据
//返回值:读到的数据
uint8_t VS_SPI_ReadWriteByte(uint8_t data)
{
    uint8_t Rxdata;
    HAL_SPI_TransmitReceive(&SPI2_Handler, &data, &Rxdata, 1, 1000);
    return Rxdata;
}
//SD卡初始化的时候,需要低速
void VS_SPI_SpeedLow(void)
{
    __HAL_SPI_DISABLE(&SPI2_Handler);
    SPI2_Handler.Instance->CFG1 &= ~(0X7 << 28);
    SPI2_Handler.Instance->CFG1 |= SPI_BAUDRATEPRESCALER_256;
    __HAL_SPI_ENABLE(&SPI2_Handler);
}
//SD卡正常工作的时候,可以高速了
void VS_SPI_SpeedHigh(void)
{
    __HAL_SPI_DISABLE(&SPI2_Handler);
    SPI2_Handler.Instance->CFG1 &= ~(0X7 << 28);
    SPI2_Handler.Instance->CFG1 |= SPI_BAUDRATEPRESCALER_128;
    __HAL_SPI_ENABLE(&SPI2_Handler);
}
void VS_Sine_Test(void);
//初始化VS10XX的IO口
int VS_Init(void)
{
    rt_pin_mode(VS_DQ, PIN_MODE_INPUT);
    rt_pin_mode(VS_RST, PIN_MODE_OUTPUT);
    rt_pin_mode(VS_XCS, PIN_MODE_OUTPUT);
    rt_pin_mode(VS_XDCS, PIN_MODE_OUTPUT);

    SPI2_Handler.Instance=SPI2;
    SPI2_Handler.Init.Mode=SPI_MODE_MASTER;
    SPI2_Handler.Init.Direction=SPI_DIRECTION_2LINES;
    SPI2_Handler.Init.DataSize=SPI_DATASIZE_8BIT;
    SPI2_Handler.Init.CLKPolarity=SPI_POLARITY_HIGH;
    SPI2_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;
    SPI2_Handler.Init.NSS=SPI_NSS_SOFT;
    SPI2_Handler.Init.NSSPMode=SPI_NSS_PULSE_DISABLE;
    SPI2_Handler.Init.MasterKeepIOState=SPI_MASTER_KEEP_IO_STATE_ENABLE;
    SPI2_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_256;
    SPI2_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;
    SPI2_Handler.Init.TIMode=SPI_TIMODE_DISABLE;
    SPI2_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;
    SPI2_Handler.Init.CRCPolynomial=7;
    HAL_SPI_Init(&SPI2_Handler);

    __HAL_SPI_ENABLE(&SPI2_Handler);

    VS_SPI_SpeedLow();
    VS_Sine_Test();
    return RT_EOK;
}
INIT_APP_EXPORT(VS_Init);
////////////////////////////////////////////////////////////////////////////////
//软复位VS10XX
void VS_Soft_Reset(void)
{
    uint8_t retry = 0;
    while (rt_pin_read(VS_DQ) == 0);                    //等待软件复位结束
    VS_SPI_ReadWriteByte(0Xff);         //启动传输
    retry = 0;
    while (VS_RD_Reg(SPI_MODE) != 0x0800)  // 软件复位,新模式
    {
        VS_WR_Cmd(SPI_MODE, 0x0804);     // 软件复位,新模式
        rt_thread_mdelay(2);     //等待至少1.35ms
        if (retry++ > 100)
            break;
    }
    while (rt_pin_read(VS_DQ) == 0);     //等待软件复位结束
    retry = 0;
    while (VS_RD_Reg(SPI_CLOCKF) != 0X9800)     //设置VS10XX的时钟,3倍频 ,1.5xADD
    {
        VS_WR_Cmd(SPI_CLOCKF, 0X9800);   //设置VS10XX的时钟,3倍频 ,1.5xADD
        if (retry++ > 100)
            break;
    }
    rt_thread_mdelay(20);
}
//硬复位MP3
//返回1:复位失败;0:复位成功
uint8_t VS_HD_Reset(void)
{
    uint8_t retry = 0;
    rt_pin_write(VS_RST, 0);
    rt_thread_mdelay(20);
    rt_pin_write(VS_XDCS, 1);   //取消数据传输
    rt_pin_write(VS_XCS, 1);   //取消数据传输
    rt_pin_write(VS_RST, 1);
    while (rt_pin_read(VS_DQ) == 0 && retry < 200)   //等待DREQ为高
    {
        retry++;
        rt_thread_mdelay(1);
    };
    rt_thread_mdelay(20);
    if (retry >= 200)
        return 1;
    else
        return 0;
}
//正弦测试
void VS_Sine_Test(void)
{
    VS_HD_Reset();
    VS_WR_Cmd(0x0b,0X2020);
    VS_WR_Cmd(SPI_MODE,0x0820);
    while(rt_pin_read(VS_DQ)==0);

    VS_SPI_SpeedLow();
    rt_pin_write(VS_XDCS, 0);
    VS_SPI_ReadWriteByte(0x53);
    VS_SPI_ReadWriteByte(0xef);
    VS_SPI_ReadWriteByte(0x6e);
    VS_SPI_ReadWriteByte(0x24);
    VS_SPI_ReadWriteByte(0x00);
    VS_SPI_ReadWriteByte(0x00);
    VS_SPI_ReadWriteByte(0x00);
    VS_SPI_ReadWriteByte(0x00);
    rt_thread_mdelay(100);
    rt_pin_write(VS_XDCS, 1);

    rt_pin_write(VS_XDCS, 0);
    VS_SPI_ReadWriteByte(0x45);
    VS_SPI_ReadWriteByte(0x78);
    VS_SPI_ReadWriteByte(0x69);
    VS_SPI_ReadWriteByte(0x74);
    VS_SPI_ReadWriteByte(0x00);
    VS_SPI_ReadWriteByte(0x00);
    VS_SPI_ReadWriteByte(0x00);
    VS_SPI_ReadWriteByte(0x00);
    rt_thread_mdelay(100);
    rt_pin_write(VS_XDCS, 1);

    rt_pin_write(VS_XDCS, 0);
    VS_SPI_ReadWriteByte(0x53);
    VS_SPI_ReadWriteByte(0xef);
    VS_SPI_ReadWriteByte(0x6e);
    VS_SPI_ReadWriteByte(0x44);
    VS_SPI_ReadWriteByte(0x00);
    VS_SPI_ReadWriteByte(0x00);
    VS_SPI_ReadWriteByte(0x00);
    VS_SPI_ReadWriteByte(0x00);
    rt_thread_mdelay(100);
    rt_pin_write(VS_XDCS, 1);

    rt_pin_write(VS_XDCS, 0);
    VS_SPI_ReadWriteByte(0x45);
    VS_SPI_ReadWriteByte(0x78);
    VS_SPI_ReadWriteByte(0x69);
    VS_SPI_ReadWriteByte(0x74);
    VS_SPI_ReadWriteByte(0x00);
    VS_SPI_ReadWriteByte(0x00);
    VS_SPI_ReadWriteByte(0x00);
    VS_SPI_ReadWriteByte(0x00);
    rt_thread_mdelay(100);
    rt_pin_write(VS_XDCS, 1);
}
//ram 测试
//返回值:RAM测试结果
// VS1003如果得到的值为0x807F，则表明完好;VS1053为0X83FF.
uint16_t VS_Ram_Test(void)
{
    VS_HD_Reset();
    VS_WR_Cmd(SPI_MODE, 0x0820);    // 进入VS10XX的测试模式
    while (rt_pin_read(VS_DQ) == 0); // 等待DREQ为高
    VS_SPI_SpeedLow(); //低速
    rt_pin_write(VS_XDCS, 0); // xDCS = 1，选择VS10XX的数据接口
    VS_SPI_ReadWriteByte(0x4d);
    VS_SPI_ReadWriteByte(0xea);
    VS_SPI_ReadWriteByte(0x6d);
    VS_SPI_ReadWriteByte(0x54);
    VS_SPI_ReadWriteByte(0x00);
    VS_SPI_ReadWriteByte(0x00);
    VS_SPI_ReadWriteByte(0x00);
    VS_SPI_ReadWriteByte(0x00);
    rt_thread_mdelay(150);
    rt_pin_write(VS_XDCS, 1);;
    return VS_RD_Reg(SPI_HDAT0);                  // VS1003如果得到的值为0x807F，则表明完好;VS1053为0X83FF.;
}
//向VS10XX写命令
//address:命令地址
//data:命令数据
void VS_WR_Cmd(uint8_t address, uint16_t data)
{
    while (rt_pin_read(VS_DQ) == 0);    //等待空闲
    VS_SPI_SpeedLow();                  //低速
    rt_pin_write(VS_XDCS, 1);
    rt_pin_write(VS_XCS, 0);
    VS_SPI_ReadWriteByte(VS_WRITE_COMMAND);                  //发送VS10XX的写命令
    VS_SPI_ReadWriteByte(address);  //地址
    VS_SPI_ReadWriteByte(data >> 8);  //发送高八位
    VS_SPI_ReadWriteByte(data);     //第八位
    rt_pin_write(VS_XCS, 1);
    VS_SPI_SpeedHigh();             //高速
}
//向VS10XX写数据
//data:要写入的数据
void VS_WR_Data(uint8_t data)
{
    VS_SPI_SpeedHigh();             //高速,对VS1003B,最大值不能超过36.864/4Mhz，这里设置为9M
    rt_pin_write(VS_XDCS, 0);
    VS_SPI_ReadWriteByte(data);
    rt_pin_write(VS_XDCS, 1);
}
//读VS10XX的寄存器
//address：寄存器地址
//返回值：读到的值
//注意不要用倍速读取,会出错
uint16_t VS_RD_Reg(uint8_t address)
{
    uint16_t temp = 0;
    while (rt_pin_read(VS_DQ) == 0); //非等待空闲状态
    VS_SPI_SpeedLow();             //低速
    rt_pin_write(VS_XDCS, 1);
    rt_pin_write(VS_XCS, 0);
    VS_SPI_ReadWriteByte(VS_READ_COMMAND);  //发送VS10XX的读命令
    VS_SPI_ReadWriteByte(address);          //地址
    temp = VS_SPI_ReadWriteByte(0xff);        //读取高字节
    temp = temp << 8;
    temp += VS_SPI_ReadWriteByte(0xff);       //读取低字节
    rt_pin_write(VS_XCS, 1);
    VS_SPI_SpeedHigh();       //高速
    return temp;
}
//读取VS10xx的RAM
//addr：RAM地址
//返回值：读到的值
uint16_t VS_WRAM_Read(uint16_t addr)
{
    uint16_t res;
    VS_WR_Cmd(SPI_WRAMADDR, addr);
    res = VS_RD_Reg(SPI_WRAM);
    return res;
}
//写VS10xx的RAM
//addr：RAM地址
//val:要写入的值
void VS_WRAM_Write(uint16_t addr, uint16_t val)
{
    VS_WR_Cmd(SPI_WRAMADDR, addr);   //写RAM地址
    while (rt_pin_read(VS_DQ) == 0);             //等待空闲
    VS_WR_Cmd(SPI_WRAM, val);        //写RAM值
}
//设置播放速度（仅VS1053有效）
//t:0,1,正常速度;2,2倍速度;3,3倍速度;4,4倍速;以此类推
void VS_Set_Speed(uint8_t t)
{
    VS_WRAM_Write(0X1E04, t);        //写入播放速度
}
//FOR WAV HEAD0 :0X7761 HEAD1:0X7665
//FOR MIDI HEAD0 :other info HEAD1:0X4D54
//FOR WMA HEAD0 :data speed HEAD1:0X574D
//FOR MP3 HEAD0 :data speed HEAD1:ID
//比特率预定值,阶层III
const uint16_t bitrate[2][16] = { { 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0 }, { 0, 32, 40, 48,
        56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0 } };
//返回Kbps的大小
//返回值：得到的码率
uint16_t VS_Get_HeadInfo(void)
{
    unsigned int HEAD0;
    unsigned int HEAD1;
    HEAD0 = VS_RD_Reg(SPI_HDAT0);
    HEAD1 = VS_RD_Reg(SPI_HDAT1);
    //printf("(H0,H1):%x,%x\n",HEAD0,HEAD1);
    switch (HEAD1)
    {
    case 0x7665:        //WAV格式
    case 0X4D54:        //MIDI格式
    case 0X4154:        //AAC_ADTS
    case 0X4144:        //AAC_ADIF
    case 0X4D34:        //AAC_MP4/M4A
    case 0X4F67:        //OGG
    case 0X574D:        //WMA格式
    case 0X664C:        //FLAC格式
    {
        ////printf("HEAD0:%d\n",HEAD0);
        HEAD1 = HEAD0 * 2 / 25;        //相当于*8/100
        if ((HEAD1 % 10) > 5)
            return HEAD1 / 10 + 1;        //对小数点第一位四舍五入
        else
            return HEAD1 / 10;
    }
    default:        //MP3格式,仅做了阶层III的表
    {
        HEAD1 >>= 3;
        HEAD1 = HEAD1 & 0x03;
        if (HEAD1 == 3)
            HEAD1 = 1;
        else
            HEAD1 = 0;
        return bitrate[HEAD1][HEAD0 >> 12];
    }
    }
}
//得到平均字节数
//返回值：平均字节数速度
uint32_t VS_Get_ByteRate(void)
{
    return VS_WRAM_Read(0X1E05);        //平均位速
}
//得到需要填充的数字
//返回值:需要填充的数字
uint16_t VS_Get_EndFillByte(void)
{
    return VS_WRAM_Read(0X1E06);        //填充字节
}
//发送一次音频数据
//固定为32字节
//返回值:0,发送成功
//       1,VS10xx不缺数据,本次数据未成功发送
uint8_t VS_Send_MusicData(uint8_t* buf)
{
    uint8_t n;
    if (rt_pin_read(VS_DQ) != 0)  //送数据给VS10XX
    {
        rt_pin_write(VS_XDCS, 0);
        for (n = 0; n < 32; n++)
        {
            VS_SPI_ReadWriteByte(buf[n]);
        }
        rt_pin_write(VS_XDCS, 1);
    }
    else
        return 1;
    return 0;  //成功发送了
}
//切歌
//通过此函数切歌，不会出现切换“噪声”
void VS_Restart_Play(void)
{
    uint16_t temp;
    uint16_t i;
    uint8_t n;
    uint8_t vsbuf[32];
    for (n = 0; n < 32; n++)
        vsbuf[n] = 0;  //清零
    temp = VS_RD_Reg(SPI_MODE);   //读取SPI_MODE的内容
    temp |= 1 << 3;                 //设置SM_CANCEL位
    temp |= 1 << 2;                 //设置SM_LAYER12位,允许播放MP1,MP2
    VS_WR_Cmd(SPI_MODE, temp);   //设置取消当前解码指令
    for (i = 0; i < 2048;)            //发送2048个0,期间读取SM_CANCEL位.如果为0,则表示已经取消了当前解码
    {
        if (VS_Send_MusicData(vsbuf) == 0)            //每发送32个字节后检测一次
        {
            i += 32;                      //发送了32个字节
            temp = VS_RD_Reg(SPI_MODE);   //读取SPI_MODE的内容
            if ((temp & (1 << 3)) == 0)
                break;  //成功取消了
        }
    }
    if (i < 2048)  //SM_CANCEL正常
    {
        temp = VS_Get_EndFillByte() & 0xff;  //读取填充字节
        for (n = 0; n < 32; n++)
            vsbuf[n] = temp;  //填充字节放入数组
        for (i = 0; i < 2052;)
        {
            if (VS_Send_MusicData(vsbuf) == 0)
                i += 32;  //填充
        }
    }
    else
        VS_Soft_Reset();      //SM_CANCEL不成功,坏情况,需要软复位
    temp = VS_RD_Reg(SPI_HDAT0);
    temp += VS_RD_Reg(SPI_HDAT1);
    if (temp)                    //软复位,还是没有成功取消,放杀手锏,硬复位
    {
        VS_HD_Reset();          //硬复位
        VS_Soft_Reset();        //软复位
    }
}
//重设解码时间
void VS_Reset_DecodeTime(void)
{
    VS_WR_Cmd(SPI_DECODE_TIME, 0x0000);
    VS_WR_Cmd(SPI_DECODE_TIME, 0x0000);        //操作两次
}
//得到mp3的播放时间n sec
//返回值：解码时长
uint16_t VS_Get_DecodeTime(void)
{
    uint16_t dt = 0;
    dt = VS_RD_Reg(SPI_DECODE_TIME);
    return dt;
}
//vs10xx装载patch.
//patch：patch首地址
//len：patch长度
void VS_Load_Patch(uint16_t *patch, uint16_t len)
{
    uint16_t i;
    uint16_t addr, n, val;
    for (i = 0; i < len;)
    {
        addr = patch[i++];
        n = patch[i++];
        if (n & 0x8000U) //RLE run, replicate n samples
        {
            n &= 0x7FFF;
            val = patch[i++];
            while (n--)
                VS_WR_Cmd(addr, val);
        }
        else //copy run, copy n sample
        {
            while (n--)
            {
                val = patch[i++];
                VS_WR_Cmd(addr, val);
            }
        }
    }
}
//设定VS10XX播放的音量和高低音
//volx:音量大小(0~254)
void VS_Set_Vol(uint8_t volx)
{
    uint16_t volt = 0;             //暂存音量值
    volt = 254 - volx;          //取反一下,得到最大值,表示最大的表示
    volt <<= 8;
    volt += 254 - volx;         //得到音量设置后大小
    VS_WR_Cmd(SPI_VOL, volt);         //设音量
}
//设定高低音控制
//bfreq:低频上限频率  2~15(单位:10Hz)
//bass:低频增益         0~15(单位:1dB)
//tfreq:高频下限频率  1~15(单位:Khz)
//treble:高频增益       0~15(单位:1.5dB,小于9的时候为负数)
void VS_Set_Bass(uint8_t bfreq, uint8_t bass, uint8_t tfreq, uint8_t treble)
{
    uint16_t bass_set = 0; //暂存音调寄存器值
    signed char temp = 0;
    if (treble == 0)
        temp = 0;            //变换
    else if (treble > 8)
        temp = treble - 8;
    else
        temp = treble - 9;
    bass_set = temp & 0X0F;             //高音设定
    bass_set <<= 4;
    bass_set += tfreq & 0xf;            //高音下限频率
    bass_set <<= 4;
    bass_set += bass & 0xf;             //低音设定
    bass_set <<= 4;
    bass_set += bfreq & 0xf;            //低音上限
    VS_WR_Cmd(SPI_BASS, bass_set);   //BASS
}
//设定音效
//eft:0,关闭;1,最小;2,中等;3,最大.
void VS_Set_Effect(uint8_t eft)
{
    uint16_t temp;
    temp = VS_RD_Reg(SPI_MODE);   //读取SPI_MODE的内容
    if (eft & 0X01)
        temp |= 1 << 4;     //设定LO
    else
        temp &= ~(1 << 5);         //取消LO
    if (eft & 0X02)
        temp |= 1 << 7;     //设定HO
    else
        temp &= ~(1 << 7);         //取消HO
    VS_WR_Cmd(SPI_MODE, temp);   //设定模式
}
//板载喇叭开/关设置函数.
//战舰V3板载了HT6872功放,通过VS1053的GPIO4(36脚),控制其工作/关闭.
//GPIO4=1,HT6872正常工作.
//GPIO4=0,HT6872关闭(默认)
//sw:0,关闭;1,开启.
void VS_SPK_Set(uint8_t sw)
{
    VS_WRAM_Write(GPIO_DDR, 1 << 4);   //VS1053的GPIO4设置成输出
    VS_WRAM_Write(GPIO_ODATA, sw << 4);   //控制VS1053的GPIO4输出值(0/1)
}
///////////////////////////////////////////////////////////////////////////////
//设置音量,音效等.
void VS_Set_All(void)
{
    VS_Set_Vol(vsset.mvol);         //设置音量
    VS_Set_Bass(vsset.bflimit, vsset.bass, vsset.tflimit, vsset.treble);
    VS_Set_Effect(vsset.effect);    //设置空间效果
    VS_SPK_Set(vsset.speakersw);    //控制板载喇叭状态
}
