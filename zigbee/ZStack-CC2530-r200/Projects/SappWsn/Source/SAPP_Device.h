#ifndef _SAPP_DEVICE_H_
#define _SAPP_DEVICE_H_
#include "SAPP_FrameWork.h"
#ifdef __cplusplus
extern "C"
{
#endif

// 功能类型值定义
enum {
    DevTemp = 1,                    // 空气温度
    DevHumm,                        // 空气湿度
    DevILLum,                       // 光照度
    DevRain,                        // 雨滴
    DevIRDist,                      // 红外测距
    DevGas,                         // 燃气
    DevSmoke,                       // 烟雾
    DevFire,                        // 火焰
    DevIRPers,                      // 人体红外
    DevVoice,                       // 语音识别
    DevExecuteB,                    // 开关量输出执行器
    DevExecuteA,                    // 模拟量输出执行器
    DevRemoter,                     // 红外遥控
    Dev125kReader,                  // 125kHz读卡器
    DevSpeaker,                     // 语音报警
    DevTest,                        // 功能测试
    DevBroadcastSend,               // 广播发送
    DevBroadcastReceive,            // 广播接收
    DevIRDecode,                    // 红外遥控解码
    DevMaxNum,
};

#if !defined( ZDO_COORDINATOR ) && !defined( RTR_NWK )
// 节点功能定义
//#define HAS_GAS                   // 瓦斯传感器
//#define HAS_TEMP                  // 温度传感器
//#define HAS_HUMM                  // 湿度传感器
//#define HAS_RAIN                  // 雨滴传感器
//#define HAS_FIRE                  // 火焰传感器
//#define HAS_SMOKE                 // 烟雾传感器
//#define HAS_ILLUM                 // 光照度传感器
//#define HAS_IRPERS                // 人体红外传感器
//#define HAS_IRDIST                // 红外测距传感器
//#define HAS_VOICE                 // 语音传感器, 修改 HAL_UART_DMA 的定义为2
//#define HAS_EXECUTEB              // 执行器
//#define HAS_EXECUTEA              // 模拟执行器(预留扩展)
//#define HAS_REMOTER               // 红外遥控(预留扩展)
#define HAS_TESTFUNCTION          // 虚拟功能
//#define HAS_BROADCASTSEND         // 广播发送
//#define HAS_BROADCASTRECEIVE      // 广播接收
//#define HAS_125KREADER            // 125K电子标签阅读器
//#define HAS_SPEAKER               // 语音报警器
//#define HAS_IRDecode              // 红外解码
#endif

#ifdef __cplusplus
}
#endif
#endif//_SAPP_DEVICE_H_
