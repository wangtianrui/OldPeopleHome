/***********************************************************
**  FileName:         IRDecode.c
**  Introductions:    IR Remote decode for MCU of CC2530
**  Last Modify time: 2013.05.03
**  Update:           基本解码测试
**  Author:           GuoZhenjiang<zhenjiang.guo@sunplusapp.com>
**  Modify:
**             2013.05.03   lijian  Change File Name to hal_irdec.c
***********************************************************/
#include <hal_board.h>
#include "hal_irdec.h"
#include <string.h>

#if defined(HAL_IRDEC) && (HAL_IRDEC == TRUE)
static struct {
    uint8 IRIntTskId;
    uint8 IRIntEp;
    OSALIRDecData_t IRCodeTab;
} IRDecodeRes;

volatile uint8 IROverTimeAdd_En=FALSE, IROverTimeOvf_flg=FALSE;
volatile uint16 IROverTimeAdd_cnt=0;
volatile IRDecodeSta_t IRDeodeSta = WaitForGuideTrig1;
static uint8 cnt_Byte, cnt_bit;     // 用于控制解码过程的基本变量

/***********************************************************
**  函数名称: IRDecodeT1Init
**  实现功能: 中断法进行红外解码初始化工作
**  入口参数: None
**  返回结果: None
***********************************************************/
void IRDecodeT1Init(uint8 taskid, uint8 ep)
{
    // 保存任务号和端点号
    IRDecodeRes.IRIntTskId = taskid;
    IRDecodeRes.IRIntEp = ep;

    // 初始化Timer1
    P1SEL |= 0x04;      // 配置P1.2为Timer1的外设功能引脚
    P1DIR &= ~0x04;     // 输入捕获引脚必须设置为输入
    PERCFG |= 0x40;     // Timer1占用第2引脚位置(通道0为P1.2)
    P2SEL |= 0x04;      // 当USART0和Timer1分配了相同引脚，Timer1优先

    // 定时器1初始化
    T1CTL = (uint8)Tdiv_32 | (uint8)Tmod_stop;    // 1MHz,暂停运行
    CLR_T1CNT;                      // 清零定时器1

    // Timer1通道0上升沿输入捕获设置
    T1CCTL0 = 0x01;                 // 上升沿捕获,通道0输入捕获中断未使能,查询中断标志即可
    T1CCTL0 |= 0x40;                // T1CCTL0.IM 定时器1通道0中断开关
    T1STAT = 0;
    IEN1 |= 0x02;                   // IEN1.T1IE 定时器1中断总开关
    EA = 1;                         // IEN0.EA 全局总中断开关

    // 初始化Time4
    P2SEL |= 0x10;  // 定时器4优先级高于定时器1
    T4CTL |= 0xEC;  // 128分频,清空计数值,允许溢出中断,自由运行
    T4CTL |= 0x10;  // 启动定时器
    IEN1 |= 0x10;   // 打开定时器4总中断
}

