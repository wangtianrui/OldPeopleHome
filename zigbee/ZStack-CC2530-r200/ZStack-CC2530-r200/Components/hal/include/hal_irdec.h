/***********************************************************
**  FileName:         IRDecode.h
**  Introductions:    IR Remote decode for MCU of CC2530
**  Last Modify time: 2013.05.20
**  Modify:           修改定时器4辅助判断解码中的超时失效
**  Author:           GuoZhenjiang <zhenjiang.guo@sunplusapp.com>
***********************************************************/
/***********************解码原理****************************
*   1、定时器1的通道0(Alt.2 = P1.2)是红外信号接收输入引脚
*   2、采用定时器1通道0上升沿输入捕获模式获取红外信号
*   3、定时器1计数频率1MHz
*   4、每当上升沿捕获后获取当前计数值(周期)，然后清零计数值并重新开始计数
*   5、获取的每一小周期按照时间阀值判断是引导码、数据0、数据1
*   5、解码过程中利用定时器4辅助判断是否超时
***********************************************************/

#ifndef __IRDECODE_H__
#define __IRDECODE_H__

#include "OSAL.h"

#define IRDEC_INT_EVENT     0xF1    //IO端口中断采用0xF0

typedef struct {
    uint8 irLen;
    uint8 irCode[15];
} OSALIRDecData_t;

typedef struct
{
    osal_event_hdr_t hdr;  //事件类型及状态
    uint8 endPoint;
    OSALIRDecData_t *data;
} OSALIRDecIntData_t;


#define CLR_T1CNT       T1CNTL = 0  // 向T1CNTL寄存器中写任何值会导致T1CNT清零。

// T1CTL
// 定时器1时钟预分频
typedef enum
{
    Tdiv_1      = 0X00, //标记频率/1
    Tdiv_8      = 0X04, //标记频率/8
    Tdiv_32     = 0X08, //标记频率/32
    Tdiv_128    = 0X0C, //标记频率/128
}Timer1Div_t;

// 定时器1运行模式
typedef enum
{
    Tmod_stop   = 0X00, //暂停运行
    Tmod_free   = 0X01, //自由运行
    Tmod_Mod    = 0X02, //模模式
    Tmod_PN     = 0X03, //正/倒计数模式
}Timer1Mod_t;

// T1CCTLx(x = 0,1,2,3,4,5)
// 定时器1通道配置
typedef enum
{
    TCCSet      = 0X00, //比较匹配时置位
    TCCClear    = 0X08, //比较匹配时清零
    TCCExch     = 0X02, //模模式
    TCCSet0Clr  = 0X03, //正/倒计数模式
}Timer1CC_t;

// 红外遥控编码解调后周期 in us
// 4.500ms引导码判断阀值
#define T_IR_GUIDE4_MIN       8000      //4.5ms引导码 理论时间9.000ms
#define T_IR_GUIDE4_MAX       9500
// 9.000ms引导码判断阀值
#define T_IR_GUIDE9_MIN      12000      //9ms引导码 理论时间13.500ms
#define T_IR_GUIDE9_MAX      14000
// 数据位0判断阀值
#define T_IR_0_MIN            1000      //位0 理论时间1.125ms
#define T_IR_0_MAX            1500
// 数据位1判断阀值
#define T_IR_1_MIN            2000      //位1 理论时间2.250ms
#define T_IR_1_MAX            2300
// 用于判断一次红外信号结束的周期阀值
#define T_IR_ENDED            50      // 比位1的周期稍微长点,不在引导码范围内

typedef enum
{
    WaitForGuideTrig1,      // 等待引导码第一个上升沿
    WaitForGuideTrig2,      // 等待引导码第二个上升沿
    Decoding,               // 位0位1解码过程中
    Decode_GuideOverTime,   // 引导码结束等待超时
    Decode_bitOverTime,     // 位0 位1 结束等待超时
    Decode_BufOverflow,     // 接收队列溢出导致解码结束
    Decode_OverUnknow,      // 未知原因导致解码结束
}IRDecodeSta_t;

// 中断处理法红外遥控解码初始化
void IRDecodeT1Init(unsigned char taskid, unsigned char ep);

#endif  //__IRDECODE_H__