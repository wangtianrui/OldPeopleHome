#include "roomPwrManSys.h"
#include "hal_io.h"
#include <string.h>
#include "hal_adc.h"
#include "hal_assert.h"
#include "hal_board.h"
#include "hal_ccm.h"
#include "hal_defs.h"
#include "hal_drivers.h"
#include "hal_flash.h"
#include "hal_irdec.h"
#include "hal_key.h"
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_sleep.h"
#include "hal_timer.h"
#include "hal_uart.h"

/********************************/
/* 协调器代码                   */
/********************************/
#if defined(ZDO_COORDINATOR)
uint8 descPkg[] = {
    0x03, DevIRPers, 0
};

static uint16 nodeNwkAddr[Devmax];
static uint8 nodeEndPoint[Devmax];

static uint8 irPersStatus = 0;
static uint8 illumStatus = 0;
static uint8 controlStatus = 0;
void roomPwrManSys_StaChgRt(struct ep_info_t *ep);
void roomPwrManSys_StaChgRt(struct ep_info_t *ep)
{
    // 寻找人体红外节点 
    descPkg[1] = DevIRPers;
    SendData(ep->ep, descPkg, 0xFFFF, CONTROL_ENDPOINT, sizeof(descPkg));
}
void roomPwrManSys_IncmRt(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg);
void roomPwrManSys_IncmRt(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg)
{
    //msg->Data[], msg->DataLength, msg->TransSeqNumber
    if((endPoint == CONTROL_ENDPOINT) && (msg->Data[0] == 0x03))
    {
        // endPoint: msg->Data[1], rCycle: msg->Data[2]
        // 将上一次寻找的节点地址和端点号保存起来
        nodeNwkAddr[descPkg[1]] = addr;
        nodeEndPoint[descPkg[1]] = msg->Data[1];
        // 准备寻找下一个节点
        descPkg[1] = descPkg[1] + 1;
        // 所有节点是否都已经寻找完毕?
        if(descPkg[1] < Devmax)
            SendData(ep->ep, descPkg, 0xFFFF, CONTROL_ENDPOINT, sizeof(descPkg));
    }
    else
    {
        if(addr == nodeNwkAddr[DevIllum])
        {
            // 接收到光照度传感器数据
            uint16 i = 0;
            memcpy(&i, msg->Data, 2);
            illumStatus = i < 45000;
            HalUARTWrite(HAL_UART_PORT_0, msg->Data, 2);
        }
        else if(addr == nodeNwkAddr[DevIRPers])
        {
            // 接收到人体红外传感器数据
            irPersStatus = !!(msg->Data[0]);
        }
        if(nodeNwkAddr[DevExecuter] != 0xFFFF)
        {
            // 如果执行节点存在
            uint8 ctrl = 0;
            if(irPersStatus && illumStatus)
                ctrl = 0x0f;
            // 如果灯光的当前状态与需要设置的状态不一样则发送数据
            if(controlStatus != ctrl)
                SendData(ep->ep, &ctrl, nodeNwkAddr[DevExecuter], nodeEndPoint[DevExecuter], 1);
            controlStatus = ctrl;
        }
    }
}
void roomPwrManSys_ToRt(struct ep_info_t *ep);
void roomPwrManSys_ToRt(struct ep_info_t *ep)
{
    // 超时函数,用于检查节点搜索是否完成
    // 如果没有完成,则继续搜索
    if(descPkg[1] < Devmax)
    {
        SendData(ep->ep, descPkg, 0xFFFF, CONTROL_ENDPOINT, sizeof(descPkg));
    }
}
void roomPwrManSys_ResAvbRt(struct ep_info_t *ep, RES_TYPE type, void *res);
void roomPwrManSys_ResAvbRt(struct ep_info_t *ep, RES_TYPE type, void *res)
{
    switch(type)
    {
    case ResInit:
        memset(nodeNwkAddr, 0xFF, sizeof(nodeNwkAddr));
        memset(nodeEndPoint, 0xFF, sizeof(nodeEndPoint));
        break;
    case ResUserTimer:
        break;
    case ResControlPkg:
        break;
    }
}
#else
/********************************/
/* 人体红外节点代码             */
/********************************/
#if defined(IRPERS_NODE)
#define SAFTY_IO_GROUP      1
#define SAFTY_IO_BIT        0
void sensorIRPersResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res);
void sensorIRPersResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res)
{
    if(type == ResInit)
    {
        HalIOSetInput(SAFTY_IO_GROUP, SAFTY_IO_BIT, Pull_Down);
        HalIOIntSet(ep->ep, SAFTY_IO_GROUP, SAFTY_IO_BIT, IOInt_Rising, 0);
    }
    //IO端口中断触发，中断源检测
    if(type == ResIOInt)
    {
        uint8 IRPersValue = 1;
        SendData(ep->ep, &IRPersValue, 0x0000, TRANSFER_ENDPOINT, sizeof(IRPersValue));
    }
}
void sensorIRPersTimeout(struct ep_info_t *ep);
void sensorIRPersTimeout(struct ep_info_t *ep)
{
    uint8 value = HalIOGetLevel(SAFTY_IO_GROUP, SAFTY_IO_BIT);
    SendData(ep->ep, &value, 0x0000, TRANSFER_ENDPOINT, sizeof(value));
}
#endif
/********************************/
/* 光照度节点代码               */
/********************************/
#if defined(ILLUM_NODE)
void sensorILLumTimeout(struct ep_info_t *ep);
void sensorILLumTimeout(struct ep_info_t *ep)
{
    uint16 LightValue = 256 - (HalAdcRead(0, HAL_ADC_RESOLUTION_14) >> 3);
    // 将AD值变换为光照度的100倍
    LightValue = LightValue * 39;// * 10000 / 256;
    SendData(ep->ep, &LightValue, 0x0000, TRANSFER_ENDPOINT, sizeof(LightValue));
}
#endif
/********************************/
/* 执行节点代码                 */
/********************************/
#if defined(EXECUTER_NODE)
#define ControlInit()   do { HalIOSetOutput(1,4);HalIOSetOutput(1,5);HalIOSetOutput(1,6);HalIOSetOutput(1,7);Control(0); } while(0)
#define Control(mask)   do { HalIOSetLevel(1,4,mask&0x01);HalIOSetLevel(1,5,mask&0x02);HalIOSetLevel(1,6,mask&0x04);HalIOSetLevel(1,7,mask&0x08); } while(0)
void OutputExecuteBResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res);
void OutputExecuteBResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res)
{
    if(type == ResInit)
        ControlInit();
}
void outputExecuteB(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg);
void outputExecuteB(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg)
{
    //msg->Data[], msg->DataLength, msg->TransSeqNumber
    Control(msg->Data[0]);
    SendData(ep->ep, &msg->Data[0], 0x0000, TRANSFER_ENDPOINT, 1);
}
void outputExecuteBTimeout(struct ep_info_t *ep);
void outputExecuteBTimeout(struct ep_info_t *ep)
{
    uint8 value = P1 >> 4;
    SendData(ep->ep, &value, 0x0000, TRANSFER_ENDPOINT, sizeof(value));
}
#endif
/********************************/
/* 温度传感器                   */
/********************************/
#if defined(TEMP_NODE) || defined(HUMM_NODE)
#include "sht10.h"
static uint16 TempValue = 0;
#endif
#if defined(TEMP_NODE)
void sensorTempResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res);
void sensorTempResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res)
{
    if(type == ResInit)
    {
        SHT10_init(0x01);
    }
}
void sensorTempTimeout(struct ep_info_t *ep);
void sensorTempTimeout(struct ep_info_t *ep)
{
    unsigned int value = 0;
    unsigned char checksum = 0;
    SHT10_Measure(&value,&checksum, TEMPERATURE);
    TempValue = (value << 2) - 3960;
    SendData(ep->ep, &TempValue, 0x0000, TRANSFER_ENDPOINT, sizeof(TempValue));
}
#endif
/********************************/
/* 湿度传感器                   */
/********************************/
#if defined(HUMM_NODE)
void sensorHummResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res);
void sensorHummResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res)
{
    if(type == ResInit)
    {
        SHT10_init(0x01);
    }
}
void sensorHummTimeout(struct ep_info_t *ep);
void sensorHummTimeout(struct ep_info_t *ep)
{
    const float C1 = -4.0f;              // for 8 Bit
    const float C2 = +0.648f;            // for 8 Bit
    const float C3 = -0.0000072f;        // for 8 Bit
    const float T1 = 0.01f;              // for 8 bit
    const float T2 = 0.00128f;           // for 8 bit
    float rh_lin    =   0.0f;                     // rh_lin: Humidity linear
    float rh_true   =   0.0f;                    // rh_true: Temperature compensated humidity
    float t_C   = 0.0f;                        // t_C   : Temperature []

    unsigned int HumiValue = 0;
    unsigned char checksum = 0;
    SHT10_Measure(&HumiValue,&checksum, HUMIDITY);
    rh_lin=C3*HumiValue*HumiValue + C2*HumiValue + C1;     //calc. humidity from ticks to [%RH]
    rh_true=(t_C-25)*(T1+T2*HumiValue)+rh_lin;   //calc. temperature compensated humidity [%RH]
    if(rh_true>100)
        rh_true=100;       //cut if the value is outside of
    if(rh_true<0.1)
        rh_true=0.1f;       //the physical possible range
    HumiValue = (unsigned int)(rh_true * 100);
    SendData(ep->ep, &HumiValue, 0x0000, TRANSFER_ENDPOINT, sizeof(HumiValue));
}
#endif
#endif

