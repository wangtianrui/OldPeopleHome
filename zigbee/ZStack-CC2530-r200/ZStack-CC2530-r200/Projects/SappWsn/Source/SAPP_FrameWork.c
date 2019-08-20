#if defined(SAPP_ZSTACK)
#include "SAPP_FrameWork.h"
#include <string.h>

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/
static void createEndPoint(struct ep_info_t *epInfo, uint8 *task_id, uint8 ep);
#if defined(ZDO_COORDINATOR)
static uint8 uartMsgProcesser(uint8 *msg);
#endif

/*********************************************************************
 * Local Variables
 *********************************************************************/
//uint8 ctrlBuffer[sizeof(TOPOINFO) + sizeof(FUNCTABLE) + FUNC_NUM * sizeof(FUNCINFO)];
static TOPOINFO topoBuffer = { 0x02 };
FUNCTABLE *funcTableBuffer;// = (FUNCTABLE *)(&ctrlBuffer[sizeof(TOPOINFO)]);

static devStates_t curNwkState;
static uint8 controlTaskId;
static uint8 functionTaskId;
static struct ep_info_t controlEndPointInfo;
static uint8 isUserTimerRunning = 0;

void sapp_taskInitProcess(void)
{
 #if defined ( BUILD_ALL_DEVICES )
    // The "Demo" target is setup to have BUILD_ALL_DEVICES and HOLD_AUTO_START
    // We are looking at a jumper (defined in SampleAppHw.c) to be jumpered
    // together - if they are - we will start up a coordinator. Otherwise,
    // the device will start as a router.
    if ( readCoordinatorJumper() )
        zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
    else
        zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
#endif // BUILD_ALL_DEVICES

#if defined ( HOLD_AUTO_START )
    // HOLD_AUTO_START is a compile option that will surpress ZDApp
    //  from starting the device and wait for the application to
    //  start the device.
    ZDOInitDevice(0);
#endif

    // 构造功能列表
    funcTableBuffer = createFuncTable(funcCount);
    funcTableBuffer->ft_type = 0x01;
    funcTableBuffer->ft_count = funcCount;
    int i;
    for(i = 0; i < funcCount; i++)
    {
        funcTableBuffer->ft_list[i].type = funcList[i].function.type;
        funcTableBuffer->ft_list[i].id = funcList[i].function.id;
        funcTableBuffer->ft_list[i].cycle = funcList[i].function.cycle;
    }
    controlTaskId = tasksCnt - 2;
    functionTaskId = tasksCnt - 1;
    HalIOInit(functionTaskId);
    createEndPoint(&controlEndPointInfo, &controlTaskId, CONTROL_ENDPOINT);
    for(i = 0; i < funcCount; i++)
    {
        struct ep_info_t *ep = &funcList[i];
        createEndPoint(ep, &functionTaskId, i + 1);
        if(ep->res_available)
          (*ep->res_available)(ep, ResInit, NULL);
    }
#if defined(ZDO_COORDINATOR)// || defined(RTR_NWK)
//    RegisterForKeys( SampleApp_TaskID );
    MT_UartRegisterTaskID(controlTaskId);
#endif
}
/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void createEndPoint(struct ep_info_t *epInfo, uint8 *task_id, uint8 ep)
{
    static cId_t commonClusterId = SAPP_PERIODIC_CLUSTERID;
    // Fill out the endpoint description.
    epInfo->task_id = *task_id;
    epInfo->ep = ep;
    epInfo->timerTick = epInfo->function.cycle;
    epInfo->userTimer = 0;

    epInfo->simpleDesc.EndPoint = ep;
    epInfo->simpleDesc.AppProfId = SAPP_PROFID;
    epInfo->simpleDesc.AppDeviceId = SAPP_DEVICEID;
    epInfo->simpleDesc.AppDevVer = SAPP_DEVICE_VERSION;
    epInfo->simpleDesc.Reserved = 0;
    epInfo->simpleDesc.AppNumInClusters = 1;
    epInfo->simpleDesc.pAppInClusterList = &commonClusterId;
    epInfo->simpleDesc.AppNumOutClusters = 1;
    epInfo->simpleDesc.pAppOutClusterList = &commonClusterId;

    epInfo->SampleApp_epDesc.endPoint = ep;
    epInfo->SampleApp_epDesc.task_id = task_id;
    epInfo->SampleApp_epDesc.simpleDesc = &epInfo->simpleDesc;
    epInfo->SampleApp_epDesc.latencyReq = noLatencyReqs;

    // Register the endpoint description with the AF
    afRegister(&epInfo->SampleApp_epDesc);
}

