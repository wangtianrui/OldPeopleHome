/***********************************************************
**  FileName:         IR.c
**  Introductions:    Sunplusapp CC2530 ZigBee Node IR Romoter Signal Send Functions
**  Last Modify time: 2013.05.21
**  Modify:           修改红外编码发送，优化时序，添加注释
**  Author:           GuoZhenjiang <zhenjiang.guo@sunplusapp.com>
***********************************************************/

#include "IR.h"
#include "OSAL.h"

#if defined(HAL_IRENC) && (HAL_IRENC == TRUE)

// 定时器1中自增量,控制每一位编码的周期
volatile uint16  T1_80usCnt = 0;

// Timer1红外编码初始化
void IRSend_T1_Init(void);
// Timer3红外编码初始化
void IRSend_T3_Init(void);
// Timer1 & Timer3 中断处理函数声明
__near_func __interrupt void IRSend_T1IntHandle(void);
__near_func __interrupt void IRSend_T3IntHandle(void);

/***********************************************************
**  函数名称: GenIR
**  实现功能: 系统时钟设置
**  入口参数: IRData:   红外编码数据首地址
**            Mode:     引导码分类:1 周期9ms    2:周期13.56ms
**            CodeLen:  编码位数(bit,不是Byte)
**  返回结果: 0:发送失败    1:发送成功
***********************************************************/
int GenIR(uint8 *IRData , uint8 Mode , uint8 CodeLen )
{
    uint16  GuideCodeLen=0;
    static IRSendSta_t IRSendSta = IRSta_Guide;
    static uint8 cntByte=0, cntbit=0, bitNum;
    bitNum = CodeLen;       //保存位数

    if(Mode == 1)
      GuideCodeLen = 56;//226;   引导码长度（4.5ms、4.5ms）
    else if(Mode == 2)
      GuideCodeLen = 114;   //引导码长度（9ms、4.5ms）

    IRSend_T1_Init();
    IRSend_T3_Init();
    IR_SEL_IOOUT;
    CLR_IOB_BIT;
    IRSendSta = IRSta_Guide;
    T1_80usCnt = 0;
    IR_SEL_T3PWM;
    while(1)
    {
        // 查询红外编码发送状态
        switch(IRSendSta)
        {
            // 引导码阶段
        case IRSta_Guide:	
            // 引导码4.5ms或9ms载波阶段
            if(T1_80usCnt <= GuideCodeLen)
            {
                IRSendSta = IRSta_Guide;
                IR_SEL_T3PWM;
            }
            // 引导码4.5ms低电平阶段
            else if((T1_80usCnt > GuideCodeLen) && (T1_80usCnt <= GuideCodeLen+56))
            {
                IRSendSta = IRSta_Guide;
                IR_SEL_IOOUT;
                CLR_IOB_BIT;
            }
            // 引导码发送完毕,准备发送数据位
            else
            {
                IRSendSta = IRSta_T3PWM;
                IR_SEL_T3PWM;
                T1_80usCnt = 0;
            }
            break;
            // 数据位中的位0、位1的0.56ms的38kHz载波阶段
        case IRSta_T3PWM:
            if(T1_80usCnt >= 7)     // 7 * 80us = 560us
            {
                IR_SEL_IOOUT;
                CLR_IOB_BIT;
                IRSendSta = IRSta_Data0;
            }
            break;
            // 数据位中的位0、位1的低电平持续阶段
        case IRSta_Data0:
            //发送数据0,低电平持续 0.565ms。
            if(!(IRData[cntByte] & (0x80 >> (cntbit % 8))))
            {
                if(T1_80usCnt >= 14)
                {
                    IR_SEL_T3PWM;
                    T1_80usCnt = 0;
                    cntbit++;
                    if(0 == (cntbit % 8))
                        cntByte++;
                }
            }
            //发送数据1,低电平持续 1.685ms。
            else if(IRData[cntByte] & (0x80 >> (cntbit % 8)))
            {
                if(T1_80usCnt >= 28)
                {				
                    IR_SEL_T3PWM;
                    T1_80usCnt = 0;
                    cntbit++;
                    if(0 == (cntbit % 8))
                        cntByte++;
                }
            }	
            // 发送完毕?
            if(cntbit >= bitNum)	
                IRSendSta = IRSta_Stop;
            // 继续发送?
            else
                IRSendSta = IRSta_T3PWM;
            break;
            // 红外编码数据部分发送完毕
        case IRSta_Stop:
            // 一组编码发送完毕  结束位是 0.56ms 的 38K 载波
            while(T1_80usCnt < 7)
                ;
            STOP_T1_T3; // stop T1 & T3
            T1_80usCnt = 0;
            IR_SEL_IOOUT;
            CLR_IOB_BIT;
            cntbit = 0;
            cntByte = 0;
            return 1;
            break;
        default:
            return 0;
            break;
        }							
    }
}

