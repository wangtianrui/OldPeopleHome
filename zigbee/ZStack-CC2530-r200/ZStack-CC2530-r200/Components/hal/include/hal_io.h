#ifndef __HAL_IO_H__
#define __HAL_IO_H__

#include <ioCC2530.h>
#include "OSAL.h"

#define IOPORT_INT_EVENT            0xF0    //IO口中断事件

#define HAL_IOPORT(group, bit)  (((group) << 3) | (bit))
#define MAX_IOGROUP     2
#define MAX_IOPORT      HAL_IOPORT(MAX_IOGROUP, 4)

typedef struct
{
    osal_event_hdr_t hdr;  //事件类型及状态
    uint8 endPoint;
    void *arg;
} OSALIOIntData_t;

//端口作为输入时内部上下拉选择
typedef enum
{
    Pull_None,  //三态输入
    Pull_Up,    //上拉使能
    Pull_Down,  //下拉使能
}PullSet_t;

/***********************************************************
**  CC2530每个通用IO引脚都可以产生中断！
**  复位后所有中断关闭，根据需要打开即可
***********************************************************/
typedef enum          //中断触发方式选择
{
    IOInt_None,       //关闭相应中断
    IOInt_Rising,     //上升沿触发
    IOInt_Falling,    //下降沿触发
}IntSel_t;

void HalIOInit(uint8 taskId);
//设置端口输入模式
void HalIOSetInput(uint8 group, uint8 bit, PullSet_t pull);
void HalIOSetOutput(uint8 group, uint8 bit);
uint8 HalIOGetLevel(uint8 group, uint8 bit);
void HalIOSetLevel(uint8 group, uint8 bit, uint8 value);

//设置端口中断触发方式
void HalIOIntSet(uint8 endPoint, uint8 group, uint8 bit, IntSel_t trigger, void *arg);
//轮询IO端口中断标志
void HalIOPortPoll(void);

#endif  //__HAL_IO_H__
