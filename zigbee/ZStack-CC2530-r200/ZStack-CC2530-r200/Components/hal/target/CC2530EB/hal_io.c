#include "hal_io.h"
#include "OSAL.h"
#include <string.h>

static struct {
    uint8 ioIntTskId;
    uint8 intInUse[MAX_IOGROUP + 1];
    uint8 endPointMap[MAX_IOPORT + 1];
    void *endPointArgMap[MAX_IOPORT + 1];
} ioIntResMap;
void HalIOInit(uint8 taskId)
{
    memset(&ioIntResMap, 0, sizeof(ioIntResMap));
    ioIntResMap.ioIntTskId = taskId;
}
/***********************************************************
**  函数名称: HalIOSetInput
**  实现功能: 设置端口为普通输入IO
**  入口参数: group:Port;
**            bit:Bit;
**            pull:(Pull_None:无上下拉; Pull_Up:上拉; Pull_Down:下拉;);
**  返回结果: IOInt_None
**  注意事项: CC2530的通用IO上下拉电阻是对整个端口的设置，
**            不能实现将同一端口的不同位配置为上下拉不同
***********************************************************/
void HalIOSetInput(uint8 group, uint8 bit, PullSet_t pull)
{
    switch(group)
    {
    case 0:
        //设置为通用输入IO
        CLRBIT(P0DIR, bit);
        CLRBIT(P0SEL, bit);
        //设置内部上下拉电阻状态
        if(Pull_None == pull)
            SETBIT(P0INP, bit);     //P0INP[7-0]:(0:上下拉有效; 1:无效;)
        else if(Pull_Up == pull)
        {
            CLRBIT(P0INP, bit);     //P0INP[7-0]:(0:上下拉有效; 1:无效;)
            CLRBIT(P2INP, 5);       //P2INP[5  ]:(0:Port0 上拉; 1:Port0 下拉;)
        }
        else if(Pull_Down == pull)
        {
            CLRBIT(P0INP, bit);     //P0INP[7-0]:(0:上下拉有效; 1:无效;)
            SETBIT(P2INP, 5);       //P2INP[5  ]:(0:Port0 上拉; 1:Port0 下拉;)
        }
        break;
    case 1:
        //设置为通用输入IO
        CLRBIT(P1DIR, bit);
        CLRBIT(P1SEL, bit);
        //设置内部上下拉电阻状态
        if(Pull_None == pull)
            SETBIT(P1INP, bit);     //P1INP[7-2]:(0:上下拉有效; 1:无效;) P[1-0] 写无效，读为0.
        else if(Pull_Up == pull)
        {
            CLRBIT(P1INP, bit);     //P1INP[7-2]:(0:上下拉有效; 1:无效;) P[1-0] 写无效，读为0.
            CLRBIT(P2INP, 6);       //P2INP[6  ]:(0:Port1 上拉; 1:Port1 下拉;)
        }
        else if(Pull_Down == pull)
        {
            CLRBIT(P1INP, bit);     //P1INP[7-2]:(0:上下拉有效; 1:无效;) P[1-0] 写无效，读为0.
            SETBIT(P2INP, 6);       //P2INP[6  ]:(0:Port1 上拉; 1:Port1 下拉;)
        }
        break;
    case 2:
        //设置为通用输入IO
        CLRBIT(P2DIR, bit);
        CLRBIT(P2SEL, bit);
        //设置内部上下拉电阻状态
        if(Pull_None == pull)
            SETBIT(P2INP, bit);     //P2INP[4-0]:(0:上下拉有效; 1:无效;)
        else if(Pull_Up == pull)
        {
            CLRBIT(P2INP, bit);     //P2INP[4-0]:(0:上下拉有效; 1:无效;)
            CLRBIT(P2INP, 7);       //P2INP[7  ]:(0:Port2 上拉; 1:Port2 下拉;)
        }
        else if(Pull_Down == pull)
        {
            CLRBIT(P2INP, bit);     //P2INP[4-0]:(0:上下拉有效; 1:无效;)
            SETBIT(P2INP, 7);       //P2INP[7  ]:(0:Port2 上拉; 1:Port2 下拉;)
        }
        break;
    default:
        break;
    }
}

