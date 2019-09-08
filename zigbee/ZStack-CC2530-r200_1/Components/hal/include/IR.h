/***********************************************************
**  FileName:         IR.h
**  Introductions:    Sunplusapp CC2530 ZigBee Node IR Romoter Signal Send Functions
**  Last Modify time: 2013.05.21
**  Modify:           去掉了头文件中没用的宏定义,添加注释
**  Author:           GuoZhenjiang <zhenjiang.guo@sunplusapp.com>
***********************************************************/

#ifndef __IR_H__
#define __IR_H__
#include <hal_board.h>

// 红外发送引脚[P1.3]端口控制相关寄存器
#define PSEL_IR_SEND    P1SEL
#define PDIR_IR_SEND    P1DIR
#define POUT_IR_SEND    P1
// 红外发送引脚位序号
#define BIT_IR_SEND     3

// 红外编码发送状态枚举
typedef enum
{
    IRSta_Guide,    // 引导码产生中
    IRSta_T3PWM,    // Timer3产生38kHz调制波
    IRSta_Data0,    // IR引脚输出低电平
    IRSta_Stop,     // 发送停止位
}IRSendSta_t;

enum
{
    IRGuideLen_9ms  = 1,    // 红外引导码长度9.0ms
    IRGuideLen_13ms = 2,    // 红外引导码长度13.5ms
};

// 位序列操作宏定义
#define BIT_1(x)   ( BV(x))
#define BIT_0(x)   (~BV(x))

#define SET_IOB_BIT     POUT_IR_SEND |= BIT_1(BIT_IR_SEND)
#define CLR_IOB_BIT     POUT_IR_SEND &= BIT_0(BIT_IR_SEND)

#define IR_SEL_IOOUT    PSEL_IR_SEND &= BIT_0(BIT_IR_SEND); PDIR_IR_SEND |= BIT_1(BIT_IR_SEND);
#define IR_SEL_T3PWM    PSEL_IR_SEND |= BIT_1(BIT_IR_SEND)
#define STOP_T1_T3      T3CTL = 0x00;   T1CTL = 0x00

// 红外编码发送初始化
void IRSendInit(void);
// 红外编码发送函数
int GenIR(uint8 *IRData , uint8 Mode , uint8 CodeLen );

#endif  // __IR_H__
