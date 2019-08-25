#if defined(SAPP_ZSTACK)
#include "SAPP_Device.h"
#include "hal_io.h"
#include <string.h>

/**************************************************************/
/* 传感器列表                                                 */
/**************************************************************/
/********************************/
/* 燃气传感器                   */
/********************************/
#if defined(HAS_GAS)
#define GAS_IO_GROUP        0
#define GAS_IO_BIT          0
void sensorGasResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res);
void sensorGasResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res)
{
    if(type == ResInit)
    {
        HalIOSetInput(GAS_IO_GROUP, GAS_IO_BIT, Pull_Down);
        HalIOIntSet(ep->ep, GAS_IO_GROUP, GAS_IO_BIT, IOInt_Rising, 0);
    }
    //IO端口中断触发，中断源检测
    if(type == ResIOInt)
    {
        uint8 GasValue = 1;
        SendData(ep->ep, &GasValue, 0x0000, TRANSFER_ENDPOINT, sizeof(GasValue));
    }
}
void sensorGasTimeout(struct ep_info_t *ep);
void sensorGasTimeout(struct ep_info_t *ep)
{
    uint8 value = HalIOGetLevel(GAS_IO_GROUP, GAS_IO_BIT);
    SendData(ep->ep, &value, 0x0000, TRANSFER_ENDPOINT, sizeof(value));
}
#endif
/********************************/
/* 温度传感器                   */
/********************************/
#if defined(HAS_TEMP) || defined(HAS_HUMM)
#include "sht10.h"
static uint16 TempValue = 0;
#endif
#if defined(HAS_TEMP)
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
#if defined(HAS_HUMM)
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
/********************************/
/* 雨滴传感器                   */
/********************************/
#if defined(HAS_RAIN)
#define RAIN_IO_GROUP       0
#define RAIN_IO_BIT         0
void sensorRainResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res);
void sensorRainResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res)
{
    if(type == ResInit)
    {
        HalIOSetInput(RAIN_IO_GROUP, RAIN_IO_BIT, Pull_None);
        HalIOIntSet(ep->ep, RAIN_IO_GROUP, RAIN_IO_BIT, IOInt_Rising, 0);
    }
    //IO端口中断触发，中断源检测
    if(type == ResIOInt)
    {
        uint8 RainValue = 1;
        SendData(ep->ep, &RainValue, 0x0000, TRANSFER_ENDPOINT, sizeof(RainValue));
    }
}
void sensorRainTimeout(struct ep_info_t *ep);
void sensorRainTimeout(struct ep_info_t *ep)
{
    uint8 value = HalIOGetLevel(RAIN_IO_GROUP, RAIN_IO_BIT);
    SendData(ep->ep, &value, 0x0000, TRANSFER_ENDPOINT, sizeof(value));
}
#endif
/********************************/
/* 火焰传感器                   */
/********************************/
#if defined(HAS_FIRE)
#define FIRE_IO_GROUP       0
#define FIRE_IO_BIT         0
void sensorFireResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res);
void sensorFireResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res)
{
    if(type == ResInit)
    {
        HalIOSetInput(FIRE_IO_GROUP, FIRE_IO_BIT, Pull_Down);
        HalIOIntSet(ep->ep, FIRE_IO_GROUP, FIRE_IO_BIT, IOInt_Rising, 0);
    }
    //IO端口中断触发，中断源检测
    if(type == ResIOInt)
    {
        uint8 FireValue = 1;
        SendData(ep->ep, &FireValue, 0x0000, TRANSFER_ENDPOINT, sizeof(FireValue));
    }
}
void sensorFireTimeout(struct ep_info_t *ep);
void sensorFireTimeout(struct ep_info_t *ep)
{
    uint8 value = HalIOGetLevel(FIRE_IO_GROUP, FIRE_IO_BIT);
    SendData(ep->ep, &value, 0x0000, TRANSFER_ENDPOINT, sizeof(value));
}
#endif
/********************************/
/* 烟雾传感器                   */
/********************************/
#if defined(HAS_SMOKE)
#define SMOKE_IO_GROUP      0
#define SMOKE_IO_BIT        0
void sensorSmokeResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res);
void sensorSmokeResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res)
{
    if(type == ResInit)
    {
        HalIOSetInput(SMOKE_IO_GROUP, SMOKE_IO_BIT, Pull_Down);
        HalIOIntSet(ep->ep, SMOKE_IO_GROUP, SMOKE_IO_BIT, IOInt_Rising, 0);
    }
    //IO端口中断触发，中断源检测
    if(type == ResIOInt)
    {
        uint8 SmokeValue = 1;
        SendData(ep->ep, &SmokeValue, 0x0000, TRANSFER_ENDPOINT, sizeof(SmokeValue));
    }
}
void sensorSmokeTimeout(struct ep_info_t *ep);
void sensorSmokeTimeout(struct ep_info_t *ep)
{
    uint8 value = HalIOGetLevel(SMOKE_IO_GROUP, SMOKE_IO_BIT);
    SendData(ep->ep, &value, 0x0000, TRANSFER_ENDPOINT, sizeof(value));
}
#endif
/********************************/
/* 光照度传感器                 */
/********************************/
#if defined(HAS_ILLUM)
#include "hal_adc.h"
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
/* 安防传感器                   */
/********************************/
#if defined(HAS_IRPERS)
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
/* 红外测距传感器               */
/********************************/
#if defined(HAS_IRDIST)
#include "hal_adc.h"
extern uint16 irDistTab[256];
void sensorIRDistTimeout(struct ep_info_t *ep);
void sensorIRDistTimeout(struct ep_info_t *ep)
{
    uint8 value = HalAdcRead(0, HAL_ADC_RESOLUTION_14) >> 3;
    // 计算距离值value的单位为mm
    uint16 IRDistValue = irDistTab[value];
    SendData(ep->ep, &IRDistValue, 0x0000, TRANSFER_ENDPOINT, sizeof(IRDistValue));
}
#endif
/********************************/
/* 语音传感器                   */
/********************************/
#if defined(HAS_VOICE)
#include "hal_uart.h"
static struct ep_info_t *voiceEndPoint = NULL;
static uint8 LastVoiceData = 0;
static void sensorVoiceUartProcess( uint8 port, uint8 event );
static void sensorVoiceUartProcess( uint8 port, uint8 event )
{
    (void)event;  // Intentionally unreferenced parameter
    while (Hal_UART_RxBufLen(port))
    {
        HalUARTRead(port, &LastVoiceData, 1);
        if(LastVoiceData == 0xAA)
            LastVoiceData = 1;
        else if(LastVoiceData == 0x55)
            LastVoiceData = 0;
        else
            LastVoiceData = -1;
        if(voiceEndPoint != NULL)
            SendData(voiceEndPoint->ep, &LastVoiceData, 0x0000, TRANSFER_ENDPOINT, 1);
    }
}
void sensorVoiceNwkStateChange(struct ep_info_t *ep);
void sensorVoiceNwkStateChange(struct ep_info_t *ep)
{
    voiceEndPoint = ep;
}
void sensorVoiceResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res);
void sensorVoiceResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res)
{
    if(type == ResInit)
    {
        halUARTCfg_t uartConfig;

        voiceEndPoint = ep;
        /* UART Configuration */
        uartConfig.configured           = TRUE;
        uartConfig.baudRate             = HAL_UART_BR_9600;
        uartConfig.flowControl          = FALSE;
        uartConfig.flowControlThreshold = MT_UART_DEFAULT_THRESHOLD;
        uartConfig.rx.maxBufSize        = MT_UART_DEFAULT_MAX_RX_BUFF;
        uartConfig.tx.maxBufSize        = MT_UART_DEFAULT_MAX_TX_BUFF;
        uartConfig.idleTimeout          = MT_UART_DEFAULT_IDLE_TIMEOUT;
        uartConfig.intEnable            = TRUE;
        uartConfig.callBackFunc         = sensorVoiceUartProcess;
        HalUARTOpen(HAL_UART_PORT_1, &uartConfig);
    }
}
void sensorVoiceTimeout(struct ep_info_t *ep);
void sensorVoiceTimeout(struct ep_info_t *ep)
{
    uint8 nulData = 0;
    SendData(ep->ep, &nulData, 0x0000, TRANSFER_ENDPOINT, 1);
}
#endif
/********************************/
/* 二进制执行器传感器           */
/********************************/
#if defined(HAS_EXECUTEB)
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
/* 模拟执行器传感器             */
/********************************/
#if defined(HAS_EXECUTEA)
void outputExecuteA(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg);
void outputExecuteA(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg)
{
}
#endif
/********************************/
/* 遥控器传感器                 */
/********************************/
#if defined(HAS_REMOTER)
#include "IR.h"
static uint8 lastCode[32];
static uint8 lastCodeLen = 0;
void IRSendResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res);
void IRSendResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res)
{
    if(type == ResInit)
        IRSendInit();   // 需要初始化
}
void outputRemoter(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg);
void outputRemoter(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg)
{
   lastCodeLen = msg->Data[0];
   memcpy(lastCode, &msg->Data[1], lastCodeLen);
   GenIR(&msg->Data[1], IRGuideLen_9ms, (uint8)lastCodeLen);
   SendData(ep->ep, lastCode, 0x0000, TRANSFER_ENDPOINT, lastCodeLen>>3);
}
void outputRemoterTimeout(struct ep_info_t *ep);
void outputRemoterTimeout(struct ep_info_t *ep)
{
    if(lastCodeLen <= 0)
    {
        uint8 value = 0;
        SendData(ep->ep, &value, 0x0000, TRANSFER_ENDPOINT, 1);
    }
    else
        SendData(ep->ep, lastCode, 0x0000, TRANSFER_ENDPOINT, lastCodeLen>>3);
}
#endif
/********************************/
/* 虚拟功能                     */
/********************************/
#if defined(HAS_TESTFUNCTION)
#define TEST_STRING     "Z-Stack for SAPP"
static uint8 lastData[119] = TEST_STRING;
static uint8 lastLen = 0;
void testFunc_RecvData(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg);
void testFunc_RecvData(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg)
{
    lastLen = msg->DataLength;
    memcpy(&lastData[sizeof(TEST_STRING) - 1], msg->Data, lastLen);
    SendData(ep->ep, lastData, 0x0000, TRANSFER_ENDPOINT,
                     lastLen + sizeof(TEST_STRING) - 1);
}
void testFunc_TimeOut(struct ep_info_t *ep);
void testFunc_TimeOut(struct ep_info_t *ep)
{
    SendData(ep->ep, lastData, 0x0000, TRANSFER_ENDPOINT,
                     lastLen + sizeof(TEST_STRING) - 1);
}
#endif
/********************************/
/* 广播发送功能                 */
/********************************/
#if defined(HAS_BROADCASTSEND)
#define BROADCAST_STRING     "\r\nBroadcast Message\r\n"
void BroadcastSend_TimeOut(struct ep_info_t *ep);
void BroadcastSend_TimeOut(struct ep_info_t *ep)
{
    SendData(ep->ep, BROADCAST_STRING, 0XFFFF, TRANSFER_ENDPOINT,
                     sizeof(BROADCAST_STRING) - 1);
    //广播后指示灯 LED_D9 闪烁2次
    HalLedBlink( HAL_LED_2, 2, 50, 100 );
}
#endif
/********************************/
/* 广播接收功能                 */
/********************************/
#if defined(HAS_BROADCASTRECEIVE)
void BroadcastReceiveData(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg);
void BroadcastReceiveData(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg)
{
    //收到广播数据后 LED_D8 连续闪烁4次!
    HalLedBlink( HAL_LED_1, 4, 50, 120 );
    HalUARTWrite(HAL_UART_PORT_0, msg->Data, msg->DataLength);
}
#define BROADCAST_STRING     "Broadcast Message"
void BroadcastSend_TimeOut(struct ep_info_t *ep);
void BroadcastSend_TimeOut(struct ep_info_t *ep)
{
    SendData(ep->ep, BROADCAST_STRING, 0X0000, TRANSFER_ENDPOINT,
                     sizeof(BROADCAST_STRING) - 1);
}
#endif
/********************************/
/* IC卡读卡器                   */
/********************************/
#if defined(HAS_125KREADER)
#include "hal_uart.h"
#define CARDID_SIZE     5                   // 卡号长度
static uint8 lastCardId[CARDID_SIZE];       // 用来保存接收到的卡号
static uint8 cardRecvIdx;                   // 上一次接收到的长度
static uint32 lastTick;                     // 上一次接收数据的系统时间
static struct ep_info_t *cardEndPoint;
static void sensor125kReaderUartProcess( uint8 port, uint8 event );
static void sensor125kReaderUartProcess( uint8 port, uint8 event )
{
    (void)event;  // Intentionally unreferenced parameter
    if((lastTick + 100) <= osal_GetSystemClock())
    {                                       // 如果本次接收到的时间距离上次超过了0.1秒
        cardRecvIdx = 0;                    // 则清空计数器，重新接收卡号
    }
    lastTick = osal_GetSystemClock();       // 记录下当前的时间
    while (Hal_UART_RxBufLen(port))
    {
        uint16 restLen = Hal_UART_RxBufLen(port);
        if(restLen > (CARDID_SIZE - cardRecvIdx))
            restLen = CARDID_SIZE - cardRecvIdx;
        HalUARTRead(port, &lastCardId[cardRecvIdx], restLen);
        cardRecvIdx += restLen;
        if(cardRecvIdx >= CARDID_SIZE)      // 如果已经接收完成一个完整的卡号
        {                                   // 则发送给协调器
            SendData(cardEndPoint->ep, lastCardId, 0x0000, TRANSFER_ENDPOINT, CARDID_SIZE);
        }
    }
}
void sensor125kReaderResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res);
void sensor125kReaderResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res)
{
    if(type == ResInit)
    {
        halUARTCfg_t uartConfig;

        memset(lastCardId, 0, sizeof(lastCardId));
        cardRecvIdx = 0;
        cardEndPoint = ep;
        /* UART Configuration */
        uartConfig.configured           = TRUE;
        uartConfig.baudRate             = HAL_UART_BR_19200;
        uartConfig.flowControl          = FALSE;
        uartConfig.flowControlThreshold = MT_UART_DEFAULT_THRESHOLD;
        uartConfig.rx.maxBufSize        = MT_UART_DEFAULT_MAX_RX_BUFF;
        uartConfig.tx.maxBufSize        = MT_UART_DEFAULT_MAX_TX_BUFF;
        uartConfig.idleTimeout          = MT_UART_DEFAULT_IDLE_TIMEOUT;
        uartConfig.intEnable            = TRUE;
        uartConfig.callBackFunc         = sensor125kReaderUartProcess;
        HalUARTOpen(HAL_UART_PORT_0, &uartConfig);
    }
}
void sensor125kReaderTimeout(struct ep_info_t *ep);
void sensor125kReaderTimeout(struct ep_info_t *ep)
{
    uint8 nullId[CARDID_SIZE] = { 0x00 };
    SendData(cardEndPoint->ep, nullId, 0x0000, TRANSFER_ENDPOINT, CARDID_SIZE);
}
#endif
/********************************/
/* 语音播放节点                 */
/********************************/
#if defined(HAS_SPEAKER)
void OutputSpeakerResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res);
void OutputSpeakerResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res)
{
    if(type == ResInit)
        HalUART1HwInit();
}
void outputSpeaker(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg);
void outputSpeaker(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg)
{
    //msg->Data[], msg->DataLength, msg->TransSeqNumber
    HalUART1HwTxByte(msg->Data[0]);
//    SendData(ep->ep, &msg->Data[0], 0x0000, TRANSFER_ENDPOINT, 1);
}
void outputSpeakerTimeout(struct ep_info_t *ep);
void outputSpeakerTimeout(struct ep_info_t *ep)
{
    uint8 value = 0;
    SendData(ep->ep, &value, 0x0000, TRANSFER_ENDPOINT, sizeof(value));
}
#endif
/********************************/
/* 红外接收解码                 */
/********************************/
#if defined(HAS_IRDecode)
#include <hal_irdec.h>
void IRDecodeResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res);
void IRDecodeResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res)
{
    if(type == ResInit)
    {
        IRDecodeT1Init(ep->task_id, ep->ep);
    }
    //定时器1通道0一次红外解码结束,向上传送解码结果
    if(type == ResTimerInt)
    {
        OSALIRDecData_t *irData = (OSALIRDecData_t *)res;
        SendData(ep->ep, irData->irCode, 0x0000, TRANSFER_ENDPOINT, irData->irLen);
    }
}
void IRDecodeTimeout(struct ep_info_t *ep);
void IRDecodeTimeout(struct ep_info_t *ep)
{
    uint8 value = 0;
    SendData(ep->ep, &value, 0x0000, TRANSFER_ENDPOINT, sizeof(value));
}
#endif
/***************************************************/
/* 节点功能列表                                    */
/***************************************************/
struct ep_info_t funcList[] = {
#if defined(HAS_GAS)
    {
        // 加入网络,收到数据,超时处理,资源变化
        NULL, NULL, sensorGasTimeout, sensorGasResAvailable,
        { DevGas, 0, 5 },                   // type, id, refresh cycle
    },
#endif
#if defined(HAS_TEMP)
    {
        NULL, NULL, sensorTempTimeout, sensorTempResAvailable,
        { DevTemp, 1, 5 },                 // type, id, refresh cycle
    },
#endif
#if defined(HAS_HUMM)
    {
        NULL, NULL, sensorHummTimeout, sensorHummResAvailable,
        { DevHumm, 0, 5 },                 // type, id, refresh cycle
    },
#endif
#if defined(HAS_ILLUM)
    {
        NULL, NULL, sensorILLumTimeout, NULL,
        { DevILLum, 0, 3 },                // type, id, refresh cycle
    },
#endif
#if defined(HAS_RAIN)
    {
        NULL, NULL, sensorRainTimeout, sensorRainResAvailable,
        { DevRain, 0, 5 },                 // type, id, refresh cycle
    },
#endif
#if defined(HAS_IRDIST)
    {
        NULL, NULL, sensorIRDistTimeout, NULL,
        { DevIRDist, 0, 3 },               // type, id, refresh cycle
    },
#endif
#if defined(HAS_SMOKE)
    {
        NULL, NULL, sensorSmokeTimeout, sensorSmokeResAvailable,
        { DevSmoke, 0, 5 },                 // type, id, refresh cycle
    },
#endif
#if defined(HAS_FIRE)
    {
        NULL, NULL, sensorFireTimeout, sensorFireResAvailable,
        { DevFire, 0, 3 },                  // type, id, refresh cycle
    },
#endif
#if defined(HAS_IRPERS)
    {
        NULL, NULL, sensorIRPersTimeout, sensorIRPersResAvailable,
        { DevIRPers, 0, 2 },                // type, id, refresh cycle
    },
#endif
#if defined(HAS_VOICE)
    {
        sensorVoiceNwkStateChange, NULL, sensorVoiceTimeout, sensorVoiceResAvailable,
        { DevVoice, 0, 5 },                // type, id, refresh cycle
    },
#endif
#if defined(HAS_EXECUTEB)
    {
        NULL, outputExecuteB, outputExecuteBTimeout, OutputExecuteBResAvailable,
        { DevExecuteB, 3, 10 },              // type, id, refresh cycle
    },
#endif
#if defined(HAS_EXECUTEA)
    {
        NULL, outputExecuteA, NULL, NULL,
        { DevExecuteA, 0, 3 },              // type, id, refresh cycle
    },
#endif
#if defined(HAS_REMOTER)
    {
        NULL, outputRemoter, outputRemoterTimeout, IRSendResAvailable,
        { DevRemoter, 0, 3 },              // type, id, refresh cycle
    },
#endif
#if defined(HAS_TESTFUNCTION)
    {
        NULL,
        testFunc_RecvData,
        testFunc_TimeOut,
        NULL,
        { DevTest, 0, 3 },
    },
#endif
#if defined(HAS_BROADCASTSEND)
    {
        NULL,
        NULL,
        BroadcastSend_TimeOut,
        NULL,
        { DevBroadcastSend, 0, 3 },
    },
#endif
#if defined(HAS_BROADCASTRECEIVE)
    {
        NULL,
        BroadcastReceiveData,
        BroadcastSend_TimeOut,
        NULL,
        { DevBroadcastReceive, 0, 30 },
    },
#endif
#if defined(HAS_125KREADER)
    {
        NULL, NULL, sensor125kReaderTimeout, sensor125kReaderResAvailable,
        { Dev125kReader, 0, 10 },
    },
#endif
#if defined(HAS_SPEAKER)
    {
        NULL, outputSpeaker, outputSpeakerTimeout, OutputSpeakerResAvailable,
        { DevSpeaker, 0, 9 },              // type, id, refresh cycle
    },
#endif
#if defined(HAS_IRDecode)
    {
        NULL, NULL, IRDecodeTimeout, IRDecodeResAvailable,
        { DevIRDecode, 0, 5 },              // type, id, refresh cycle
    },
#endif
#if defined(ZDO_COORDINATOR)
    {   // 协调器
        CoordinatorNwkStateChangeRoutine,
        CoordinatorIncomingRoutine,
        CoordinatorTimeoutRoutine,
        CoordinatorResAvailableRoutine,
        { DevCoordinator, 0, 0 },
    },
#elif defined(RTR_NWK)
    {   // 路由器
        RouterNwkStateChangeRoutine,
        RouterIncomingRoutine,
        RouterTimeoutRoutine,
        RouterResAvailableRoutine,
        { DevRouter, 0, 30 },
    },
#endif
};

// 不能修改下面的内容!!!
const uint8 funcCount = sizeof(funcList) / sizeof(funcList[0]);
#endif