/***********************************************************
**  函数名称: IRSend_T1_Init
**  实现功能: 红外编码发送定时器1初始化
**  入口参数: None
**  返回结果: None
***********************************************************/
void IRSend_T1_Init(void)
{
    T1CTL = 0x0E;       // f = 32MHz / 128 = 250 000Hz, T = 4us;模模式,从0~T1CC0反复计数。
    T1CCTL0 = 0x44;     // Timer1通道0中断允许,比较匹配模式,比较比配时输出置位。
    T1CC0L = 19;        // 先写T1CC0L,再写T1CC0H,T1CNT=0时更新。 4us*20 = 80us;
    T1CC0H = 0;
    TIMIF |= BIT_1(6);  // TIMIF.T1OVFIM Timer1溢出中断允许
    IEN1 |= BIT_1(1);   // IEN1.T1IE Timer1总中断允许
    EA = 1;             // 打开全局总中断
    T1_80usCnt = 0;     // 80us溢出中断自增量清零
}

/***********************************************************
**  函数名称: IRSend_T3_Init
**  实现功能: 红外编码发送定时器3初始化
**  入口参数: None
**  返回结果: None
***********************************************************/
void IRSend_T3_Init(void)
{
    P1SEL |= BIT_1(3);      // P1.3 us as T3 PWM Out Pin.
    T3CTL = 0x02;           // T3CTL.MODE[10] 模模式,从0~T3CC0反复计数
    T3CTL |= BIT_1(5);      // T3CTL.MODE[001] f = 32MHz / 2 = 16MHz;
    T3CCTL0 |= BIT_1(2);    // T3CCTL0.MODE 输出比较比配模式
    T3CCTL0 |= BIT_1(4);    // T3CCTL0.CMP[010] 比较匹配时,输出取反

    T3CC0 = 208;            // Timer3通道0输出比较匹配上限值,16MHz / 208 = 76923.076923Hz; 76923 / 2 =38k(比较匹配时取反).
    T3CTL |= BIT_1(4);      // T3CTL.START 启动Timer3
    P2SEL |= BIT_1(5);      // P2SEL.PRI2P1 当USART1和Timer3占用相同引脚,Timer3优先。
    T3CCTL0 |= BIT_1(6);    // T3CCTL0.IM Timer3通道0中断允许。
    IEN1 |= BIT_1(3);       // IEN1.T3IE Timer3总中断允许。
    EA = 1;                 // 全局总中断允许。
}

/***********************************************************
**  函数名称: IRSendInit
**  实现功能: 红外编码发送初始化
**  入口参数: None
**  返回结果: None
***********************************************************/
void IRSendInit(void)
{
    IR_SEL_IOOUT;
    CLR_IOB_BIT;
}

/***********************************************************
**  函数名称: IRSend_T1IntHandle
**  实现功能: Timer1 红外编码发送中的中断处理函数
**  入口参数: None
**  返回结果: None
***********************************************************/
#pragma vector = T1_VECTOR
__interrupt void IRSend_T1IntHandle(void)
{
    T1STAT &= BIT_0(5);     // T1STAT.OVFIF 清除Timer1溢出中断标志
    T1STAT &= BIT_0(1);     // T1STAT.CH0IF 清除Timer1通道0中断标志
    IRCON &= BIT_0(1);      // IRCON.T1IF   清除Timer1总中断标志
    T1_80usCnt++;
}

/***********************************************************
**  函数名称: IRSend_T3IntHandle
**  实现功能: Timer3 红外编码发送中的中断处理函数
**  入口参数: None
**  返回结果: None
***********************************************************/
#pragma vector = T3_VECTOR
__near_func __interrupt void IRSend_T3IntHandle(void)
{
    TIMIF &= BIT_0(1);      // TIMIF.T3CH0IF
}


#endif  // defined(HAL_IRENC) && (HAL_IRENC == TRUE)