struct ep_info_t funcList[] = {
#if defined(ZDO_COORDINATOR)
    {
        roomPwrManSys_StaChgRt,
        roomPwrManSys_IncmRt,
        roomPwrManSys_ToRt,
        roomPwrManSys_ResAvbRt,
        { DevPwrmanSys, 0, 3 },
    },
#else
# if defined(IRPERS_NODE)
    {
        NULL, NULL, sensorILLumTimeout, sensorIRPersResAvailable,
        { DevIRPers, 0, 2 },                // type, id, refresh cycle
    },
# elif defined(ILLUM_NODE)
    {
        NULL, NULL, sensorILLumTimeout, NULL,
        { DevIllum, 0, 5 },                // type, id, refresh cycle
    },
# elif defined(EXECUTER_NODE)
    {
        NULL, outputExecuteB, outputExecuteBTimeout, OutputExecuteBResAvailable,
        { DevExecuter, 0, 7 },              // type, id, refresh cycle
    },
#elif defined(TEMP_NODE)
    {
        NULL, NULL, sensorTempTimeout, sensorTempResAvailable,
        { DevTemp, 1, 5 },                 // type, id, refresh cycle
    },
#elif defined(HUMM_NODE)
    {
        NULL, NULL, sensorHummTimeout, sensorHummResAvailable,
        { DevHumm, 0, 5 },                 // type, id, refresh cycle
    },
# else
#  error You must define one device
# endif
#endif
};

// 不能修改下面的内容!!!
const uint8 funcCount = sizeof(funcList) / sizeof(funcList[0]);