uint16 sapp_controlEpProcess(uint8 task_id, uint16 events)
{
    afIncomingMSGPacket_t *MSGpkt;

    if ( events & SYS_EVENT_MSG )
    {
        MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(task_id);
        while ( MSGpkt )
        {
            switch ( MSGpkt->hdr.event )
            {
#if defined(ZDO_COORDINATOR)
            case CMD_SERIAL_MSG:
//                SampleApp_UartMessage((uint8 *)MSGpkt);
                uartMsgProcesser((uint8 *)MSGpkt);
                HalLedBlink( HAL_LED_1, 2, 50, 90 );
                break;
#endif
                // Received when a messages is received (OTA) for this endpoint
            case AF_INCOMING_MSG_CMD:
            {
                // TODO: QueryProfile or QueryTopo
                switch(MSGpkt->clusterId)
                {
                case SAPP_PERIODIC_CLUSTERID:
                    switch(MSGpkt->cmd.Data[0])
                    {
                    case 0x01:
                        // CtrlQueryProfile
                        // 获取到数据包的来源地址来当做发送数据的目标
                        SendData(CONTROL_ENDPOINT, funcTableBuffer->ft_data, MSGpkt->srcAddr.addr.shortAddr, MSGpkt->srcAddr.endPoint, sizeof(FUNCTABLE) + funcCount * sizeof(FUNCINFO));
                        break;
                    case 0x02:
                        // CtrlQueryTopo
                        // 获取到数据包的来源地址来当做发送数据的目标
                        SendData(CONTROL_ENDPOINT, (unsigned char *)&topoBuffer, MSGpkt->srcAddr.addr.shortAddr, MSGpkt->srcAddr.endPoint, sizeof(TOPOINFO));
                        break;
                    case 0x03:
                        // CtrlQuerySpecialFunction
                        // cmd.Data[0] = 3, cmd.Data[1] = funcCode, cmd.Data[2] = funcID
                        {
                            uint8 i;
                            for(i = 0; i < funcTableBuffer->ft_count; i++)
                            {
                                if((funcTableBuffer->ft_list[i].type == MSGpkt->cmd.Data[1])
                                   && (funcTableBuffer->ft_list[i].id == MSGpkt->cmd.Data[2]))
                                {
                                    // 0x03, EndPoint, rCycle
                                    uint8 specialFunc[3] = { 0x03, i + 1, funcTableBuffer->ft_list[i].cycle };
                                    SendData(CONTROL_ENDPOINT, specialFunc, MSGpkt->srcAddr.addr.shortAddr, MSGpkt->srcAddr.endPoint, sizeof(specialFunc));
                                    break;
                                }
                            }
                        }
                        break;
                    default:
                        {
                            int i;
                            for(i = 0; i < funcCount; i++)
                            {
                                struct ep_info_t *ep = &funcList[i];
                                if(ep->res_available)   (*ep->res_available)(ep, ResControlPkg, MSGpkt);
                            }
                        }
                        break;
                    }
                    HalLedBlink( HAL_LED_2, 1, 50, 250 );
                    break;
                }
                break;
            }
            // Received whenever the device changes state in the network
            case ZDO_STATE_CHANGE:
            {
                devStates_t st = (devStates_t)(MSGpkt->hdr.status);
                if ( (st == DEV_ZB_COORD)
                        || (st == DEV_ROUTER)
                        || (st == DEV_END_DEVICE) )
                {
//                    topoBuffer->type = 0x02;
                    memcpy(topoBuffer.IEEE, NLME_GetExtAddr(), 8);
#if !defined(ZDO_COORDINATOR)
                    topoBuffer.PAddr = NLME_GetCoordShortAddr();
#else
                    topoBuffer.PAddr = 0xFFFF;
#endif
                    osal_memcpy(&topoBuffer.panid, &_NIB.nwkPanId, sizeof(uint16));
                    osal_memcpy(&topoBuffer.channel, &_NIB.nwkLogicalChannel, sizeof(uint8));
                    //向协调器发送拓扑信息
                    SendData(CONTROL_ENDPOINT, (unsigned char *)&topoBuffer, 0x0000, TRANSFER_ENDPOINT, sizeof(TOPOINFO));
                    HalLedBlink( HAL_LED_2, 4, 50, 250 );
                }
            }
            break;
            default:
                break;
            }
            // Release the memory
            osal_msg_deallocate( (uint8 *)MSGpkt );
            // Next - if one is available
            MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( task_id );
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }
    // 定时器时间到, 遍历所有端点看是否有userTimer
    if(events & SAPP_SEND_PERIODIC_MSG_EVT)
    {
        int i;
        uint8 hasUserTimer = 0;
        for(i = 0; i < funcCount; i++)
        {
            struct ep_info_t *ep = &funcList[i];
            if(ep->userTimer && ep->res_available)
            {
                hasUserTimer = 1;
                ep->userTimer = ep->userTimer - 1;
                if(ep->userTimer <= 1)
                {
                    ep->userTimer = 0;
                    (*ep->res_available)(ep, ResUserTimer, NULL);
                }
            }
        }
        if(hasUserTimer)
        {
            // 重新启动定时器
            osal_start_timerEx(task_id, SAPP_SEND_PERIODIC_MSG_EVT, 1000);
        }
        else
        {
            isUserTimerRunning = 0;
            osal_stop_timerEx(task_id, SAPP_SEND_PERIODIC_MSG_EVT);
        }
        // return unprocessed events
        return (events ^ SAPP_SEND_PERIODIC_MSG_EVT);
    }
    // Discard unknown events
    return 0;
}

uint16 sapp_functionEpProcess(uint8 task_id, uint16 events)
{
    afIncomingMSGPacket_t *MSGpkt;
    if(events & SYS_EVENT_MSG)
    {
        MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( task_id );
        while ( MSGpkt )
        {
            switch ( MSGpkt->hdr.event )
            {
            // 接收到数据包
            case AF_INCOMING_MSG_CMD:
                {
                    switch ( MSGpkt->clusterId )
                    {
                    case SAPP_PERIODIC_CLUSTERID:
                        if(MSGpkt->endPoint <= funcCount)
                        {
                            struct ep_info_t *ep = &funcList[MSGpkt->endPoint - 1];
                            if(ep->incoming_data)
                                (*ep->incoming_data)(ep, MSGpkt->srcAddr.addr.shortAddr, MSGpkt->srcAddr.endPoint, &MSGpkt->cmd);
                        }
                        HalLedBlink( HAL_LED_2, 1, 50, 250 );
                        break;
                    }
                }
                break;

            case ZDO_STATE_CHANGE:
                {
                    curNwkState = (devStates_t)(MSGpkt->hdr.status);
                    if ( (curNwkState == DEV_ZB_COORD)
                            || (curNwkState == DEV_ROUTER)
                            || (curNwkState == DEV_END_DEVICE) )
                    {
                        int i;
                        int hasTimeOut = 0;
                        for(i = 0; i < funcCount; i++)
                        {
                            struct ep_info_t *ep = &funcList[i];
                            if(ep->nwk_stat_change)
                                (*ep->nwk_stat_change)(ep);
                            // 重置端点计数器
                            if(ep->time_out && ep->function.cycle)
                            {
                                ep->timerTick = ep->function.cycle;
                                hasTimeOut = 1;
                            }
                        }
                        if(hasTimeOut)
                        {
                            // 加入网络成功,启动定时器,为各个端点提供定时
                            osal_start_timerEx(task_id,
                                               SAPP_SEND_PERIODIC_MSG_EVT,
                                               1000);
                        }
                    }
                    else
                        osal_stop_timerEx(task_id, SAPP_SEND_PERIODIC_MSG_EVT);
                }
                break;
            case IOPORT_INT_EVENT:
              {
                OSALIOIntData_t* IOIntData;
                IOIntData =(OSALIOIntData_t*)MSGpkt;
                if(IOIntData->endPoint <= funcCount)
                {
                    struct ep_info_t *ep = &funcList[IOIntData->endPoint - 1];
                    if(ep->res_available)
                        (*ep->res_available)(ep, ResIOInt, IOIntData->arg);
                }
              }
              break;
#if defined(HAL_IRDEC) && (HAL_IRDEC == TRUE)
            case IRDEC_INT_EVENT:   //
              {
                OSALIRDecIntData_t* TimerIntData = (OSALIRDecIntData_t*)MSGpkt;
                if(TimerIntData->endPoint <= funcCount)
                {
                    struct ep_info_t *ep = &funcList[TimerIntData->endPoint - 1];
                    if(ep->res_available)
                        (*ep->res_available)(ep, ResTimerInt, TimerIntData->data);
                }
              }
              break;
#endif
            default:
                break;
            }
            // Release the memory
            osal_msg_deallocate( (uint8 *)MSGpkt );
            // Next - if one is available
            MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( task_id );
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    // 定时器时间到, 遍历所有端点看是否有需要调用time_out
    if(events & SAPP_SEND_PERIODIC_MSG_EVT)
    {
        int i;
        for(i = 0; i < funcCount; i++)
        {
            struct ep_info_t *ep = &funcList[i];
            if(ep->time_out && ep->function.cycle)
            {
                // 端点需要周期执行
                ep->timerTick = ep->timerTick - 1;
                if(ep->timerTick == 0)
                {
                  // 定时时间到,执行time_out函数
                  (*ep->time_out)(ep);
                  ep->timerTick = ep->function.cycle;
                }
            }
#if 0
            if(ep->userTimer && ep->res_available)
            {
                ep->userTimer = ep->userTimer - 1;
                if(ep->userTimer <= 1)
                {
                    (*ep->res_available)(ep, ResUserTimer, NULL);
                    ep->userTimer = 0;
                }
            }
#endif
        }
        // 重新启动定时器
        osal_start_timerEx(task_id, SAPP_SEND_PERIODIC_MSG_EVT, 1000);
        // return unprocessed events
        return (events ^ SAPP_SEND_PERIODIC_MSG_EVT);
    }
    // Discard unknown events
    return 0;
}

#if defined(ZDO_COORDINATOR)
static uint8 uartMsgProcesser(uint8 *msg)
{
    mtOSALSerialData_t *pMsg = (mtOSALSerialData_t *)msg;
    mtUserSerialMsg_t *pMsgBody = (mtUserSerialMsg_t *)pMsg->msg;
    if ( (curNwkState != DEV_ZB_COORD)
            && (curNwkState != DEV_ROUTER)
            && (curNwkState != DEV_END_DEVICE) )
        return 1;
    switch(pMsgBody->cmd)
    {
    case 0x0018:
        {
            switch(pMsgBody->cmdEndPoint)
            {
            case 0xF1:
                {
                    // 转发数据
                    SendData(TRANSFER_ENDPOINT, pMsgBody->data,
                             pMsgBody->addr, pMsgBody->endPoint,
                             pMsgBody->len - 6);
                }
                break;
            }
        }
        break;
    }
    return 1;
}
#endif

uint8 SendData(uint8 srcEP, const void *buf, uint16 addr, uint8 dstEP, uint8 Len)
{
    static uint8 transID = 0;
    afAddrType_t SendDataAddr;
    struct ep_info_t *epInfo;

    if(srcEP <= funcCount)
        epInfo = &funcList[srcEP - 1];
    else
        epInfo = &controlEndPointInfo;

    SendDataAddr.addrMode = (afAddrMode_t)Addr16Bit;         //短地址发送
    SendDataAddr.endPoint = dstEP;
    SendDataAddr.addr.shortAddr = addr;
    if ( AF_DataRequest( &SendDataAddr, //发送的地址和模式
                         // TODO:
                         &epInfo->SampleApp_epDesc,   //终端（比如操作系统中任务ID等）
                         SAPP_PERIODIC_CLUSTERID,//发送串ID
                         Len,
                         (uint8*)buf,
                         &transID,  //信息ID（操作系统参数）
                         AF_DISCV_ROUTE,
                         AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
    {
        HalLedBlink( HAL_LED_1, 1, 50, 250 );
        return 1;
    }
    else
    {
        return 0;
    }
}

void CreateUserTimer(struct ep_info_t *ep, uint8 seconds)
{
    if(ep == NULL)
        return;
    if(ep->res_available == NULL)
        return;
    ep->userTimer = seconds;
    if(isUserTimerRunning == 0)
    {
        osal_start_timerEx(controlTaskId,
                           SAPP_SEND_PERIODIC_MSG_EVT,
                           1000);
        isUserTimerRunning = 1;
    }
}

void DeleteUserTimer(struct ep_info_t *ep)
{
    if(ep == NULL)
        return;
    ep->userTimer = 0;
}

void ModifyRefreshCycle(struct ep_info_t *ep, uint8 seconds)
{
    if(ep == NULL)
        return;
    if(ep->time_out == NULL)
        return;
    ep->function.cycle = seconds;
    if(ep->timerTick > seconds)
        ep->timerTick = seconds;
}

#if ! defined(ZDO_COORDINATOR) && defined(RTR_NWK)
void RouterTimeoutRoutine(struct ep_info_t *ep)
{
    SendData(ep->ep, (unsigned char *)&topoBuffer, 0x0000, TRANSFER_ENDPOINT, sizeof(TOPOINFO)); //节点向协调器发送采集数据
}
#endif

#if defined(ZDO_COORDINATOR)
void CoordinatorIncomingRoutine(struct ep_info_t *ep, uint16 addr, uint8 endPoint, afMSGCommandFormat_t *msg)
{
    //msg->Data[], msg->DataLength, msg->TransSeqNumber
    // 转发数据到串口
    if(msg->DataLength > 0)
    {
        mtUserSerialMsg_t *pMsg = osal_mem_alloc(sizeof(mtUserSerialMsg_t) + msg->DataLength - 1);
        pMsg->sop = MT_UART_SOF;
        pMsg->len = msg->DataLength + 6;
        pMsg->cmd = 0x0018;
        pMsg->cmdEndPoint = 0xF1;
        pMsg->addr = addr;
        pMsg->endPoint = endPoint;
        memcpy(pMsg->data, msg->Data, msg->DataLength);
        pMsg->fsc = MT_UartCalcFCS(0, &pMsg->len, 1);
        pMsg->fsc = MT_UartCalcFCS(pMsg->fsc, pMsg->dataBody, pMsg->len);
        HalUARTWrite(HAL_UART_PORT_0, &pMsg->sop, sizeof(mtUserSerialMsg_t) - 2 + msg->DataLength);
        HalUARTWrite(HAL_UART_PORT_0, &pMsg->fsc, 1);
        osal_mem_free(pMsg);
    }
}
#endif
#endif//SAPP_ZSTACK
