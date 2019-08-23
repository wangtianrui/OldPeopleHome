#ifndef _SAPP_SAMPLE_H_
#define _SAPP_SAMPLE_H_
#include "SAPP_FrameWork.h"
#ifdef __cplusplus
extern "C"
{
#endif

enum {
    DevIRPers,          //人体红外传感器
    DevIllum,           //光照度传感器
    DevExecuter,        //执行器
    DevTemp,            //温度传感器
    DevHumm,            //湿度传感器
    Devmax,             //设备数量
    DevPwrmanSys
};

#if !defined(ZDO_COORDINATOR)

//#define ILLUM_NODE    //光照节点
//#define IRPERS_NODE   //人体红外节点
#define EXECUTER_NODE   //执行器节点
//#define TEMP_NODE     //温度节点
//#define HUMM_NODE     //湿度节点

#endif

#ifdef __cplusplus
}
#endif
#endif//_SAPP_SAMPLE_H_