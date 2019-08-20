#include "SAPP_Sample.h"
#include "Sensor.h"
#include <string.h>

void sampleNwkStateChang(struct ep_info_t *ep);
void sampleNwkStateChang(struct ep_info_t *ep)
{
}
void sampleIncomingData(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg);
void sampleIncomingData(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg)
{
    //msg->Data[], msg->DataLength, msg->TransSeqNumber
}
void sampleTimeOut(struct ep_info_t *ep);
void sampleTimeOut(struct ep_info_t *ep)
{
}
void sampleResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res);
void sampleResAvailable(struct ep_info_t *ep, RES_TYPE type, void *res)
{
    switch(type)
    {
    case ResInit:
        break;
    case ResUserTimer:
        break;
    case ResControlPkg:
        break;
    }
}

struct ep_info_t funcList[] = {
    {
        sampleNwkStateChang,
        sampleIncomingData,
        sampleTimeOut,
        sampleResAvailable,
        { DevSensorId, 0, 3 },
    },
};

// 不能修改下面的内容!!!
const uint8 funcCount = sizeof(funcList) / sizeof(funcList[0]);