void HalIOSetOutput(uint8 group, uint8 bit)
{
   switch(group)
   {
   case 0: P0DIR |= (1 << bit); P0SEL &= ~(1 << bit); break;
   case 1: P1DIR |= (1 << bit); P1SEL &= ~(1 << bit); break;
   case 2: P2DIR |= (1 << bit); P2SEL &= ~(1 << bit); break;
   }
}
uint8 HalIOGetLevel(uint8 group, uint8 bit)
{
    switch(group)
    {
    case 0: return !!(P0 & (1 << bit));
    case 1: return !!(P1 & (1 << bit));
    case 2: return !!(P2 & (1 << bit));
    }
    return 0;
}
void HalIOSetLevel(uint8 group, uint8 bit, uint8 value)
{
    switch(group)
    {
    case 0:
        if(value)
            SETBIT(P0, bit);
        else
            CLRBIT(P0, bit);
        break;
    case 1:
        if(value)
            SETBIT(P1, bit);
        else
            CLRBIT(P1, bit);
        break;
    case 2:
        if(value)
            SETBIT(P2, bit);
        else
            CLRBIT(P2, bit);
        break;
    }
}
/***********************************************************
**  函数名称: IOIntteruptSet
**  实现功能: 设置端口中断触发方式
**  入口参数: group:Port;
**            bit:Bit;
**            trigger:(IOInt_Rising:上升沿触发; IOInt_Falling:下降沿触发;)
**  返回结果: IOInt_None
**  注意事项: CC2530的通用IO中断触发方式是对整个端口的设置，
**            只有P1口的高四位和第四位触发方式可以设置为不同
**            P0端口和P2端口的所有端口触发方式以最后一次设置为准。
***********************************************************/
void HalIOIntSet(uint8 endPoint, uint8 group, uint8 bit, IntSel_t trigger, void *arg)
{
    if(HAL_IOPORT(group, bit) > MAX_IOPORT)
        return;
    if(trigger == IOInt_None)
    {
        CLRBIT(ioIntResMap.intInUse[group], bit);
    }
    else
    {
        SETBIT(ioIntResMap.intInUse[group], bit);
        ioIntResMap.endPointMap[HAL_IOPORT(group, bit)] = endPoint;
        ioIntResMap.endPointArgMap[HAL_IOPORT(group, bit)] = arg;
    }
    switch(group)
    {
    case 0:
        if(trigger == IOInt_None)
            CLRBIT(P0IEN, bit);
        else
        {
            SETBIT(P0IEN, bit);
            if(trigger == IOInt_Rising)
                CLRBIT(PICTL, 0);
            else
                SETBIT(PICTL, 0);
        }
        P0IFG = 0x00;       //清除P0相应位中断标志
        P0IF = 0;           //清除P0端口总中断标志
        //SETBIT(IEN1, 5);  //P0总中断允许
        CLRBIT(IEN1, 5);    //P0总中断禁止
        break;
    case 1:
        if(trigger == IOInt_None)
            CLRBIT(P1IEN, bit);
        else
        {
            uint8 ctlBit = (bit <= 3) ? 1 : 2;
            SETBIT(P1IEN, bit);
            if(trigger == IOInt_Rising)
                CLRBIT(PICTL, ctlBit);
            else
                SETBIT(PICTL, ctlBit);
        }
        P1IFG = 0X00;     //清除P0相应位中断标志
        P1IF = 0;         //清除P0端口总中断标志
        //SETBIT(IEN2, 4);  //P1总中断允许
        CLRBIT(IEN2, 4);    //P1总中断禁止
        break;
    case 2:
        if(trigger == IOInt_None)
            CLRBIT(P2IEN, bit);
        else
        {
            SETBIT(P2IEN, bit);
            if(trigger == IOInt_Rising)
                CLRBIT(PICTL, 3);
            else
                SETBIT(PICTL, 3);
        }
        P2IFG = 0X00;     //清除P0相应位中断标志
        P2IF = 0;         //清除P0端口总中断标志
        //SETBIT(IEN2, 1);  //P2总中断允许
        CLRBIT(IEN2, 1);    //P2总中断禁止
        break;
    default :
        break;
    }
}

void HalIOPortPoll()
{
    OSALIOIntData_t* IOIntData;

    uint8 idx;
    uint8 flag = ioIntResMap.intInUse[0] & P0IFG;
    for(idx = 0; flag && (idx < 8); idx++)
    {
        if(BV(idx) & flag)
        {
            IOIntData = (OSALIOIntData_t *)osal_msg_allocate(sizeof(OSALIOIntData_t));
            IOIntData->hdr.event = IOPORT_INT_EVENT;
            IOIntData->endPoint = ioIntResMap.endPointMap[HAL_IOPORT(0, idx)];
            IOIntData->arg = ioIntResMap.endPointArgMap[HAL_IOPORT(0, idx)];
            osal_msg_send(ioIntResMap.ioIntTskId, (uint8*)(IOIntData));
        }
    }
    flag = ioIntResMap.intInUse[1] & P1IFG;
    for(idx = 0; flag && (idx < 8); idx++)
    {
        if(BV(idx) & flag)
        {
            IOIntData = (OSALIOIntData_t *)osal_msg_allocate(sizeof(OSALIOIntData_t));
            IOIntData->hdr.event = IOPORT_INT_EVENT;
            IOIntData->endPoint = ioIntResMap.endPointMap[HAL_IOPORT(1, idx)];
            IOIntData->arg = ioIntResMap.endPointArgMap[HAL_IOPORT(1, idx)];
            osal_msg_send(ioIntResMap.ioIntTskId, (uint8*)(IOIntData));
        }
    }
    flag = ioIntResMap.intInUse[2] & P2IFG;
    for(idx = 0; flag && (idx < 5); idx++)
    {
        if(BV(idx) & flag)
        {
            IOIntData = (OSALIOIntData_t *)osal_msg_allocate(sizeof(OSALIOIntData_t));
            IOIntData->hdr.event = IOPORT_INT_EVENT;
            IOIntData->endPoint = ioIntResMap.endPointMap[HAL_IOPORT(2, idx)];
            IOIntData->arg = ioIntResMap.endPointArgMap[HAL_IOPORT(2, idx)];
            osal_msg_send(ioIntResMap.ioIntTskId, (uint8*)(IOIntData));
        }
    }
    // 中断事件处理完毕,清除硬件中断标志位。
    P0IFG = 0;
    P1IFG = 0;
    P2IFG = 0;
}
