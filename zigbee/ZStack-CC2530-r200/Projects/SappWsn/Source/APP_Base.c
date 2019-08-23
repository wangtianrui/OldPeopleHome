#include "APP_Base.h"

#if defined(SAPP_ZSTACK_DEMO)
#include "hal_led.h"
// 任务建立实验范例代码
// 任务处理函数
uint16 Hello_ProcessEvent(uint8 task_id, uint16 events);
uint16 Hello_ProcessEvent(uint8 task_id, uint16 events)
{
    if(events & 0x0001)
    {
        // 控制LED闪烁
        HalLedBlink(HAL_LED_1, 1, 50, 250);
        // 启动定时器, 设置1秒钟后再次触发该任务
        osal_start_timerEx(task_id, 0x0001, 1000);
    }
    // 清除定时器事件标志
    return (events ^ 0x0001);
}
#endif

// 任务列表
const pTaskEventHandlerFn tasksArr[] = {
    macEventLoop,
    nwk_event_loop,
    Hal_ProcessEvent,
#if defined( MT_TASK )
    MT_ProcessEvent,
#endif
    APS_event_loop,
#if defined ( ZIGBEE_FRAGMENTATION )
    APSF_ProcessEvent,
#endif
    ZDApp_event_loop,
#if defined ( ZIGBEE_FREQ_AGILITY ) || defined ( ZIGBEE_PANID_CONFLICT )
    ZDNwkMgr_event_loop,
#endif
#if defined(SAPP_ZSTACK)
    sapp_controlEpProcess,
    sapp_functionEpProcess,
#endif
#if defined(SAPP_ZSTACK_DEMO)
    // 任务建立实验范例代码
    // 任务列表
    Hello_ProcessEvent,
#endif
};
const uint8 tasksCnt = sizeof(tasksArr)/sizeof(tasksArr[0]);

// 初始化任务
void osalInitTasks( void )
{
    uint8 taskID = 0;

    macTaskInit( taskID++ );
    nwk_init( taskID++ );
    Hal_Init( taskID++ );
#if defined( MT_TASK )
    MT_TaskInit( taskID++ );
#endif
    APS_Init( taskID++ );
#if defined ( ZIGBEE_FRAGMENTATION )
    APSF_Init( taskID++ );
#endif
    ZDApp_Init( taskID++ );
#if defined ( ZIGBEE_FREQ_AGILITY ) || defined ( ZIGBEE_PANID_CONFLICT )
    ZDNwkMgr_Init( taskID++ );
#endif
#if defined(SAPP_ZSTACK)
    sapp_taskInitProcess();
#endif
#if defined(SAPP_ZSTACK_DEMO)
    // 任务建立实验范例代码
    // 启动定时器
    osal_start_timerEx(taskID, 0x0001, 1000);
#endif
}
