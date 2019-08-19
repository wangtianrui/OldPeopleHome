#ifndef _SAPP_SAMPLE_H_
#define _SAPP_SAMPLE_H_
#include "SAPP_FrameWork.h"
#ifdef __cplusplus
extern "C"
{
#endif

enum {
    DevIRPers,
    DevIllum,
    DevExecuter,
    
    Devmax,
    DevPwrmanSys
};

#if !defined(ZDO_COORDINATOR)

//#define ILLUM_NODE
//#define IRPERS_NODE
#define EXECUTER_NODE

#endif

#ifdef __cplusplus
}
#endif
#endif//_SAPP_SAMPLE_H_