/***********************************************************
**  函数名称: Timer1_ISR
**  实现功能: Timer1 中断处理函数,利用Timer1通道0输入捕获功能实现红外解码
**  入口参数: None
**  返回结果: None
***********************************************************/
HAL_ISR_FUNC_DECLARATION(Timer1_ISR, T1_VECTOR);
HAL_ISR_FUNCTION(Timer1_ISR, T1_VECTOR)
{
#define CLR_T1CH0_IF    T1STAT &= 0xFE
#define CLR_T1CH1_IF    T1STAT &= 0xFD
#define CLR_T1OVF_IF    T1STAT &= 0xDF
#define CLR_T1IF        IRCON &= 0xFD

    static uint8 IRdata[15];            // 用于存储解析的红外数据
    static uint16 tCapture = 0;         // 用于存储判断每个捕获周期

    // 红外解码中捕获到定时器1通道0上升沿
    if(T1STAT & 0x01)
    {
        switch(IRDeodeSta)
        {
        case WaitForGuideTrig1:
            IRDeodeSta = WaitForGuideTrig2;
            CLR_T1CNT;                      // 清零定时器1
            T1CTL = (uint8)Tdiv_32 | (uint8)Tmod_free;    // 1MHz,自由计数模式
            break;
        case WaitForGuideTrig2:
            tCapture = T1CC0L;                          // 获取当前一个周期值
            tCapture |= (uint16)T1CC0H << 8;
            CLR_T1CNT;                                  // 清零定时器1
            T1CTL = (uint8)Tdiv_32 | (uint8)Tmod_free;  // 1MHz,自由计数模式
            // 如果是引导码4.5ms引导码或者9ms引导码
            if(((tCapture > T_IR_GUIDE9_MIN) && (tCapture < T_IR_GUIDE9_MAX)) || ((tCapture > T_IR_GUIDE4_MIN) && (tCapture < T_IR_GUIDE4_MAX)))
            {
                IRDeodeSta = Decoding;
                // 准备解码工作
                cnt_bit = 0;
                cnt_Byte = 0;
            }
            else
                IRDeodeSta = WaitForGuideTrig1;
            IROverTimeAdd_cnt = 0;
            break;
        case Decoding:
            tCapture = T1CC0L;                          // 获取当前一个周期值
            tCapture |= (uint16)T1CC0H << 8;
            CLR_T1CNT;                                  // 清零定时器1
            T1CTL = (uint8)Tdiv_32 | (uint8)Tmod_free;  // 1MHz,自由计数模式
            // 接收到位0?
            if((tCapture > T_IR_0_MIN) && (tCapture < T_IR_0_MAX))
            {
                IRDeodeSta = Decoding;
                IROverTimeAdd_cnt = 0;
                IRdata[cnt_Byte] &= ~(0x80 >> cnt_bit);    // 保存当前位0
                cnt_bit++;
                if(cnt_bit >= 8)    // 接收完毕一个字节
                {
                    IRDecodeRes.IRCodeTab.irCode[cnt_Byte] = IRdata[cnt_Byte];
                    cnt_bit=0;
                    cnt_Byte++;
                    if(cnt_Byte >= 15)  // 超出最大接收队列长度
                    {
                        IRDeodeSta = Decode_BufOverflow;
                        goto OneIRpkgGot;
                    }
                }
            }
            // 接收到位1?
            else if((tCapture > T_IR_1_MIN) && (tCapture < T_IR_1_MAX))
            {
                IRDeodeSta = Decoding;
                IROverTimeAdd_cnt = 0;
                IRdata[cnt_Byte] |= 0x80 >> cnt_bit;    // 保存当前位0
                cnt_bit++;
                if(cnt_bit >= 8)    // 接收完毕一个字节
                {
                    IRDecodeRes.IRCodeTab.irCode[cnt_Byte] = IRdata[cnt_Byte];
                    cnt_bit=0;
                    cnt_Byte++;
                    if(cnt_Byte >= 15)  // 超出最大接收队列长度
                    {
                        IRDeodeSta = Decode_BufOverflow;
                        goto OneIRpkgGot;
                    }
                }
            }
            // 没有超时,不是位0,不是位1,出错???
            else
            {
                IRDeodeSta = Decode_OverUnknow;
                goto OneIRpkgGot;
            }
            break;
        default :
            IRDeodeSta = WaitForGuideTrig1;
            break;
        }
    OneIRpkgGot:
        {
            switch(IRDeodeSta)     // 一次红外解码结束
            {
                // 处理 接收队列溢出导致解码结束
            case Decode_BufOverflow:
                // 处理 未知原因导致解码结束
            case Decode_OverUnknow:
                IROverTimeAdd_cnt = 0;
                IRDecodeRes.IRCodeTab.irLen = cnt_Byte;
                cnt_bit = 0;
                cnt_Byte = 0;
                OSALIRDecIntData_t* T1Ch0IntData;
                T1Ch0IntData = (OSALIRDecIntData_t *)osal_msg_allocate(sizeof(OSALIRDecIntData_t));
                T1Ch0IntData->hdr.event = IRDEC_INT_EVENT;
                T1Ch0IntData->endPoint = IRDecodeRes.IRIntEp;
                T1Ch0IntData->data = &IRDecodeRes.IRCodeTab;
                osal_msg_send(IRDecodeRes.IRIntTskId, (uint8*)(T1Ch0IntData));
                IRDeodeSta = WaitForGuideTrig1;
                break;
            default :
                break;
            }
        }
        CLR_T1CH0_IF;         // 清除通道0输入捕获中断标志
    }
    CLR_T1IF;         // 清除T1IF
 }

/***********************************************************
**  函数名称: Timer4_ISR
**  实现功能: Timer4 中断处理函数,利用Timer1通道0输入捕获功能实现红外解码
**  入口参数: None
**  返回结果: None
***********************************************************/
HAL_ISR_FUNC_DECLARATION(Timer4_ISR, T4_VECTOR);
HAL_ISR_FUNCTION(Timer4_ISR, T4_VECTOR)
{
    // (32MHz / 128) T = 4us; f = 250000Hz;
    switch(IRDeodeSta)
    {
        // 等待第二次触发,检测引导码是否超时
    case WaitForGuideTrig2:
        IROverTimeAdd_cnt++;
        if(IROverTimeAdd_cnt >= 16)   // 约16ms内没有清零
        {
            IROverTimeAdd_cnt = 0;
            IRDeodeSta = WaitForGuideTrig1;
        }
        break;
        // 位0 位1 解码中,检测是否解码超时
    case Decoding:
        IROverTimeAdd_cnt++;
        if(IROverTimeAdd_cnt >= 5)   // 4ms内没有清零
        {
            IROverTimeAdd_cnt = 0;
            IRDecodeRes.IRCodeTab.irLen = cnt_Byte;
            cnt_bit = 0;
            cnt_Byte = 0;
            OSALIRDecIntData_t* T4OvfIntData;
            T4OvfIntData = (OSALIRDecIntData_t *)osal_msg_allocate(sizeof(OSALIRDecIntData_t));
            T4OvfIntData->hdr.event = IRDEC_INT_EVENT;
            T4OvfIntData->endPoint = IRDecodeRes.IRIntEp;
            T4OvfIntData->data = &IRDecodeRes.IRCodeTab;
            osal_msg_send(IRDecodeRes.IRIntTskId, (uint8*)(T4OvfIntData));
            IRDeodeSta = WaitForGuideTrig1;
        }
        break;
    default :
        IROverTimeAdd_cnt = 0;
        break;
    }
    T4OVFIF = 0;    // 清除定时器4溢出中断标志
    T4IF = 0;       // 清除定时器4总中断标志
}
#endif  //#if defined(HAL_IRDEC) && (HAL_IRDEC == TRUE)
