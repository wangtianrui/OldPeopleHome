/**************************************************************************************************
  Filename:       ZDObject.c
  Revised:        $Date: 2011-07-13 10:55:53 -0700 (Wed, 13 Jul 2011) $
  Revision:       $Revision: 26766 $

  Description:    This is the Zigbee Device Object.


  Copyright 2004-2011 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License"). You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product. Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "ZComdef.h"
#include "OSAL.h"
#include "OSAL_Nv.h"
#include "rtg.h"
#include "NLMEDE.h"
#include "nwk_globals.h"
#include "APS.h"
#include "APSMEDE.h"
#include "AssocList.h"
#include "BindingTable.h"
#include "AddrMgr.h"
#include "AF.h"
#include "ZDObject.h"
#include "ZDProfile.h"
#include "ZDConfig.h"
#include "ZDSecMgr.h"
#include "ZDApp.h"
#include "nwk_util.h"   // NLME_IsAddressBroadcast()
#include "ZGlobals.h"
#if defined MT_ZDO_CB_FUNC
#include "MT.h"
#endif

#if defined( LCD_SUPPORTED )
  #include "OnBoard.h"
#endif

/* HAL */
#include "hal_lcd.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
// NLME Stub Implementations
#define ZDO_ProcessMgmtPermitJoinTimeout NLME_PermitJoiningTimeout

// Status fields used by ZDO_ProcessMgmtRtgReq
#define ZDO_MGMT_RTG_ENTRY_ACTIVE             0x00
#define ZDO_MGMT_RTG_ENTRY_DISCOVERY_UNDERWAY 0x01
#define ZDO_MGMT_RTG_ENTRY_DISCOVERY_FAILED   0x02
#define ZDO_MGMT_RTG_ENTRY_INACTIVE           0x03

/*********************************************************************
 * TYPEDEFS
 */
#if defined ( REFLECTOR )
typedef struct
{
  byte SrcTransSeq;
  zAddrType_t SrcAddr;
  uint16 LocalCoordinator;
  byte epIntf;
  uint16 ProfileID;
  byte numInClusters;
  uint16 *inClusters;
  byte numOutClusters;
  uint16 *outClusters;
  byte SecurityUse;
  byte status;
} ZDO_EDBind_t;
#endif // defined ( REFLECTOR )

enum
{
  ZDMATCH_INIT,           // Initialized
  ZDMATCH_WAIT_REQ,       // Received first request, waiting for second
  ZDMATCH_SENDING_BINDS   // Received both requests, sending unbind/binds
};

enum
{
  ZDMATCH_SENDING_NOT,
  ZDMATCH_SENDING_UNBIND,
  ZDMATCH_SENDING_BIND
};

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static uint16 ZDOBuildBuf[26];  // temp area to build data without allocation

#if defined ( REFLECTOR )
static ZDO_EDBind_t *ZDO_EDBind;     // Null when not used
#endif

#if defined ( MANAGED_SCAN )
  uint32 managedScanNextChannel = 0;
  uint32 managedScanChannelMask = 0;
  uint8  managedScanTimesPerChannel = 0;
#endif

ZDMatchEndDeviceBind_t *matchED = (ZDMatchEndDeviceBind_t *)NULL;

uint32 apsChannelMask = 0;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void ZDODeviceSetup( void );
#if defined ( MANAGED_SCAN )
  static void ZDOManagedScan_Next( void );
#endif
#if defined ( REFLECTOR )
  static void ZDO_RemoveEndDeviceBind( void );
  static void ZDO_SendEDBindRsp( byte TransSeq, zAddrType_t *dstAddr, byte Status, byte secUse );
#endif
static byte ZDO_CompareClusterLists( byte numList1, uint16 *list1,
                                byte numList2, uint16 *list2, uint16 *pMatches );
static void ZDO_RemoveMatchMemory( void );
static uint8 ZDO_CopyMatchInfo( ZDEndDeviceBind_t *destReq, ZDEndDeviceBind_t *srcReq );
static void ZDO_EndDeviceBindMatchTimeoutCB( void );
uint8 *ZDO_ConvertOTAClusters( uint8 cnt, uint8 *inBuf, uint16 *outList );
static void zdoSendStateChangeMsg(uint8 state, uint8 taskId);

/*********************************************************************
 * @fn          ZDO_Init
 *
 * @brief       ZDObject and ZDProfile initialization.
 *
 * @param       none
 *
 * @return      none
 */
void ZDO_Init( void )
{
  // Initialize ZD items
  #if defined ( REFLECTOR )
  ZDO_EDBind = NULL;
  #endif

  // Initialize default ZDO_UseExtendedPANID to the APS one.
  osal_cpyExtAddr( ZDO_UseExtendedPANID, AIB_apsUseExtendedPANID );

  // Setup the device - type of device to create.
  ZDODeviceSetup();
}

#if defined ( MANAGED_SCAN )
/*********************************************************************
 * @fn      ZDOManagedScan_Next()
 *
 * @brief   Setup a managed scan.
 *
 * @param   none
 *
 * @return  none
 */
static void ZDOManagedScan_Next( void )
{
  // Is it the first time
  if ( managedScanNextChannel == 0 && managedScanTimesPerChannel == 0 )
  {
    // Setup the defaults
    managedScanNextChannel  = 1;

    while( managedScanNextChannel && (zgDefaultChannelList & managedScanNextChannel) == 0 )
      managedScanNextChannel <<= 1;

    managedScanChannelMask = managedScanNextChannel;
    managedScanTimesPerChannel = MANAGEDSCAN_TIMES_PRE_CHANNEL;
  }
  else
  {
    // Do we need to go to the next channel
    if ( managedScanTimesPerChannel == 0 )
    {
      // Find next active channel
      managedScanChannelMask  = managedScanNextChannel;
      managedScanTimesPerChannel = MANAGEDSCAN_TIMES_PRE_CHANNEL;
    }
    else
    {
      managedScanTimesPerChannel--;

      if ( managedScanTimesPerChannel == 0 )
      {
        managedScanNextChannel  <<= 1;
        while( managedScanNextChannel && (zgDefaultChannelList & managedScanNextChannel) == 0 )
          managedScanNextChannel <<= 1;

        if ( managedScanNextChannel == 0 )
          zdoDiscCounter  = NUM_DISC_ATTEMPTS + 1; // Stop
      }
    }
  }
}
#endif // MANAGED_SCAN

/*********************************************************************
 * @fn      ZDODeviceSetup()
 *
 * @brief   Call set functions depending on the type of device compiled.
 *
 * @param   none
 *
 * @return  none
 */
static void ZDODeviceSetup( void )
{
  if ( ZG_BUILD_COORDINATOR_TYPE )
  {
    NLME_CoordinatorInit();
  }

#if defined ( REFLECTOR )
  APS_ReflectorInit( (ZG_DEVICE_COORDINATOR_TYPE) ? APS_REFLECTOR_PUBLIC :  APS_REFLECTOR_PRIVATE );
#endif

  if ( ZG_BUILD_JOINING_TYPE )
  {
    NLME_DeviceJoiningInit();
  }
}

/*********************************************************************
 * @fn          ZDO_StartDevice
 *
 * @brief       This function starts a device in a network.
 *
 * @param       logicalType     - Device type to start
 *              startMode       - indicates mode of device startup
 *              beaconOrder     - indicates time betwen beacons
 *              superframeOrder - indicates length of active superframe
 *
 * @return      none
 */
void ZDO_StartDevice( byte logicalType, devStartModes_t startMode, byte beaconOrder, byte superframeOrder )
{
  ZStatus_t ret;
#if defined ( ZIGBEE_FREQ_AGILITY )
  static uint8 discRetries = 0;
#endif
#if defined ( ZIGBEE_COMMISSIONING )
  static uint8 scanCnt = 0;
#endif

  ret = ZUnsupportedMode;

  if ( ZG_BUILD_COORDINATOR_TYPE && logicalType == NODETYPE_COORDINATOR )
  {
    if ( startMode == MODE_HARD )
    {
      devState = DEV_COORD_STARTING;
      ret = NLME_NetworkFormationRequest( zgConfigPANID, zgApsUseExtendedPANID, zgDefaultChannelList,
                                          zgDefaultStartingScanDuration, beaconOrder,
                                          superframeOrder, false );
    }
    else if ( startMode == MODE_RESUME )
    {
      // Just start the coordinator
      devState = DEV_COORD_STARTING;
      ret = NLME_StartRouterRequest( beaconOrder, beaconOrder, false );
    }
    else
    {
#if defined( LCD_SUPPORTED )
      HalLcdWriteScreen( "StartDevice ERR", "MODE unknown" );
#endif
    }
  }

  if ( ZG_BUILD_JOINING_TYPE && (logicalType == NODETYPE_ROUTER || logicalType == NODETYPE_DEVICE) )
  {
    if ( (startMode == MODE_JOIN) || (startMode == MODE_REJOIN) )
    {
      devState = DEV_NWK_DISC;

  #if defined( MANAGED_SCAN )
      ZDOManagedScan_Next();
      ret = NLME_NetworkDiscoveryRequest( managedScanChannelMask, BEACON_ORDER_15_MSEC );
  #else
      ret = NLME_NetworkDiscoveryRequest( zgDefaultChannelList, zgDefaultStartingScanDuration );
    #if defined ( ZIGBEE_FREQ_AGILITY )
      if ( !( ZDO_Config_Node_Descriptor.CapabilityFlags & CAPINFO_RCVR_ON_IDLE ) &&
            ( ret == ZSuccess ) && ( ++discRetries == 4 ) )
      {
        // For devices with RxOnWhenIdle equals to FALSE, any network channel
        // change will not be recieved. On these devices or routers that have
        // lost the network, an active scan shall be conducted on the Default
        // Channel list using the extended PANID to find the network. If the
        // extended PANID isn't found using the Default Channel list, an scan
        // should be completed using all channels.
        zgDefaultChannelList = MAX_CHANNELS_24GHZ;
      }
    #endif // ZIGBEE_FREQ_AGILITY
    #if defined ( ZIGBEE_COMMISSIONING )
      if (startMode == MODE_REJOIN && scanCnt++ >= 5 )
      {
        // When ApsUseExtendedPanID is commissioned to a non zero value via
        // application specific means, the device shall conduct an active scan
        // on the Default Channel list and join the PAN with the same
        // ExtendedPanID. If the PAN is not found, an scan should be completed
        // on all channels.
        // When devices rejoin the network and the PAN is not found from
        zgDefaultChannelList = MAX_CHANNELS_24GHZ;
      }
    #endif // ZIGBEE_COMMISSIONING
  #endif
    }
    else if ( startMode == MODE_RESUME )
    {
      if ( logicalType == NODETYPE_ROUTER )
      {
        ZMacScanCnf_t scanCnf;
        devState = DEV_NWK_ORPHAN;

        /* if router and nvram is available, fake successful orphan scan */
        scanCnf.hdr.Status = ZSUCCESS;
        scanCnf.ScanType = ZMAC_ORPHAN_SCAN;
        scanCnf.UnscannedChannels = 0;
        scanCnf.ResultListSize = 0;
        nwk_ScanJoiningOrphan(&scanCnf);

        ret = ZSuccess;
      }
      else
      {
        devState = DEV_NWK_ORPHAN;
        ret = NLME_OrphanJoinRequest( zgDefaultChannelList,
                                      zgDefaultStartingScanDuration );
      }
    }
    else
    {
#if defined( LCD_SUPPORTED )
      HalLcdWriteScreen( "StartDevice ERR", "MODE unknown" );
#endif
    }
  }

  if ( ret != ZSuccess )
  {
    osal_start_timerEx(ZDAppTaskID, ZDO_NETWORK_INIT, NWK_RETRY_DELAY );
  }
}

/**************************************************************************************************
 * @fn          zdoSendStateChangeMsg
 *
 * @brief       Helper function for ZDO_UpdateNwkStatus.
 *
 * input parameters
 *
 * @param       taskId - The OSAL task identifier to which to send the ZDO_STATE_CHANGE_EVT.
 * @param       state - The current device state.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
static void zdoSendStateChangeMsg(uint8 state, uint8 taskId)
{
  osal_event_hdr_t *pMsg = (osal_event_hdr_t *)osal_msg_find(taskId, ZDO_STATE_CHANGE);

  if (NULL == pMsg)
  {
    if (NULL == (pMsg = (osal_event_hdr_t *)osal_msg_allocate(sizeof(osal_event_hdr_t))))
    {
      // Upon failure to notify any EndPoint of the state change, re-set the ZDO event to
      // try again later when more Heap may be available.
      osal_set_event(ZDAppTaskID, ZDO_STATE_CHANGE_EVT);
    }
    else
    {
      pMsg->event = ZDO_STATE_CHANGE;
      pMsg->status = state;

      (void)osal_msg_send(taskId, (uint8 *)pMsg);
    }
  }
  else
  {
    // Modify in place the status of an existing ZDO_STATE_CHANGE message to the EndPoint.
    pMsg->status = state;
  }
}

/**************************************************************************************************
 * @fn          ZDO_UpdateNwkStatus
 *
 * @brief       This function sends a ZDO_STATE_CHANGE message to the task of every EndPoint
 *              registered with AF (except, of course, the ZDO_EP). Even if a single task has more
 *              than one registered EndPoint, it will only receive one notification per state
 *              change. Although the device may go through a sequence of state changes, the
 *              Application task may only receive notification of the final, steady-state state
 *              because it has the lowest priority and never even runs to receive the intermediate
 *              state change notifications.
 *
 * input parameters
 *
 * @param       state - The current device state.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
void ZDO_UpdateNwkStatus(devStates_t state)
{
  epList_t *pItem = epList;

  while (pItem != NULL)
  {
    if (pItem->epDesc->endPoint != ZDO_EP)
    {
      zdoSendStateChangeMsg(state, *(pItem->epDesc->task_id));
    }

    pItem = pItem->nextDesc;
  }
#if defined MT_ZDO_CB_FUNC
  zdoSendStateChangeMsg(state, MT_TaskID);
#endif

  ZDAppNwkAddr.addr.shortAddr = NLME_GetShortAddr();
  (void)NLME_GetExtAddr();  // Load the saveExtAddr pointer.
}

#if defined ( REFLECTOR )
/*********************************************************************
 * @fn          ZDO_RemoveEndDeviceBind
 *
 * @brief       Remove the end device bind
 *
 * @param  none
 *
 * @return      none
 */
static void ZDO_RemoveEndDeviceBind( void )
{
  if ( ZDO_EDBind != NULL )
  {
    // Free the RAM
    if ( ZDO_EDBind->inClusters != NULL )
    {
      osal_mem_free( ZDO_EDBind->inClusters );
    }
    if ( ZDO_EDBind->outClusters != NULL )
    {
      osal_mem_free( ZDO_EDBind->outClusters );
    }
    osal_mem_free( ZDO_EDBind );
    ZDO_EDBind = NULL;
  }
}
#endif // REFLECTOR

#if defined ( REFLECTOR )
/*********************************************************************
 * @fn          ZDO_RemoveEndDeviceBind
 *
 * @brief       Remove the end device bind
 *
 * @param  none
 *
 * @return      none
 */
static void ZDO_SendEDBindRsp( byte TransSeq, zAddrType_t *dstAddr, byte Status, byte secUse )
{
  ZDP_EndDeviceBindRsp( TransSeq, dstAddr, Status, secUse );

#if defined( LCD_SUPPORTED )
  HalLcdWriteString( "End Device Bind", HAL_LCD_LINE_1 );
  if ( Status == ZDP_SUCCESS )
  {
    HalLcdWriteString( "Success Sent", HAL_LCD_LINE_2 );
  }
  else
  {
    HalLcdWriteString( "Timeout", HAL_LCD_LINE_2 );
  }
#endif

}
#endif // REFLECTOR

/*********************************************************************
 * @fn          ZDO_CompareClusterLists
 *
 * @brief       Compare one list to another list
 *
 * @param       numList1 - number of items in list 1
 * @param       list1 - first list of cluster IDs
 * @param       numList2 - number of items in list 2
 * @param       list2 - second list of cluster IDs
 * @param       pMatches - buffer to put matches
 *
 * @return      number of matches
 */
static byte ZDO_CompareClusterLists( byte numList1, uint16 *list1,
                          byte numList2, uint16 *list2, uint16 *pMatches )
{
  byte x, y;
  uint16 z;
  byte numMatches = 0;

  // Check the first in against the seconds out
  for ( x = 0; x < numList1; x++ )
  {
    for ( y = 0; y < numList2; y++ )
    {
      z = list2[y];
      if ( list1[x] == z )
      {
        pMatches[numMatches++] = z;
      }
    }
  }

  return ( numMatches );
}

/*********************************************************************
 * Utility functions
 */

/*********************************************************************
 * @fn          ZDO_CompareByteLists
 *
 * @brief       Compares two lists for matches.
 *
 * @param       ACnt  - number of entries in list A
 * @param       AList  - List A
 * @param       BCnt  - number of entries in list B
 * @param       BList  - List B
 *
 * @return      true if a match is found
 */
byte ZDO_AnyClusterMatches( byte ACnt, uint16 *AList, byte BCnt, uint16 *BList )
{
  byte x, y;

  for ( x = 0; x < ACnt; x++ )
  {
    for ( y = 0; y < BCnt; y++ )
    {
      if ( AList[x] == BList[y] )
      {
        return true;
      }
    }
  }

  return false;
}

/*********************************************************************
 * Callback functions from ZDProfile
 */

/*********************************************************************
 * @fn          ZDO_ProcessNodeDescReq
 *
 * @brief       This function processes and responds to the
 *              Node_Desc_req message.
 *
 * @param       inMsg - incoming message
 *
 * @return      none
 */
void ZDO_ProcessNodeDescReq( zdoIncomingMsg_t *inMsg )
{
  uint16 aoi = BUILD_UINT16( inMsg->asdu[0], inMsg->asdu[1] );
  NodeDescriptorFormat_t *desc = NULL;

  if ( aoi == ZDAppNwkAddr.addr.shortAddr )
  {
    desc = &ZDO_Config_Node_Descriptor;
  }

  if ( desc != NULL )
  {
    ZDP_NodeDescMsg( inMsg, aoi, desc );
  }
  else
  {
    ZDP_GenericRsp( inMsg->TransSeq, &(inMsg->srcAddr),
              ZDP_INVALID_REQTYPE, aoi, Node_Desc_rsp, inMsg->SecurityUse );
  }
}

/*********************************************************************
 * @fn          ZDO_ProcessPowerDescReq
 *
 * @brief       This function processes and responds to the
 *              Node_Power_req message.
 *
 * @param       inMsg  - incoming request
 *
 * @return      none
 */
void ZDO_ProcessPowerDescReq( zdoIncomingMsg_t *inMsg )
{
  uint16 aoi = BUILD_UINT16( inMsg->asdu[0], inMsg->asdu[1] );
  NodePowerDescriptorFormat_t *desc = NULL;

  if ( aoi == ZDAppNwkAddr.addr.shortAddr )
  {
    desc = &ZDO_Config_Power_Descriptor;
  }

  if ( desc != NULL )
  {
    ZDP_PowerDescMsg( inMsg, aoi, desc );
  }
  else
  {
    ZDP_GenericRsp( inMsg->TransSeq, &(inMsg->srcAddr),
              ZDP_INVALID_REQTYPE, aoi, Power_Desc_rsp, inMsg->SecurityUse );
  }
}

/*********************************************************************
 * @fn          ZDO_ProcessSimpleDescReq
 *
 * @brief       This function processes and responds to the
 *              Simple_Desc_req message.
 *
 * @param       inMsg - incoming message (request)
 *
 * @return      none
 */
void ZDO_ProcessSimpleDescReq( zdoIncomingMsg_t *inMsg )
{
  SimpleDescriptionFormat_t *sDesc = NULL;
  uint16 aoi = BUILD_UINT16( inMsg->asdu[0], inMsg->asdu[1] );
  byte endPoint = inMsg->asdu[2];
  byte free = false;
  byte stat = ZDP_SUCCESS;

  if ( (endPoint == ZDO_EP) || (endPoint > MAX_ENDPOINTS) )
  {
    stat = ZDP_INVALID_EP;
  }
  else if ( aoi == ZDAppNwkAddr.addr.shortAddr )
  {
    free = afFindSimpleDesc( &sDesc, endPoint );
    if ( sDesc == NULL )
    {
      stat = ZDP_NOT_ACTIVE;
    }
  }
  else
  {
    if ( ZSTACK_ROUTER_BUILD )
    {
      stat = ZDP_DEVICE_NOT_FOUND;
    }
    else if ( ZSTACK_END_DEVICE_BUILD )
    {
      stat = ZDP_INVALID_REQTYPE;
    }
  }

  ZDP_SimpleDescMsg( inMsg, stat, sDesc );

  if ( free && sDesc )
  {
    osal_mem_free( sDesc );
  }
}

/*********************************************************************
 * @fn          ZDO_ProcessActiveEPReq
 *
 * @brief       This function processes and responds to the
 *              Active_EP_req message.
 *
 * @param       inMsg  - incoming message (request)
 *
 * @return      none
 */
void ZDO_ProcessActiveEPReq( zdoIncomingMsg_t *inMsg )
{
  byte cnt = 0;
  uint16 aoi;
  byte stat = ZDP_SUCCESS;

  aoi = BUILD_UINT16( inMsg->asdu[0], inMsg->asdu[1] );

  if ( aoi == NLME_GetShortAddr() )
  {
    cnt = afNumEndPoints() - 1;  // -1 for ZDO endpoint descriptor
    afEndPoints( (uint8 *)ZDOBuildBuf, true );
  }
  else
  {
    stat = ZDP_INVALID_REQTYPE;
  }

  ZDP_ActiveEPRsp( inMsg->TransSeq, &(inMsg->srcAddr), stat,
                  aoi, cnt, (uint8 *)ZDOBuildBuf, inMsg->SecurityUse );
}

/*********************************************************************
 * @fn          ZDO_ConvertOTAClusters
 *
 * @brief       This function will convert the over-the-air cluster list
 *              format to an internal format.
 *
 * @param       inMsg  - incoming message (request)
 *
 * @return      pointer to incremented inBuf
 */
uint8 *ZDO_ConvertOTAClusters( uint8 cnt, uint8 *inBuf, uint16 *outList )
{
  uint8 x;

  for ( x = 0; x < cnt; x++ )
  {
    // convert ota format to internal
    outList[x] = BUILD_UINT16( inBuf[0], inBuf[1] );
    inBuf += sizeof( uint16 );
  }
  return ( inBuf );
}

/*********************************************************************
 * @fn          ZDO_ProcessMatchDescReq
 *
 * @brief       This function processes and responds to the
 *              Match_Desc_req message.
 *
 * @param       inMsg  - incoming message (request)
 *
 * @return      none
 */
void ZDO_ProcessMatchDescReq( zdoIncomingMsg_t *inMsg )
{
  uint8 epCnt = 0;
  uint8 numInClusters;
  uint16 *inClusters = NULL;
  uint8 numOutClusters;
  uint16 *outClusters = NULL;
  epList_t *epDesc;
  SimpleDescriptionFormat_t *sDesc = NULL;
  uint8 allocated;
  uint8 *msg;
  uint16 aoi;
  uint16 profileID;

  // Parse the incoming message
  msg = inMsg->asdu;
  aoi = BUILD_UINT16( msg[0], msg[1] );
  profileID = BUILD_UINT16( msg[2], msg[3] );
  msg += 4;

  if ( ADDR_BCAST_NOT_ME == NLME_IsAddressBroadcast(aoi) )
  {
    ZDP_MatchDescRsp( inMsg->TransSeq, &(inMsg->srcAddr), ZDP_INVALID_REQTYPE,
                          ZDAppNwkAddr.addr.shortAddr, 0, NULL, inMsg->SecurityUse );
    return;
  }
  else if ( (ADDR_NOT_BCAST == NLME_IsAddressBroadcast(aoi)) && (aoi != ZDAppNwkAddr.addr.shortAddr) )
  {
    ZDP_MatchDescRsp( inMsg->TransSeq, &(inMsg->srcAddr), ZDP_INVALID_REQTYPE,
                             ZDAppNwkAddr.addr.shortAddr, 0, NULL, inMsg->SecurityUse );
    return;
  }

  if ((numInClusters = *msg++) &&
      (inClusters = (uint16*)osal_mem_alloc( numInClusters * sizeof( uint16 ) )))
  {
    msg = ZDO_ConvertOTAClusters( numInClusters, msg, inClusters );
  }
  else
  {
    numInClusters = 0;
  }

  if ((numOutClusters = *msg++) &&
      (outClusters = (uint16 *)osal_mem_alloc( numOutClusters * sizeof( uint16 ) )))
  {
    msg = ZDO_ConvertOTAClusters( numOutClusters, msg, outClusters );
  }
  else
  {
    numOutClusters = 0;
  }

  // First count the number of endpoints that match.
  epDesc = epList;
  while ( epDesc )
  {
    // Don't search endpoint 0 and check if response is allowed
    if ( epDesc->epDesc->endPoint != ZDO_EP && (epDesc->flags&eEP_AllowMatch) )
    {
      if ( epDesc->pfnDescCB )
      {
        sDesc = (SimpleDescriptionFormat_t *)epDesc->pfnDescCB( AF_DESCRIPTOR_SIMPLE, epDesc->epDesc->endPoint );
        allocated = TRUE;
      }
      else
      {
        sDesc = epDesc->epDesc->simpleDesc;
        allocated = FALSE;
      }

      if ( sDesc && sDesc->AppProfId == profileID )
      {
        uint8 *uint8Buf = (uint8 *)ZDOBuildBuf;

        // Are there matching input clusters?
        if ((ZDO_AnyClusterMatches( numInClusters, inClusters,
                   sDesc->AppNumInClusters, sDesc->pAppInClusterList )) ||
            // Are there matching output clusters?
            (ZDO_AnyClusterMatches( numOutClusters, outClusters,
                   sDesc->AppNumOutClusters, sDesc->pAppOutClusterList )))
        {
          // Notify the endpoint of the match.
          uint8 bufLen = sizeof( ZDO_MatchDescRspSent_t ) + (numOutClusters + numInClusters) * sizeof(uint16);
          ZDO_MatchDescRspSent_t *pRspSent = (ZDO_MatchDescRspSent_t *) osal_msg_allocate( bufLen );

          if (pRspSent)
          {
            pRspSent->hdr.event = ZDO_MATCH_DESC_RSP_SENT;
            pRspSent->nwkAddr = inMsg->srcAddr.addr.shortAddr;
            pRspSent->numInClusters = numInClusters;
            pRspSent->numOutClusters = numOutClusters;

            if (numInClusters)
            {
              pRspSent->pInClusters = (uint16*) (pRspSent + 1);
              osal_memcpy(pRspSent->pInClusters, inClusters, numInClusters * sizeof(uint16));
            }
            else
            {
              pRspSent->pInClusters = NULL;
            }

            if (numOutClusters)
            {
              pRspSent->pOutClusters = (uint16*)(pRspSent + 1) + numInClusters;
              osal_memcpy(pRspSent->pOutClusters, outClusters, numOutClusters * sizeof(uint16));
            }
            else
            {
              pRspSent->pOutClusters = NULL;
            }

            osal_msg_send( *epDesc->epDesc->task_id, (uint8 *)pRspSent );
          }

          uint8Buf[epCnt++] = sDesc->EndPoint;
        }
      }

      if ( allocated )
      {
        osal_mem_free( sDesc );
      }
    }
    epDesc = epDesc->nextDesc;
  }

  if ( epCnt )
  {
    // Send the message if at least one match found.
    if ( ZSuccess == ZDP_MatchDescRsp( inMsg->TransSeq, &(inMsg->srcAddr), ZDP_SUCCESS,
              ZDAppNwkAddr.addr.shortAddr, epCnt, (uint8 *)ZDOBuildBuf, inMsg->SecurityUse ) )
    {
#if defined( LCD_SUPPORTED )
      HalLcdWriteScreen( "Match Desc Req", "Rsp Sent" );
#endif
    }
  }
  else
  {
    // No match found
    if (ADDR_NOT_BCAST == NLME_IsAddressBroadcast(aoi))
    {
      // send response message with match length = 0
      ZDP_MatchDescRsp( inMsg->TransSeq, &(inMsg->srcAddr), ZDP_SUCCESS,
                        ZDAppNwkAddr.addr.shortAddr, 0, (uint8 *)ZDOBuildBuf, inMsg->SecurityUse );
#if defined( LCD_SUPPORTED )
      HalLcdWriteScreen( "Match Desc Req", "Rsp Non Matched" );
#endif
    }
    else
    {
      // no response mesage for broadcast message
#if defined( LCD_SUPPORTED )
      HalLcdWriteScreen( "Match Desc Req", "Non Matched" );
#endif
    }
  }

  if ( inClusters != NULL )
  {
    osal_mem_free( inClusters );
  }

  if ( outClusters != NULL )
  {
    osal_mem_free( outClusters );
  }
}

/*********************************************************************
 * @fn      ZDO_ProcessBindUnbindReq()
 *
 * @brief   Called to process a Bind or Unbind Request message.
 *
 * @param   inMsg  - incoming message (request)
 * @param   pReq - place to put parsed information
 *
 * @return  none
 */
void ZDO_ProcessBindUnbindReq( zdoIncomingMsg_t *inMsg, ZDO_BindUnbindReq_t *pReq )
{
  zAddrType_t SourceAddr;       // Binding Source addres
  byte bindStat;

  SourceAddr.addrMode = Addr64Bit;
  osal_cpyExtAddr( SourceAddr.addr.extAddr, pReq->srcAddress );

  // If the local device is not the primary binding cache
  // check the src address of the bind request.
  // If it is not the local device's extended address
  // discard the request.
  if ( !osal_ExtAddrEqual( SourceAddr.addr.extAddr, NLME_GetExtAddr()) ||
        (pReq->dstAddress.addrMode != Addr64Bit &&
         pReq->dstAddress.addrMode != AddrGroup) )
  {
    bindStat = ZDP_NOT_SUPPORTED;
  }
  else
  {
    // Check source & destination endpoints
    if ( (pReq->srcEndpoint == 0 || pReq->srcEndpoint > MAX_ENDPOINTS)
        || (( pReq->dstAddress.addrMode == Addr64Bit ) &&
            (pReq->dstEndpoint == 0 || pReq->dstEndpoint > MAX_ENDPOINTS)) )
    {
      bindStat = ZDP_INVALID_EP;
    }
    else
    {
      if ( inMsg->clusterID == Bind_req )
      {
        // Assume the table is full
        bindStat = ZDP_TABLE_FULL;

#if defined( APP_TP ) || defined( APP_TP2 )
        // For ZigBee Conformance Testing
        if ( bindNumOfEntries() < gNWK_MAX_BINDING_ENTRIES )
#endif
        {
          if ( APSME_BindRequest( pReq->srcEndpoint, pReq->clusterID,
                         &(pReq->dstAddress), pReq->dstEndpoint ) == ZSuccess )
          {
            uint16 nwkAddr;

            // valid entry
            bindStat = ZDP_SUCCESS;

            // Notify to save info into NV
            ZDApp_NVUpdate();

            // Check for the destination address
            if ( pReq->dstAddress.addrMode == Addr64Bit )
            {
              if ( APSME_LookupNwkAddr( pReq->dstAddress.addr.extAddr, &nwkAddr ) == FALSE )
              {
                ZDP_NwkAddrReq( pReq->dstAddress.addr.extAddr, ZDP_ADDR_REQTYPE_SINGLE, 0, 0 );
              }
            }
          }
        }
      }
      else // Unbind_req
      {
        if ( APSME_UnBindRequest( pReq->srcEndpoint, pReq->clusterID,
                       &(pReq->dstAddress), pReq->dstEndpoint ) == ZSuccess )
        {
          bindStat = ZDP_SUCCESS;

          // Notify to save info into NV
          ZDApp_NVUpdate();
        }
        else
          bindStat = ZDP_NO_ENTRY;
      }
    }
  }

  // Send back a response message
  ZDP_SendData( &(inMsg->TransSeq), &(inMsg->srcAddr),
               (inMsg->clusterID | ZDO_RESPONSE_BIT), 1, &bindStat,
               inMsg->SecurityUse );
}

/*********************************************************************
 * @fn      ZDO_UpdateAddrManager
 *
 * @brief   Update the Address Manager.
 *
 * @param   nwkAddr - network address
 * @param   extAddr - extended address
 *
 * @return  none
 */
void ZDO_UpdateAddrManager( uint16 nwkAddr, uint8 *extAddr )
{
  AddrMgrEntry_t addrEntry;

  // Update the address manager
  addrEntry.user = ADDRMGR_USER_DEFAULT;
  addrEntry.nwkAddr = nwkAddr;
  AddrMgrExtAddrSet( addrEntry.extAddr, extAddr );
  AddrMgrEntryUpdate( &addrEntry );
}

/*********************************************************************
 * @fn      ZDO_ProcessServerDiscReq
 *
 * @brief   Process the Server_Discovery_req message.
 *
 * @param   inMsg  - incoming message (request)
 *
 * @return  none
 */
void ZDO_ProcessServerDiscReq( zdoIncomingMsg_t *inMsg )
{
  uint16 serverMask = BUILD_UINT16( inMsg->asdu[0], inMsg->asdu[1] );
  uint16 matchMask = serverMask & ZDO_Config_Node_Descriptor.ServerMask;

  if ( matchMask )
  {
    ZDP_ServerDiscRsp( inMsg->TransSeq, &(inMsg->srcAddr), ZSUCCESS,
                ZDAppNwkAddr.addr.shortAddr, matchMask, inMsg->SecurityUse );
  }
}

/*********************************************************************
 * Call Back Functions from APS  - API
 */

/*********************************************************************
 * @fn          ZDO_EndDeviceTimeoutCB
 *
 * @brief       This function handles the binding timer for the End
 *              Device Bind command.
 *
 * @param       none
 *
 * @return      none
 */
void ZDO_EndDeviceTimeoutCB( void )
{
#if defined ( REFLECTOR )
  byte stat;
  if ( ZDO_EDBind )
  {
    stat = ZDO_EDBind->status;

    // Send the response message to the first sent
    ZDO_SendEDBindRsp( ZDO_EDBind->SrcTransSeq, &(ZDO_EDBind->SrcAddr),
                        stat, ZDO_EDBind->SecurityUse );

    ZDO_RemoveEndDeviceBind();
  }
#endif  // REFLECTOR
}

/*********************************************************************
 * Optional Management Messages
 */

/*********************************************************************
 * @fn          ZDO_ProcessMgmtLqiReq
 *
 * @brief       This function handles parsing the incoming Management
 *              LQI request and generate the response.
 *
 *   Note:      This function will limit the number of items returned
 *              to ZDO_MAX_LQI_ITEMS items.
 *
 * @param       inMsg - incoming message (request)
 *
 * @return      none
 */
void ZDO_ProcessMgmtLqiReq( zdoIncomingMsg_t *inMsg )
{
  byte x;
  byte index;
  byte numItems;
  byte maxItems;
  ZDP_MgmtLqiItem_t* table = NULL;
  ZDP_MgmtLqiItem_t* item;
  neighborEntry_t    entry;
  byte aItems;
  associated_devices_t *aDevice;
  AddrMgrEntry_t  nwkEntry;
  uint8 StartIndex = inMsg->asdu[0];

  // Get the number of neighbor items
  NLME_GetRequest( nwkNumNeighborTableEntries, 0, &maxItems );

  // Get the number of associated items
  aItems = (uint8)AssocCount( PARENT, CHILD_FFD_RX_IDLE );

  // Total number of items
  maxItems += aItems;

  // Start with the supplied index
  if ( maxItems > StartIndex )
  {
    numItems = maxItems - StartIndex;

    // limit the size of the list
    if ( numItems > ZDO_MAX_LQI_ITEMS )
      numItems = ZDO_MAX_LQI_ITEMS;

    // Allocate the memory to build the table
    table = (ZDP_MgmtLqiItem_t*)osal_mem_alloc( (short)
              ( numItems * sizeof( ZDP_MgmtLqiItem_t ) ) );

    if ( table != NULL )
    {
      x = 0;
      item = table;
      index = StartIndex;

      // Loop through associated items and build list
      for ( ; x < numItems; x++ )
      {
        if ( index < aItems )
        {
          // get next associated device
          aDevice = AssocFindDevice( index++ );

          // set basic fields
          item->panID   = _NIB.nwkPanId;
          osal_cpyExtAddr( item->extPanID, _NIB.extendedPANID );
          item->nwkAddr = aDevice->shortAddr;
          item->permit  = ZDP_MGMT_BOOL_UNKNOWN;
          item->depth   = 0xFF;
          item->lqi     = aDevice->linkInfo.rxLqi;

          // set extented address
          nwkEntry.user    = ADDRMGR_USER_DEFAULT;
          nwkEntry.nwkAddr = aDevice->shortAddr;

          if ( AddrMgrEntryLookupNwk( &nwkEntry ) == TRUE )
          {
            osal_cpyExtAddr( item->extAddr, nwkEntry.extAddr );
          }
          else
          {
            osal_memset( item->extAddr, 0xFF, Z_EXTADDR_LEN );
          }

          // use association info to set other fields
          if ( aDevice->nodeRelation == PARENT )
          {
            if (  aDevice->shortAddr == 0 )
            {
              item->devType = ZDP_MGMT_DT_COORD;
              item->depth = 0;
            }
            else
            {
              item->devType = ZDP_MGMT_DT_ROUTER;
              item->depth = _NIB.nodeDepth - 1;
            }

            item->rxOnIdle = ZDP_MGMT_BOOL_UNKNOWN;
            item->relation = ZDP_MGMT_REL_PARENT;
          }
          else
          {
            // If not parent, then it's a child
            item->depth = _NIB.nodeDepth + 1;

            if ( aDevice->nodeRelation < CHILD_FFD )
            {
              item->devType = ZDP_MGMT_DT_ENDDEV;

              if ( aDevice->nodeRelation == CHILD_RFD )
              {
                item->rxOnIdle = FALSE;
              }
              else
              {
                item->rxOnIdle = TRUE;
              }
            }
            else
            {
              item->devType = ZDP_MGMT_DT_ROUTER;

              if ( aDevice->nodeRelation == CHILD_FFD )
              {
                item->rxOnIdle = FALSE;
              }
              else
              {
                item->rxOnIdle = TRUE;
              }
            }

            item->relation = ZDP_MGMT_REL_CHILD;
          }

          item++;
        }
        else
        {
          if ( StartIndex <= aItems )
            // Start with 1st neighbor
            index = 0;
          else
            // Start with >1st neighbor
            index = StartIndex - aItems;
          break;
        }
      }

      // Loop through neighbor items and finish list
      for ( ; x < numItems; x++ )
      {
        // Add next neighbor table item
        NLME_GetRequest( nwkNeighborTable, index++, &entry );

        // set ZDP_MgmtLqiItem_t fields
        item->panID    = entry.panId;
        osal_cpyExtAddr( item->extPanID, _NIB.extendedPANID );
        osal_memset( item->extAddr, 0xFF, Z_EXTADDR_LEN );
        item->nwkAddr  = entry.neighborAddress;
        item->rxOnIdle = ZDP_MGMT_BOOL_UNKNOWN;
        item->relation = ZDP_MGMT_REL_UNKNOWN;
        item->permit   = ZDP_MGMT_BOOL_UNKNOWN;
        item->depth    = 0xFF;
        item->lqi      = entry.linkInfo.rxLqi;

        if ( item->nwkAddr == 0 )
        {
          item->devType = ZDP_MGMT_DT_COORD;
        }
        else
        {
          item->devType = ZDP_MGMT_DT_ROUTER;
        }

        item++;
      }
    }
  }
  else
  {
    numItems = 0;
  }

  // Send response
  ZDP_MgmtLqiRsp( inMsg->TransSeq, &(inMsg->srcAddr), ZSuccess, maxItems,
                  StartIndex, numItems, table, false );

  if ( table )
  {
    osal_mem_free( table );
  }
}

/*********************************************************************
 * @fn          ZDO_ProcessMgmtNwkDiscReq
 *
 * @brief       This function handles parsing the incoming Management
 *              Network Discover request and starts the request.
 *
 * @param       inMsg - incoming message (request)
 *
 * @return      none
 */
void ZDO_ProcessMgmtNwkDiscReq( zdoIncomingMsg_t *inMsg )
{
  NLME_ScanFields_t scan;
  uint8             index;
  uint8             *msg;

  msg = inMsg->asdu;
  scan.channels = osal_build_uint32( msg, 4 );
  msg += 4;
  scan.duration = *msg++;
  index         = *msg;
  scan.scanType = ZMAC_ACTIVE_SCAN;
  scan.scanApp  = NLME_DISC_SCAN;

  // Save off the information to be used for the response
  zdappMgmtNwkDiscReqInProgress          = true;
  zdappMgmtNwkDiscRspAddr.addrMode       = Addr16Bit;
  zdappMgmtNwkDiscRspAddr.addr.shortAddr = inMsg->srcAddr.addr.shortAddr;
  zdappMgmtNwkDiscStartIndex             = index;
  zdappMgmtNwkDiscRspTransSeq            = inMsg->TransSeq;

  if ( NLME_NwkDiscReq2( &scan ) != ZSuccess )
  {
    NLME_NwkDiscTerm();

    // zdappMgmtNwkDiscReqInProgress will be reset in the confirm callback
  }
}

#if defined ( ZDO_MGMT_NWKDISC_RESPONSE )
/*********************************************************************
 * @fn          ZDO_FinishProcessingMgmtNwkDiscReq
 *
 * @brief       This function finishes the processing of the Management
 *              Network Discover Request and generates the response.
 *
 *   Note:      This function will limit the number of items returned
 *              to ZDO_MAX_NWKDISC_ITEMS items.
 *
 * @param       ResultCountSrcAddr - source of the request
 * @param       msg - pointer to incoming message
 * @param       SecurityUse -
 *
 * @return      none
 */
void ZDO_FinishProcessingMgmtNwkDiscReq( void )
{
  byte count, i, ResultCount = 0;
  networkDesc_t *newDesc = NULL, *pList, *NetworkList;

  NetworkList = nwk_getNwkDescList();

  // Count the number of nwk descriptors in the list
  pList = nwk_getNwkDescList();
  while (pList)
  {
    ResultCount++;
    pList = pList->nextDesc;
  }

  if ( ZSTACK_ROUTER_BUILD )
  {
    // Look for my PanID.
    pList = nwk_getNwkDescList();
    while ( pList )
    {
      if ( pList->panId == _NIB.nwkPanId )
      {
        break;
      }


      if ( !pList->nextDesc )
      {
        break;
      }
      pList = pList->nextDesc;
    }


    // If my Pan not present (query to a star network ZC or an isolated ZR?),
    // prepend it.
    if ( !pList || (pList->panId != _NIB.nwkPanId) )
    {
      newDesc = (networkDesc_t *)osal_mem_alloc( sizeof( networkDesc_t ) );
      if ( newDesc )
      {
        byte pJoin;

        newDesc->panId = _NIB.nwkPanId;
        newDesc->logicalChannel = _NIB.nwkLogicalChannel;
        newDesc->version = NLME_GetProtocolVersion();
        newDesc->stackProfile = zgStackProfile;

        //Extended PanID
        osal_cpyExtAddr( newDesc->extendedPANID, _NIB.extendedPANID);

        ZMacGetReq( ZMacAssociationPermit, &pJoin );
        newDesc->chosenRouter = ((pJoin) ? ZDAppNwkAddr.addr.shortAddr :
                                           INVALID_NODE_ADDR);

        newDesc->nextDesc = NetworkList;
        NetworkList = newDesc;
        ResultCount++;
      }
    }
  }

  // Calc the count and apply a max count.
  if ( zdappMgmtNwkDiscStartIndex > ResultCount )
  {
    count = 0;
  }
  else
  {
    count = ResultCount - zdappMgmtNwkDiscStartIndex;
    if ( count > ZDO_MAX_NWKDISC_ITEMS )
    {
      count = ZDO_MAX_NWKDISC_ITEMS;
    }

    // Move the list pointer up to the start index.
    for ( i = 0; i < zdappMgmtNwkDiscStartIndex; i++ )
    {
      NetworkList = NetworkList->nextDesc;
    }
  }

  ZDP_MgmtNwkDiscRsp( zdappMgmtNwkDiscRspTransSeq,
                     &zdappMgmtNwkDiscRspAddr, ZSuccess, ResultCount,
                      zdappMgmtNwkDiscStartIndex,
                      count,
                      NetworkList,
                      false );

  if ( ZSTACK_ROUTER_BUILD )
  {
    if ( newDesc != NULL )
    {
      osal_mem_free( newDesc );
    }
  }

  NLME_NwkDiscTerm();
}
#endif

/*********************************************************************
 * @fn          ZDO_ProcessMgmtRtgReq
 *
 * @brief       This function finishes the processing of the Management
 *              Routing Request and generates the response.
 *
 *   Note:      This function will limit the number of items returned
 *              to ZDO_MAX_RTG_ITEMS items.
 *
 * @param       inMsg - incoming message (request)
 *
 * @return      none
 */
void ZDO_ProcessMgmtRtgReq( zdoIncomingMsg_t *inMsg )
{
  byte x;
  byte maxNumItems;
  byte numItems = 0;
  uint8 *pBuf = NULL;
  rtgItem_t *pList;
  uint8 StartIndex = inMsg->asdu[0];

  // Get the number of table items
  NLME_GetRequest( nwkNumRoutingTableEntries, 0, &maxNumItems );

  if ( maxNumItems > StartIndex )
  {
    numItems = maxNumItems - StartIndex;    // Start at the passed in index

    // limit the size of the list
    if ( numItems > ZDO_MAX_RTG_ITEMS )
    {
      numItems = ZDO_MAX_RTG_ITEMS;
    }

    // Allocate the memory to build the table
    pBuf = osal_mem_alloc( (short)(sizeof( rtgItem_t ) * numItems) );

    if ( pBuf != NULL )
    {
      // Convert buffer to list
      pList = (rtgItem_t *)pBuf;

      // Loop through items and build list
      for ( x = 0; x < numItems; x++ )
      {
        NLME_GetRequest( nwkRoutingTable, (uint16)(x + StartIndex), (void*)pList );

        // Remap the status to the RoutingTableList Record Format defined in the ZigBee spec
        switch( pList->status )
        {
          case RT_ACTIVE:
            pList->status = ZDO_MGMT_RTG_ENTRY_ACTIVE;
            break;

          case RT_DISC:
            pList->status = ZDO_MGMT_RTG_ENTRY_DISCOVERY_UNDERWAY;
            break;

          case RT_LINK_FAIL:
            pList->status = ZDO_MGMT_RTG_ENTRY_DISCOVERY_FAILED;
            break;

          case RT_INIT:
          case RT_REPAIR:
          default:
            pList->status = ZDO_MGMT_RTG_ENTRY_INACTIVE;
            break;
        }

        // Increment pointer to next record
        pList++;
      }
    }
    else
    {
      numItems = 0;
    }
  }

  // Send response
  ZDP_MgmtRtgRsp( inMsg->TransSeq, &(inMsg->srcAddr), ZSuccess, maxNumItems, StartIndex, numItems,
                        (rtgItem_t *)pBuf, false );

  if ( pBuf != NULL )
  {
    osal_mem_free( pBuf );
  }
}

/*********************************************************************
 * @fn          ZDO_ProcessMgmtBindReq
 *
 * @brief       This function finishes the processing of the Management
 *              Bind Request and generates the response.
 *
 *   Note:      This function will limit the number of items returned
 *              to ZDO_MAX_BIND_ITEMS items.
 *
 * @param       inMsg - incoming message (request)
 *
 * @return      none
 */
void ZDO_ProcessMgmtBindReq( zdoIncomingMsg_t *inMsg )
{
#if defined ( REFLECTOR )
  byte x;
  uint16 maxNumItems;
  uint16 numItems;
  uint8 *pBuf = NULL;
  apsBindingItem_t *pList;
  uint8 StartIndex = inMsg->asdu[0];
  uint8 status;

  // Get the number of table items
  APSME_GetRequest( apsNumBindingTableEntries, 0, (byte*)(&maxNumItems) );

  if ( maxNumItems > StartIndex )
  {
    numItems = maxNumItems - StartIndex;    // Start at the passed in index
  }
  else
  {
    numItems = 0;
  }

  // limit the size of the list
  if ( numItems > ZDO_MAX_BIND_ITEMS )
  {
    numItems = ZDO_MAX_BIND_ITEMS;
  }

  // Allocate the memory to build the table
  if ( numItems && (pBuf = osal_mem_alloc( sizeof( apsBindingItem_t ) * numItems )) )
  {
    status = ZSuccess;

    // Convert buffer to list
    pList = (apsBindingItem_t *)pBuf;

    // Loop through items and build list
    for ( x = 0; x < numItems; x++ )
    {
      APSME_GetRequest( apsBindingTable, (x + StartIndex), (void*)pList );
      pList++;
    }

  }
  else
  {
    status = ZDP_NOT_PERMITTED;
    numItems = 0;
  }

  // Send response
  ZDP_MgmtBindRsp( inMsg->TransSeq, &(inMsg->srcAddr), status, (byte)maxNumItems, StartIndex,
                   (byte)numItems, (apsBindingItem_t *)pBuf, false );

  if ( pBuf )
  {
    osal_mem_free( pBuf );
  }
#else
  (void)inMsg;
#endif
}

/*********************************************************************
 * @fn          ZDO_ProcessMgmtDirectJoinReq
 *
 * @brief       This function finishes the processing of the Management
 *              Direct Join Request and generates the response.
 *
 * @param       inMsg - incoming message (request)
 *
 * @return      none
 */
void ZDO_ProcessMgmtDirectJoinReq( zdoIncomingMsg_t *inMsg )
{
  uint8 *deviceAddr;
  uint8 capInfo;
  uint8 stat;

  // Parse the message
  deviceAddr = inMsg->asdu;
  capInfo = inMsg->asdu[Z_EXTADDR_LEN];

  stat = (byte) NLME_DirectJoinRequest( deviceAddr, capInfo );

  ZDP_MgmtDirectJoinRsp( inMsg->TransSeq, &(inMsg->srcAddr), stat, false );
}

/*********************************************************************
 * @fn          ZDO_ProcessMgmtLeaveReq
 *
 * @brief       This function processes a Management Leave Request
 *              and generates the response.
 *
 * @param       inMsg - incoming message (request)
 *
 * @return      none
 */
void ZDO_ProcessMgmtLeaveReq( zdoIncomingMsg_t *inMsg )
{
  NLME_LeaveReq_t req;
  ZStatus_t       status;
  uint8           option;
  uint8 *msg = inMsg->asdu;

  if ( ( AddrMgrExtAddrValid( msg ) == FALSE                 ) ||
       ( osal_ExtAddrEqual( msg, NLME_GetExtAddr() ) == TRUE )    )
  {
    // Remove this device
    req.extAddr = NULL;
  }
  else
  {
    // Remove child device
    req.extAddr = msg;
  }

  option = msg[Z_EXTADDR_LEN];
  if ( option & ZDP_MGMT_LEAVE_REQ_RC )
  {
    req.removeChildren = TRUE;
  }

  if ( option & ZDP_MGMT_LEAVE_REQ_REJOIN )
  {
     req.rejoin = TRUE;
  }

  req.silent = FALSE;

  status = NLME_LeaveReq( &req );

  ZDP_MgmtLeaveRsp( inMsg->TransSeq, &(inMsg->srcAddr), status, FALSE );
}


/*********************************************************************
 * @fn          ZDO_ProcessMgmtPermitJoinReq
 *
 * @brief       This function processes a Management Permit Join Request
 *              and generates the response.
 *
 * @param       inMsg - incoming message (request)
 *
 * @return      none
 */
void ZDO_ProcessMgmtPermitJoinReq( zdoIncomingMsg_t *inMsg )
{
  uint8 stat;
  uint8 duration;
  uint8 tcsig;

  duration = inMsg->asdu[ZDP_MGMT_PERMIT_JOIN_REQ_DURATION];
  tcsig    = inMsg->asdu[ZDP_MGMT_PERMIT_JOIN_REQ_TC_SIG];

  // Set the network layer permit join duration
  stat = (byte) NLME_PermitJoiningRequest( duration );

  // Handle the Trust Center Significance
  if ( ZG_SECURE_ENABLED && ZG_BUILD_COORDINATOR_TYPE && ZG_DEVICE_COORDINATOR_TYPE )
  {
    if ( tcsig == TRUE )
    {
      ZDSecMgrPermitJoining( duration );
    }
  }

  // Send a response if unicast
  if ( !inMsg->wasBroadcast )
  {
    ZDP_MgmtPermitJoinRsp( inMsg->TransSeq, &(inMsg->srcAddr), stat, false );
  }
}

/*
 * This function stub allows the next higher layer to be notified of
 * a permit joining timeout.
 */
/*********************************************************************
 * @fn          ZDO_ProcessMgmtPermitJoinTimeout
 *
 * @brief       This function stub allows the next higher layer to be
 *              notified of a permit joining timeout. Currently, this
 *              directly bypasses the APS layer.
 *
 * @param       none
 *
 * @return      none
 */
void ZDO_ProcessMgmtPermitJoinTimeout( void )
{
  #if defined( ZDO_MGMT_PERMIT_JOIN_RESPONSE )
  // Currently, only the ZDSecMgr needs to be notified
  if ( ZG_SECURE_ENABLED && ZG_BUILD_COORDINATOR_TYPE && ZG_DEVICE_COORDINATOR_TYPE )
  {
    ZDSecMgrPermitJoiningTimeout();
  }
  #endif
}

/*********************************************************************
 * @fn          ZDO_ProcessUserDescReq
 *
 * @brief       This function finishes the processing of the User
 *              Descriptor Request and generates the response.
 *
 * @param       inMsg - incoming message (request)
 *
 * @return      none
 */
void ZDO_ProcessUserDescReq( zdoIncomingMsg_t *inMsg )
{
  uint16 aoi = BUILD_UINT16( inMsg->asdu[0], inMsg->asdu[1] );
  UserDescriptorFormat_t userDesc;

  if ( (aoi == ZDAppNwkAddr.addr.shortAddr) && (ZSUCCESS == osal_nv_read(
             ZCD_NV_USERDESC, 0, sizeof(UserDescriptorFormat_t), &userDesc )) )
  {
    ZDP_UserDescRsp( inMsg->TransSeq, &(inMsg->srcAddr), aoi, &userDesc, false );
  }
  else
  {
    ZDP_GenericRsp(inMsg->TransSeq, &(inMsg->srcAddr),
           ZDP_NOT_SUPPORTED, aoi, User_Desc_rsp, inMsg->SecurityUse );
  }
}

/*********************************************************************
 * @fn          ZDO_ProcessUserDescSet
 *
 * @brief       This function finishes the processing of the User
 *              Descriptor Set and generates the response.
 *
 * @param       inMsg - incoming message (request)
 *
 * @return      none
 */
void ZDO_ProcessUserDescSet( zdoIncomingMsg_t *inMsg )
{
  uint8 *msg;
  uint16 aoi;
  UserDescriptorFormat_t userDesc;
  uint8 outMsg[3];
  uint8 status;

  msg = inMsg->asdu;
  aoi = BUILD_UINT16( msg[0], msg[1] );

  if ( aoi == ZDAppNwkAddr.addr.shortAddr )
  {
    userDesc.len = (msg[2] < AF_MAX_USER_DESCRIPTOR_LEN) ? msg[2] : AF_MAX_USER_DESCRIPTOR_LEN;
    msg ++;  // increment one for the length field

    osal_memcpy( userDesc.desc, &msg[2], userDesc.len );
    osal_nv_write( ZCD_NV_USERDESC, 0, sizeof(UserDescriptorFormat_t), &userDesc );
    if ( userDesc.len != 0 )
    {
      ZDO_Config_Node_Descriptor.UserDescAvail = TRUE;
    }
    else
    {
      ZDO_Config_Node_Descriptor.UserDescAvail = FALSE;
    }

    status = ZDP_SUCCESS;
  }
  else
  {
    status =  ZDP_NOT_SUPPORTED;
  }

  outMsg[0] = status;
  outMsg[1] = LO_UINT16( aoi );
  outMsg[2] = LO_UINT16( aoi );

  ZDP_SendData( &(inMsg->TransSeq), &(inMsg->srcAddr), User_Desc_conf, 3, outMsg,
               inMsg->SecurityUse );
}

/*********************************************************************
 * @fn          ZDO_ProcessDeviceAnnce
 *
 * @brief       This function processes a device annouce message.
 *
 * @param       inMsg - incoming message
 *
 * @return      none
 */
void ZDO_ProcessDeviceAnnce( zdoIncomingMsg_t *inMsg )
{
  ZDO_DeviceAnnce_t Annce;
  AddrMgrEntry_t addrEntry;
  uint8 parentExt[Z_EXTADDR_LEN];

  // Parse incoming message
  ZDO_ParseDeviceAnnce( inMsg, &Annce );

  if ( ZSTACK_END_DEVICE_BUILD )
  {
    // Make sure the message didn't come from myself - end device only
    if ( osal_ExtAddrEqual( NLME_GetExtAddr(), Annce.extAddr ) && Annce.nwkAddr == NLME_GetShortAddr() )
    {
      return;
    }
  }

#if defined ( ZIGBEE_STOCHASTIC_ADDRESSING )
  // Clean up the neighbor table
  nwkNeighborRemoveAllStranded();

  // If address conflict is detected, no need to update the address manager
  if ( NLME_CheckNewAddrSet( Annce.nwkAddr, Annce.extAddr )== ZFailure )
  {
    return;
  }
#endif

#if defined ( ZIGBEE_STOCHASTIC_ADDRESSING )
  // Check for parent's address
  NLME_GetCoordExtAddr( parentExt );
  if ( osal_ExtAddrEqual( parentExt, Annce.extAddr ) )
  {
    if ( Annce.nwkAddr != NLME_GetCoordShortAddr() )
    {
      // Set the Parent's MAC's new short address
      _NIB.nwkCoordAddress = Annce.nwkAddr;
      ZMacSetReq( ZMacCoordShortAddress, (byte*)&(_NIB.nwkCoordAddress) );
    }
  }

  if ( ZSTACK_ROUTER_BUILD )
  {
    // If the device annce comes from a end device child that has moved
    // to another parent, remove it from associated device list

    // If the dev annce is coming from other device's children,
    // (The dev annce from its own children shall be unicast to itself,
    // So check the mac destination address)
    // Remove it from the associated device list. If it is not
    // a child, no action will be taken in AssocRemove() anyway.
    if ( inMsg->macDestAddr != NLME_GetShortAddr() )
    {
      associated_devices_t *dev_ptr;

      // If it's an end device child
      dev_ptr = AssocGetWithExt( Annce.extAddr );
      if ( dev_ptr )
      {
        if ( dev_ptr->nodeRelation == CHILD_RFD ||
             dev_ptr->nodeRelation == CHILD_RFD_RX_IDLE )
        {
          AssocRemove( Annce.extAddr );
        }
      }
    }

    if ( Annce.nwkAddr != NLME_GetShortAddr() )
    {
      // If an associated device is found with matched extended Address,
      // update its short address
      if ( AssocChangeNwkAddr( Annce.nwkAddr, Annce.extAddr ) )
      {
        // Set event to save NV
        ZDApp_NVUpdate();
      }
    }
  }

  // Update the neighbor table
  nwkNeighborUpdateNwkAddr( Annce.nwkAddr, Annce.extAddr );

  // Assume that the device has moved, remove existing routing entries
  RTG_RemoveRtgEntry( Annce.nwkAddr, 0 );

#endif // ZIGBEE_STOCHASTIC_ADDRESSING

  // Fill in the extended address in address manager if we don't have it already.
  addrEntry.user = ADDRMGR_USER_DEFAULT;
  addrEntry.nwkAddr = Annce.nwkAddr;
  if ( AddrMgrEntryLookupNwk( &addrEntry ) )
  {
    osal_memset( parentExt, 0, Z_EXTADDR_LEN );
    if ( osal_ExtAddrEqual( parentExt, addrEntry.extAddr ) )
    {
      AddrMgrExtAddrSet( addrEntry.extAddr, Annce.extAddr );
      AddrMgrEntryUpdate( &addrEntry );
    }
  }

  // Update the short address in address manager if it's been changed
  AddrMgrExtAddrSet( addrEntry.extAddr, Annce.extAddr );
  if ( AddrMgrEntryLookupExt( &addrEntry ) )
  {
    if ( addrEntry.nwkAddr != Annce.nwkAddr )
    {
      addrEntry.nwkAddr = Annce.nwkAddr;
      AddrMgrEntryUpdate( &addrEntry );
    }
  }
}

/*********************************************************************
 * @fn          ZDO_BuildSimpleDescBuf
 *
 * @brief       Build a byte sequence representation of a Simple Descriptor.
 *
 * @param       buf  - pointer to a byte array big enough for data.
 * @param       desc - SimpleDescriptionFormat_t *
 *
 * @return      none
 */
void ZDO_BuildSimpleDescBuf( uint8 *buf, SimpleDescriptionFormat_t *desc )
{
  byte cnt;
  uint16 *ptr;

  *buf++ = desc->EndPoint;
  *buf++ = HI_UINT16( desc->AppProfId );
  *buf++ = LO_UINT16( desc->AppProfId );
  *buf++ = HI_UINT16( desc->AppDeviceId );
  *buf++ = LO_UINT16( desc->AppDeviceId );

  *buf++ = (byte)(desc->AppDevVer << 4);

  *buf++ = desc->AppNumInClusters;
  ptr = desc->pAppInClusterList;
  for ( cnt = 0; cnt < desc->AppNumInClusters; ptr++, cnt++ )
  {
    *buf++ = HI_UINT16( *ptr );
    *buf++ = LO_UINT16( *ptr );
  }

  *buf++ = desc->AppNumOutClusters;
  ptr = desc->pAppOutClusterList;
  for ( cnt = 0; cnt < desc->AppNumOutClusters; ptr++, cnt++ )
  {
    *buf++ = HI_UINT16( *ptr );
    *buf++ = LO_UINT16( *ptr );
  }
}

/*********************************************************************
 * @fn      ZDO_MatchEndDeviceBind()
 *
 * @brief
 *
 *   Called to match end device binding requests
 *
 * @param  bindReq  - binding request information
 * @param  SecurityUse - Security enable/disable
 *
 * @return  none
 */
void ZDO_MatchEndDeviceBind( ZDEndDeviceBind_t *bindReq )
{
  zAddrType_t dstAddr;
  uint8 sendRsp = FALSE;
  uint8 status;

  // Is this the first request?
  if ( matchED == NULL )
  {
    // Create match info structure
    matchED = (ZDMatchEndDeviceBind_t *)osal_mem_alloc( sizeof ( ZDMatchEndDeviceBind_t ) );
    if ( matchED )
    {
      // Clear the structure
      osal_memset( (uint8 *)matchED, 0, sizeof ( ZDMatchEndDeviceBind_t ) );

      // Copy the first request's information
      if ( !ZDO_CopyMatchInfo( &(matchED->ed1), bindReq ) )
      {
        status = ZDP_NO_ENTRY;
        sendRsp = TRUE;
      }
    }
    else
    {
      status = ZDP_NO_ENTRY;
      sendRsp = TRUE;
    }

    if ( !sendRsp )
    {
      // Set into the correct state
      matchED->state = ZDMATCH_WAIT_REQ;

      // Setup the timeout
      APS_SetEndDeviceBindTimeout( AIB_MaxBindingTime, ZDO_EndDeviceBindMatchTimeoutCB );
    }
  }
  else
  {
      matchED->state = ZDMATCH_SENDING_BINDS;

      // Copy the 2nd request's information
      if ( !ZDO_CopyMatchInfo( &(matchED->ed2), bindReq ) )
      {
        status = ZDP_NO_ENTRY;
        sendRsp = TRUE;
      }

      // Make a source match for ed1
      matchED->ed1numMatched = ZDO_CompareClusterLists(
                  matchED->ed1.numOutClusters, matchED->ed1.outClusters,
                  matchED->ed2.numInClusters, matchED->ed2.inClusters, ZDOBuildBuf );
      if ( matchED->ed1numMatched )
      {
        // Save the match list
        matchED->ed1Matched = osal_mem_alloc( (short)(matchED->ed1numMatched * sizeof ( uint16 )) );
        if ( matchED->ed1Matched )
        {
          osal_memcpy( matchED->ed1Matched, ZDOBuildBuf, (matchED->ed1numMatched * sizeof ( uint16 )) );
        }
        else
        {
          // Allocation error, stop
          status = ZDP_NO_ENTRY;
          sendRsp = TRUE;
        }
      }

      // Make a source match for ed2
      matchED->ed2numMatched = ZDO_CompareClusterLists(
                  matchED->ed2.numOutClusters, matchED->ed2.outClusters,
                  matchED->ed1.numInClusters, matchED->ed1.inClusters, ZDOBuildBuf );
      if ( matchED->ed2numMatched )
      {
        // Save the match list
        matchED->ed2Matched = osal_mem_alloc( (short)(matchED->ed2numMatched * sizeof ( uint16 )) );
        if ( matchED->ed2Matched )
        {
          osal_memcpy( matchED->ed2Matched, ZDOBuildBuf, (matchED->ed2numMatched * sizeof ( uint16 )) );
        }
        else
        {
          // Allocation error, stop
          status = ZDP_NO_ENTRY;
          sendRsp = TRUE;
        }
      }

      if ( (sendRsp == FALSE) && (matchED->ed1numMatched || matchED->ed2numMatched) )
      {
        // Do the first unbind/bind state
        ZDMatchSendState( ZDMATCH_REASON_START, ZDP_SUCCESS, 0 );
      }
      else
      {
        status = ZDP_NO_MATCH;
        sendRsp = TRUE;
      }
  }

  if ( sendRsp )
  {
    // send response to this requester
    dstAddr.addrMode = Addr16Bit;
    dstAddr.addr.shortAddr = bindReq->srcAddr;
    ZDP_EndDeviceBindRsp( bindReq->TransSeq, &dstAddr, status, bindReq->SecurityUse );

    if ( matchED->state == ZDMATCH_SENDING_BINDS )
    {
      // send response to first requester
      dstAddr.addrMode = Addr16Bit;
      dstAddr.addr.shortAddr = matchED->ed1.srcAddr;
      ZDP_EndDeviceBindRsp( matchED->ed1.TransSeq, &dstAddr, status, matchED->ed1.SecurityUse );
    }

    // Process ended - release memory used
    ZDO_RemoveMatchMemory();
  }
}

/*********************************************************************
 * @fn      ZDO_RemoveMatchMemory()
 *
 * @brief   Called to clear the memory used for the end device bind.
 *
 * @param  none
 *
 * @return  none
 */
static void ZDO_RemoveMatchMemory( void )
{
  if ( matchED != NULL )
  {
    if ( matchED->ed2Matched != NULL )
    {
      osal_mem_free( matchED->ed2Matched );
    }
    if ( matchED->ed1Matched != NULL )
    {
      osal_mem_free( matchED->ed1Matched );
    }
    if ( matchED->ed1.inClusters != NULL )
    {
      osal_mem_free( matchED->ed1.inClusters );
    }
    if ( matchED->ed1.outClusters != NULL )
    {
      osal_mem_free( matchED->ed1.outClusters );
    }
    if ( matchED->ed2.inClusters != NULL )
    {
      osal_mem_free( matchED->ed2.inClusters );
    }
    if ( matchED->ed2.outClusters != NULL )
    {
      osal_mem_free( matchED->ed2.outClusters );
    }

    osal_mem_free( matchED );
    matchED = (ZDMatchEndDeviceBind_t *)NULL;
  }
}

/*********************************************************************
 * @fn      ZDO_CopyMatchInfo()
 *
 * @brief   Called to copy memory used for the end device bind.
 *
 * @param  srcReq - source information
 * @param  dstReq - destination location
 *
 * @return  TRUE if copy was successful.
 */
static uint8 ZDO_CopyMatchInfo( ZDEndDeviceBind_t *destReq, ZDEndDeviceBind_t *srcReq )
{
  uint8 allOK = TRUE;

  // Copy bind information into the match info structure
  osal_memcpy( (uint8 *)destReq, srcReq, sizeof ( ZDEndDeviceBind_t ) );

  // Initialize the destination cluster pointers
  destReq->inClusters = NULL;
  destReq->outClusters = NULL;

  // Copy input cluster IDs
  if ( srcReq->numInClusters )
  {
    destReq->inClusters = osal_mem_alloc( (short)(srcReq->numInClusters * sizeof ( uint16 )) );
    if ( destReq->inClusters )
    {
      // Copy the clusters
      osal_memcpy( (uint8*)(destReq->inClusters), (uint8 *)(srcReq->inClusters),
                      (srcReq->numInClusters * sizeof ( uint16 )) );
    }
    else
    {
      allOK = FALSE;
    }
  }

  // Copy output cluster IDs
  if ( srcReq->numOutClusters )
  {
    destReq->outClusters = osal_mem_alloc( (short)(srcReq->numOutClusters * sizeof ( uint16 )) );
    if ( destReq->outClusters )
    {
      // Copy the clusters
      osal_memcpy( (uint8 *)(destReq->outClusters), (uint8 *)(srcReq->outClusters),
                      (srcReq->numOutClusters * sizeof ( uint16 )) );
    }
    else
    {
      allOK = FALSE;
    }
  }

  if ( allOK == FALSE )
  {
    if ( destReq->inClusters != NULL )
    {
      osal_mem_free( destReq->inClusters );
    }
    if ( destReq->outClusters != NULL )
    {
      osal_mem_free( destReq->outClusters );
    }
  }

  return ( allOK );
}

/*********************************************************************
 * @fn      ZDMatchSendState()
 *
 * @brief   State machine for the End device match message algorithm.
 *
 * @param  reason - state of algoritm
 * @param  status - initial message status
 * @param  TransSeq - next transaction sequence number
 *
 * @return  FALSE if error and we are not currently matching, TRUE
 *          if success.
 */
uint8 ZDMatchSendState( uint8 reason, uint8 status, uint8 TransSeq )
{
  uint8 *dstIEEEAddr = NULL;
  uint8 dstEP = 0xFF;
  zAddrType_t dstAddr;
  zAddrType_t destinationAddr;
  uint16 msgType;
  uint16 clusterID = 0xFFFF;
  ZDEndDeviceBind_t *ed = NULL;
  uint8 rspStatus = ZDP_SUCCESS;

  if ( matchED == NULL )
  {
    return ( FALSE );
  }

  // Check sequence number
  if ( reason == ZDMATCH_REASON_BIND_RSP || reason == ZDMATCH_REASON_UNBIND_RSP )
  {
    if ( TransSeq != matchED->transSeq )
    {
      return( FALSE ); // ignore the message
    }
  }

  // turn off timer
  APS_SetEndDeviceBindTimeout( 0, ZDO_EndDeviceBindMatchTimeoutCB );

  if ( reason == ZDMATCH_REASON_TIMEOUT )
  {
    rspStatus = ZDP_TIMEOUT;    // The process will stop
  }

  if ( reason == ZDMATCH_REASON_START || reason == ZDMATCH_REASON_BIND_RSP )
  {
    matchED->sending = ZDMATCH_SENDING_UNBIND;

    if ( reason == ZDMATCH_REASON_BIND_RSP && status != ZDP_SUCCESS )
    {
      rspStatus = status;
    }
  }
  else if ( reason == ZDMATCH_REASON_UNBIND_RSP )
  {
    if ( status == ZDP_SUCCESS )
    {
      matchED->sending = ZDMATCH_SENDING_UNBIND;
    }
    else
    {
      matchED->sending = ZDMATCH_SENDING_BIND;
    }
  }

  if ( reason != ZDMATCH_REASON_START && matchED->sending == ZDMATCH_SENDING_UNBIND )
  {
    // Move to the next cluster ID
    if ( matchED->ed1numMatched )
    {
      matchED->ed1numMatched--;
    }
    else if ( matchED->ed2numMatched )
    {
      matchED->ed2numMatched--;
    }
  }

  // What message do we send now
  if ( matchED->ed1numMatched )
  {
    ed = &(matchED->ed1);
    clusterID = matchED->ed1Matched[matchED->ed1numMatched-1];
    dstIEEEAddr = matchED->ed2.ieeeAddr;
    dstEP = matchED->ed2.endpoint;
  }
  else if ( matchED->ed2numMatched )
  {
    ed = &(matchED->ed2);
    clusterID = matchED->ed2Matched[matchED->ed2numMatched-1];
    dstIEEEAddr = matchED->ed1.ieeeAddr;
    dstEP = matchED->ed1.endpoint;
  }

  dstAddr.addrMode = Addr16Bit;

  // Send the next message
  if ( (rspStatus == ZDP_SUCCESS) && ed )
  {
    // Send unbind/bind message to source
    if ( matchED->sending == ZDMATCH_SENDING_UNBIND )
    {
      msgType = Unbind_req;
    }
    else
    {
      msgType = Bind_req;
    }

    dstAddr.addr.shortAddr = ed->srcAddr;

    // Save off the transaction sequence number
    matchED->transSeq = ZDP_TransID;

    destinationAddr.addrMode = Addr64Bit;
    osal_cpyExtAddr( destinationAddr.addr.extAddr, dstIEEEAddr );

    ZDP_BindUnbindReq( msgType, &dstAddr, ed->ieeeAddr, ed->endpoint, clusterID,
        &destinationAddr, dstEP, ed->SecurityUse );

    // Set timeout for response
    APS_SetEndDeviceBindTimeout( AIB_MaxBindingTime, ZDO_EndDeviceBindMatchTimeoutCB );
  }
  else
  {
    // Send the response messages to requesting devices
    // send response to first requester
    dstAddr.addr.shortAddr = matchED->ed1.srcAddr;
    ZDP_EndDeviceBindRsp( matchED->ed1.TransSeq, &dstAddr, rspStatus, matchED->ed1.SecurityUse );

    // send response to second requester
    if ( matchED->state == ZDMATCH_SENDING_BINDS )
    {
      dstAddr.addr.shortAddr = matchED->ed2.srcAddr;
      ZDP_EndDeviceBindRsp( matchED->ed2.TransSeq, &dstAddr, rspStatus, matchED->ed2.SecurityUse );
    }

    // Process ended - release memory used
    ZDO_RemoveMatchMemory();
  }

  return ( TRUE );
}

/*********************************************************************
 * @fn      ZDO_EndDeviceBindMatchTimeoutCB()
 *
 * @brief   End device bind timeout.
 *
 * @param  none
 *
 * @return  none
 */
static void ZDO_EndDeviceBindMatchTimeoutCB( void )
{
  ZDMatchSendState( ZDMATCH_REASON_TIMEOUT, ZDP_TIMEOUT, 0 );
}

/*********************************************************************
 * ZDO MESSAGE PARSING API FUNCTIONS
 */

/*********************************************************************
 * @fn          ZDO_ParseEndDeviceBindReq
 *
 * @brief       This function parses the End_Device_Bind_req message.
 *
 *     NOTE:  The clusters lists in bindReq are allocated in this
 *            function and must be freed by that calling function.
 *
 * @param       inMsg  - incoming message (request)
 * @param       bindReq - pointer to place to parse message to
 *
 * @return      none
 */
void ZDO_ParseEndDeviceBindReq( zdoIncomingMsg_t *inMsg, ZDEndDeviceBind_t *bindReq )
{
  uint8 *msg;

  // Parse the message
  bindReq->TransSeq = inMsg->TransSeq;
  bindReq->srcAddr = inMsg->srcAddr.addr.shortAddr;
  bindReq->SecurityUse = inMsg->SecurityUse;
  msg = inMsg->asdu;

  bindReq->localCoordinator = BUILD_UINT16( msg[0], msg[1] );
  msg += 2;

  osal_cpyExtAddr( bindReq->ieeeAddr, msg );
  msg += Z_EXTADDR_LEN;

  bindReq->endpoint = *msg++;
  bindReq->profileID = BUILD_UINT16( msg[0], msg[1] );
  msg += 2;

  bindReq->inClusters = NULL;
  bindReq->outClusters = NULL;

  if ((bindReq->numInClusters = *msg++) &&
      (bindReq->inClusters = (uint16*)osal_mem_alloc( (bindReq->numInClusters * sizeof( uint16 )))))
  {
    msg = ZDO_ConvertOTAClusters( bindReq->numInClusters, msg, bindReq->inClusters );
  }
  else
  {
    bindReq->numInClusters = 0;
  }

  if ((bindReq->numOutClusters = *msg++) &&
      (bindReq->outClusters = (uint16*)osal_mem_alloc((bindReq->numOutClusters * sizeof(uint16)))))
  {
    msg = ZDO_ConvertOTAClusters( bindReq->numOutClusters, msg, bindReq->outClusters );
  }
  else
  {
    bindReq->numOutClusters = 0;
  }
}

/*********************************************************************
 * @fn          ZDO_ParseBindUnbindReq
 *
 * @brief       This function parses the Bind_req or Unbind_req message.
 *
 * @param       inMsg  - incoming message (request)
 * @param       pReq - place to put parsed information
 *
 * @return      none
 */
void ZDO_ParseBindUnbindReq( zdoIncomingMsg_t *inMsg, ZDO_BindUnbindReq_t *pReq )
{
  uint8 *msg;

  msg = inMsg->asdu;
  osal_cpyExtAddr( pReq->srcAddress, msg );
  msg += Z_EXTADDR_LEN;
  pReq->srcEndpoint = *msg++;
  pReq->clusterID = BUILD_UINT16( msg[0], msg[1] );
  msg += 2;
  pReq->dstAddress.addrMode = *msg++;
  if ( pReq->dstAddress.addrMode == Addr64Bit )
  {
    osal_cpyExtAddr( pReq->dstAddress.addr.extAddr, msg );
    msg += Z_EXTADDR_LEN;
    pReq->dstEndpoint = *msg;
  }
  else
  {
    // copy group address
    pReq->dstAddress.addr.shortAddr = BUILD_UINT16( msg[0], msg[1] );
  }
}

/*********************************************************************
 * @fn      ZDO_ParseAddrRsp
 *
 * @brief   Turns the inMsg (incoming message) into the out parsed
 *          structure.
 *
 * @param   inMsg - incoming message
 *
 * @return  pointer to parsed structures.  This structure was
 *          allocated using osal_mem_alloc, so it must be freed
 *          by the calling function [osal_mem_free()].
 */
ZDO_NwkIEEEAddrResp_t *ZDO_ParseAddrRsp( zdoIncomingMsg_t *inMsg )
{
  ZDO_NwkIEEEAddrResp_t *rsp;
  uint8 *msg;
  byte cnt = 0;

  // Calculate the number of items in the list
  if ( inMsg->asduLen > (1 + Z_EXTADDR_LEN + 2) )
  {
    cnt = inMsg->asdu[1 + Z_EXTADDR_LEN + 2];
  }
  else
  {
    cnt = 0;
  }

  // Make buffer
  rsp = (ZDO_NwkIEEEAddrResp_t *)osal_mem_alloc( sizeof(ZDO_NwkIEEEAddrResp_t) + (cnt * sizeof ( uint16 )) );

  if ( rsp )
  {
    msg = inMsg->asdu;

    rsp->status = *msg++;
    if ( rsp->status == ZDO_SUCCESS )
    {
      osal_cpyExtAddr( rsp->extAddr, msg );
      msg += Z_EXTADDR_LEN;
      rsp->nwkAddr = BUILD_UINT16( msg[0], msg[1] );

      msg += 2;
      rsp->numAssocDevs = 0;

      // StartIndex field is only present if NumAssocDev field is non-zero.
      if ( cnt > 0 )
      {
        uint16 *pList = &(rsp->devList[0]);
        byte n = cnt;

        rsp->numAssocDevs = *msg++;
        rsp->startIndex = *msg++;

        while ( n != 0 )
        {
          *pList++ = BUILD_UINT16( msg[0], msg[1] );
          msg += sizeof( uint16 );
          n--;
        }
      }
    }
  }

  return ( rsp );
}

/*********************************************************************
 * @fn          ZDO_ParseNodeDescRsp
 *
 * @brief       This function parses the Node_Desc_rsp message.
 *
 * @param       inMsg - incoming message
 * @param       pNDRsp - place to parse the message into
 *
 * @return      none
 */
void ZDO_ParseNodeDescRsp( zdoIncomingMsg_t *inMsg, ZDO_NodeDescRsp_t *pNDRsp )
{
  uint8 *msg;

  msg = inMsg->asdu;

  pNDRsp->status = *msg++;
  pNDRsp->nwkAddr = BUILD_UINT16( msg[0], msg[1] );

  if ( pNDRsp->status == ZDP_SUCCESS )
  {
    msg += 2;
    pNDRsp->nodeDesc.LogicalType = *msg & 0x07;

    pNDRsp->nodeDesc.ComplexDescAvail = ( *msg & 0x08 ) >> 3;
    pNDRsp->nodeDesc.UserDescAvail = ( *msg & 0x10 ) >> 4;

    msg++;  // Reserved bits.
    pNDRsp->nodeDesc.FrequencyBand = (*msg >> 3) & 0x1f;
    pNDRsp->nodeDesc.APSFlags = *msg++ & 0x07;
    pNDRsp->nodeDesc.CapabilityFlags = *msg++;
    pNDRsp->nodeDesc.ManufacturerCode[0] = *msg++;
    pNDRsp->nodeDesc.ManufacturerCode[1] = *msg++;
    pNDRsp->nodeDesc.MaxBufferSize = *msg++;
    pNDRsp->nodeDesc.MaxInTransferSize[0] = *msg++;
    pNDRsp->nodeDesc.MaxInTransferSize[1] = *msg++;
    pNDRsp->nodeDesc.ServerMask = BUILD_UINT16( msg[0], msg[1] );
    msg += 2;
    pNDRsp->nodeDesc.MaxOutTransferSize[0] = *msg++;
    pNDRsp->nodeDesc.MaxOutTransferSize[1] = *msg++;
    pNDRsp->nodeDesc.DescriptorCapability = *msg;
  }
}

/*********************************************************************
 * @fn          ZDO_ParesPowerDescRsp
 *
 * @brief       This function parses the Power_Desc_rsp message.
 *
 * @param       inMsg  - incoming message
 * @param       pNPRsp - place to parse the message into
 *
 * @return      none
 */
void ZDO_ParsePowerDescRsp( zdoIncomingMsg_t *inMsg, ZDO_PowerRsp_t *pNPRsp )
{
  uint8 *msg;

  msg = inMsg->asdu;
  pNPRsp->status = *msg++;
  pNPRsp->nwkAddr = BUILD_UINT16( msg[0], msg[1] );

  if ( pNPRsp->status == ZDP_SUCCESS )
  {
    msg += 2;
    pNPRsp->pwrDesc.AvailablePowerSources = *msg >> 4;
    pNPRsp->pwrDesc.PowerMode = *msg++ & 0x0F;
    pNPRsp->pwrDesc.CurrentPowerSourceLevel = *msg >> 4;
    pNPRsp->pwrDesc.CurrentPowerSource = *msg++ & 0x0F;
  }
}

/*********************************************************************
 * @fn          ZDO_ParseSimpleDescRsp
 *
 * @brief       This function parse the Simple_Desc_rsp message.
 *
 *   NOTE: The pAppInClusterList and pAppOutClusterList fields
 *         in the SimpleDescriptionFormat_t structure are allocated
 *         and the calling function needs to free [osal_msg_free()]
 *         these buffers.
 *
 * @param       inMsg  - incoming message
 * @param       pSimpleDescRsp - place to parse the message into
 *
 * @return      none
 */
void ZDO_ParseSimpleDescRsp( zdoIncomingMsg_t *inMsg, ZDO_SimpleDescRsp_t *pSimpleDescRsp )
{
  uint8 *msg;

  msg = inMsg->asdu;
  pSimpleDescRsp->status = *msg++;
  pSimpleDescRsp->nwkAddr = BUILD_UINT16( msg[0], msg[1] );
  msg += sizeof ( uint16 );
  msg++; // Skip past the length field.

  if ( pSimpleDescRsp->status == ZDP_SUCCESS )
  {
    ZDO_ParseSimpleDescBuf( msg, &(pSimpleDescRsp->simpleDesc) );
  }
}

/*********************************************************************
 * @fn          ZDO_ParseEPListRsp
 *
 * @brief       This parse the Active_EP_rsp or Match_Desc_rsp message.
 *
 * @param       inMsg  - incoming message
 *
 * @return      none
 */
ZDO_ActiveEndpointRsp_t *ZDO_ParseEPListRsp( zdoIncomingMsg_t *inMsg )
{
  ZDO_ActiveEndpointRsp_t *pRsp;
  uint8 *msg;
  uint8 Status;
  uint8 cnt;

  msg = inMsg->asdu;
  Status = *msg++;
  cnt = msg[2];

  pRsp = (ZDO_ActiveEndpointRsp_t *)osal_mem_alloc( sizeof(  ZDO_ActiveEndpointRsp_t ) + cnt );
  if ( pRsp )
  {
    pRsp->status = Status;
    pRsp->nwkAddr = BUILD_UINT16( msg[0], msg[1] );
    msg += sizeof( uint16 );
    pRsp->cnt = cnt;
    msg++; // pass cnt
    osal_memcpy( pRsp->epList, msg, cnt );
  }

  return ( pRsp );
}

/*********************************************************************
 * @fn          ZDO_ParseServerDiscRsp
 *
 * @brief       Parse the Server_Discovery_rsp message.
 *
 * @param       inMsg - incoming message.
 * @param       pRsp - place to put the parsed information.
 *
 * @return      none
 */
void ZDO_ParseServerDiscRsp( zdoIncomingMsg_t *inMsg, ZDO_ServerDiscRsp_t *pRsp )
{
  pRsp->status = inMsg->asdu[0];
  pRsp->serverMask = BUILD_UINT16( inMsg->asdu[1], inMsg->asdu[2] );
}

/*********************************************************************
 * @fn          ZDO_ParseMgmtLqiRsp
 *
 * @brief       This function parses the incoming Management
 *              LQI response
 *
 * @param       inMsg - incoming message
 *
 * @return      a pointer to parsed response structure (NULL if not allocated).
 *          This structure was allocated using osal_mem_alloc, so it must be freed
 *          by the calling function [osal_mem_free()].
 */
ZDO_MgmtLqiRsp_t *ZDO_ParseMgmtLqiRsp( zdoIncomingMsg_t *inMsg )
{
  ZDO_MgmtLqiRsp_t *pRsp;
  uint8 status;
  uint8 startIndex = 0;
  uint8 neighborLqiCount = 0;
  uint8 neighborLqiEntries = 0;
  uint8 *msg;

  msg = inMsg->asdu;

  status = *msg++;
  if ( status == ZSuccess )
  {
    neighborLqiEntries = *msg++;
    startIndex = *msg++;
    neighborLqiCount = *msg++;
  }

  // Allocate a buffer big enough to handle the list.
  pRsp = (ZDO_MgmtLqiRsp_t *)osal_mem_alloc(
            sizeof( ZDO_MgmtLqiRsp_t ) + (neighborLqiCount * sizeof( neighborLqiItem_t )) );
  if ( pRsp )
  {
    uint8 x;
    neighborLqiItem_t *pList = pRsp->list;
    pRsp->status = status;
    pRsp->neighborLqiEntries = neighborLqiEntries;
    pRsp->startIndex = startIndex;
    pRsp->neighborLqiCount = neighborLqiCount;

    for ( x = 0; x < neighborLqiCount; x++ )
    {
      osal_cpyExtAddr(pList->extPANId, msg);   //Copy extended PAN ID
      msg += Z_EXTADDR_LEN;

      msg += Z_EXTADDR_LEN;  // Throwing away IEEE.
      pList->nwkAddr = BUILD_UINT16( msg[0], msg[1] );
      msg += 2 + 1 + 1 + 1;      // Skip DeviceType, RxOnIdle, Rlationship, PermitJoining and Depth
      pList->rxLqi = *msg++;
      pList->txQuality = 0;  // This is not specified OTA by ZigBee 1.1.
      pList++;
    }
  }

  return ( pRsp );
}

/*********************************************************************
 * @fn          ZDO_ParseMgmNwkDiscRsp
 *
 * @brief       This function parses the incoming Management
 *              Network Discover response.
 *
 * @param       inMsg - incoming message
 *
 * @return      pointer to parsed response.  This structure was
 *          allocated using osal_mem_alloc, so it must be freed
 *          by the calling function [osal_mem_free()].
 */
ZDO_MgmNwkDiscRsp_t *ZDO_ParseMgmNwkDiscRsp( zdoIncomingMsg_t *inMsg )
{
  ZDO_MgmNwkDiscRsp_t *pRsp;
  uint8 status;
  uint8 networkCount = 0;
  uint8 startIndex = 0;
  uint8 networkListCount = 0;
  uint8 *msg;

  msg = inMsg->asdu;
  status = *msg++;

  if ( status == ZSuccess )
  {
    networkCount = *msg++;
    startIndex = *msg++;
    networkListCount = *msg++;
  }

  // Allocate a buffer big enough to handle the list.
  pRsp = (ZDO_MgmNwkDiscRsp_t *)osal_mem_alloc( sizeof( ZDO_MgmNwkDiscRsp_t )
                                  + (networkListCount * sizeof( mgmtNwkDiscItem_t )) );
  if ( pRsp )
  {
    uint8 x;
    mgmtNwkDiscItem_t *pList;

    pRsp->status = status;
    pRsp->networkCount = networkCount;
    pRsp->startIndex = startIndex;
    pRsp->networkListCount = networkListCount;
    pList = pRsp->list;

    for ( x = 0; x < networkListCount; x++ )
    {
      osal_cpyExtAddr(pList->extendedPANID, msg);   //Copy extended PAN ID
      pList->PANId = BUILD_UINT16( msg[0], msg[1] );
      msg += Z_EXTADDR_LEN;

      pList->logicalChannel = *msg++;
      pList->stackProfile = (*msg) & 0x0F;
      pList->version = (*msg++ >> 4) & 0x0F;
      pList->beaconOrder = (*msg) & 0x0F;
      pList->superFrameOrder = (*msg++ >> 4) & 0x0F;
      pList->permitJoining = *msg++;
      pList++;
    }
  }

  return ( pRsp );
}

/*********************************************************************
 * @fn          ZDO_ParseMgmtRtgRsp
 *
 * @brief       This function parses the incoming Management
 *              Routing response.
 *
 * @param       inMsg - incoming message
 *
 * @return      a pointer to parsed response structure (NULL if not allocated).
 *          This structure was allocated using osal_mem_alloc, so it must be freed
 *          by the calling function [osal_mem_free()].
 */
ZDO_MgmtRtgRsp_t *ZDO_ParseMgmtRtgRsp( zdoIncomingMsg_t *inMsg )
{
  ZDO_MgmtRtgRsp_t *pRsp;
  uint8 status;
  uint8 rtgCount = 0;
  uint8 startIndex = 0;
  uint8 rtgListCount = 0;
  uint8 *msg;

  msg = inMsg->asdu;

  status = *msg++;
  if ( status == ZSuccess )
  {
    rtgCount = *msg++;
    startIndex = *msg++;
    rtgListCount = *msg++;
  }

  // Allocate a buffer big enough to handle the list
  pRsp = (ZDO_MgmtRtgRsp_t *)osal_mem_alloc(
          sizeof( ZDO_MgmtRtgRsp_t ) + (rtgListCount * sizeof( rtgItem_t )) );
  if ( pRsp )
  {
    uint8 x;
    rtgItem_t *pList = pRsp->list;
    pRsp->status = status;
    pRsp->rtgCount = rtgCount;
    pRsp->startIndex = startIndex;
    pRsp->rtgListCount = rtgListCount;

    for ( x = 0; x < rtgListCount; x++ )
    {
      pList->dstAddress = BUILD_UINT16( msg[0], msg[1] );
      msg += 2;
      pList->status = *msg++;
      pList->nextHopAddress = BUILD_UINT16( msg[0], msg[1] );
      msg += 2;
      pList++;
    }
  }

  return ( pRsp );
}

/*********************************************************************
 * @fn          ZDO_ParseMgmtBindRsp
 *
 * @brief       This function parses the incoming Management
 *              Binding response.
 *
 * @param       inMsg - pointer to message to parse
 *
 * @return      a pointer to parsed response structure (NULL if not allocated).
 *          This structure was allocated using osal_mem_alloc, so it must be freed
 *          by the calling function [osal_mem_free()].
 */
ZDO_MgmtBindRsp_t *ZDO_ParseMgmtBindRsp( zdoIncomingMsg_t *inMsg )
{
  ZDO_MgmtBindRsp_t *pRsp;
  uint8 status;
  uint8 bindingCount = 0;
  uint8 startIndex = 0;
  uint8 bindingListCount = 0;
  uint8 *msg;

  msg = inMsg->asdu;

  status = *msg++;
  if ( status == ZSuccess )
  {
    bindingCount = *msg++;
    startIndex = *msg++;
    bindingListCount = *msg++;
  }

  // Allocate a buffer big enough to handle the list
  pRsp = (ZDO_MgmtBindRsp_t *)osal_mem_alloc(
          (sizeof ( ZDO_MgmtBindRsp_t ) + (bindingListCount * sizeof( apsBindingItem_t ))) );
  if ( pRsp )
  {
    uint8 x;
    apsBindingItem_t *pList = pRsp->list;
    pRsp->status = status;
    pRsp->bindingCount = bindingCount;
    pRsp->startIndex = startIndex;
    pRsp->bindingListCount = bindingListCount;

    for ( x = 0; x < bindingListCount; x++ )
    {
      osal_cpyExtAddr( pList->srcAddr, msg );
      msg += Z_EXTADDR_LEN;
      pList->srcEP = *msg++;

      // Get the Cluster ID

      pList->clusterID = BUILD_UINT16( msg[0], msg[1] );
      msg += 2;
      pList->dstAddr.addrMode = *msg++;
      if ( pList->dstAddr.addrMode == Addr64Bit )
      {
        osal_cpyExtAddr( pList->dstAddr.addr.extAddr, msg );
        msg += Z_EXTADDR_LEN;
        pList->dstEP = *msg++;
      }
      else
      {
        pList->dstAddr.addr.shortAddr = BUILD_UINT16( msg[0], msg[1] );
        msg += 2;
      }

      pList++;
    }
  }

  return ( pRsp );
}

/*********************************************************************
 * @fn          ZDO_ParseUserDescRsp
 *
 * @brief       This function parses the incoming User
 *              Descriptor Response.
 *
 * @param       inMsg - incoming response message
 *
 * @return      a pointer to parsed response structure (NULL if not allocated).
 *          This structure was allocated using osal_mem_alloc, so it must be freed
 *          by the calling function [osal_mem_free()].
 */
ZDO_UserDescRsp_t *ZDO_ParseUserDescRsp( zdoIncomingMsg_t *inMsg )
{
  ZDO_UserDescRsp_t *pRsp;
  uint8 *msg;
  uint8 descLen = 0;

  msg = inMsg->asdu;

  if ( msg[0] == ZSuccess )
  {
    descLen = msg[3];
  }

  pRsp = (ZDO_UserDescRsp_t *)osal_mem_alloc( sizeof ( ZDO_UserDescRsp_t ) + descLen );
  if ( pRsp )
  {
    pRsp->status = msg[0];
    pRsp->nwkAddr = BUILD_UINT16( msg[1], msg[2] );
    pRsp->length = descLen;
    if ( descLen )
    {
      osal_memcpy( pRsp->desc, &msg[4], descLen );
    }
  }

  return ( pRsp );
}

/*********************************************************************
 * @fn          ZDO_ParseSimpleDescBuf
 *
 * @brief       Parse a byte sequence representation of a Simple Descriptor.
 *
 * @param       buf  - pointer to a byte array representing a Simple Desc.
 * @param       desc - SimpleDescriptionFormat_t *
 *
 *              This routine allocates storage for the cluster IDs because
 *              they are 16-bit and need to be aligned to be properly processed.
 *              This routine returns non-zero if an allocation fails.
 *
 *              NOTE: This means that the caller or user of the input structure
 *                    is responsible for freeing the memory
 *
 * @return      0: success
 *              1: failure due to malloc failure.
 */
uint8 ZDO_ParseSimpleDescBuf( uint8 *buf, SimpleDescriptionFormat_t *desc )
{
  uint8 num, i;

  desc->EndPoint = *buf++;
  desc->AppProfId = BUILD_UINT16( buf[0], buf[1] );
  buf += 2;
  desc->AppDeviceId = BUILD_UINT16( buf[0], buf[1] );
  buf += 2;
  desc->AppDevVer = *buf >> 4;

  desc->Reserved = 0;
  buf++;

  // move in input cluster list (if any). allocate aligned memory.
  num = desc->AppNumInClusters = *buf++;
  if ( num )
  {
    if (!(desc->pAppInClusterList = (uint16 *)osal_mem_alloc(num*sizeof(uint16))))
    {
      // malloc failed. we're done.
      return 1;
    }
    for (i=0; i<num; ++i)
    {
      desc->pAppInClusterList[i] = BUILD_UINT16( buf[0], buf[1] );
      buf += 2;
    }
  }

  // move in output cluster list (if any). allocate aligned memory.
  num = desc->AppNumOutClusters = *buf++;
  if (num)
  {
    if (!(desc->pAppOutClusterList = (uint16 *)osal_mem_alloc(num*sizeof(uint16))))
    {
      // malloc failed. free input cluster list memory if there is any
      if ( desc->pAppInClusterList != NULL )
      {
        osal_mem_free(desc->pAppInClusterList);

        desc->pAppInClusterList = NULL;
      }
      return 1;
    }
    for (i=0; i<num; ++i)
    {
      desc->pAppOutClusterList[i] = BUILD_UINT16( buf[0], buf[1] );
      buf += 2;
    }
  }
  return 0;
}

/*********************************************************************
 * @fn          ZDO_ParseDeviceAnnce
 *
 * @brief       Parse a Device Announce message.
 *
 * @param       inMsg - Incoming message
 * @param       pAnnce - place to put the parsed information
 *
 * @return      none
 */
void ZDO_ParseDeviceAnnce( zdoIncomingMsg_t *inMsg, ZDO_DeviceAnnce_t *pAnnce )
{
  uint8 *msg;

  // Parse incoming message
  msg = inMsg->asdu;
  pAnnce->nwkAddr = BUILD_UINT16( msg[0], msg[1] );
  msg += 2;
  osal_cpyExtAddr( pAnnce->extAddr, msg );
  msg += Z_EXTADDR_LEN;
  pAnnce->capabilities = *msg;
}

/*********************************************************************
 * @fn          ZDO_ParseMgmtNwkUpdateNotify
 *
 * @brief       This function handles parsing of the incoming Management
 *              Network Update notify.
 *
 * @param       inMsg - incoming message (request)
 *
 * @return      a pointer to parsed response structure (NULL if not allocated).
 *          This structure was allocated using osal_mem_alloc, so it must be freed
 *          by the calling function [osal_mem_free()].
 */
ZDO_MgmtNwkUpdateNotify_t *ZDO_ParseMgmtNwkUpdateNotify( zdoIncomingMsg_t *inMsg )
{
  uint8 status;
  uint32 scannedChannels = 0;
  uint16 totalTransmissions = 0;
  uint16 transmissionFailures = 0;
  uint8 listCount = 0;
  uint8 *msg = inMsg->asdu;
  ZDO_MgmtNwkUpdateNotify_t *pRsp;

  status = *msg++;
  if ( status == ZSuccess )
  {
    scannedChannels = osal_build_uint32( msg, 4 );
    msg += 4;
    totalTransmissions = BUILD_UINT16( msg[0], msg[1] );
    msg += 2;
    transmissionFailures = BUILD_UINT16( msg[0], msg[1] );
    msg += 2;
    listCount = *msg++;
  }

  pRsp = (ZDO_MgmtNwkUpdateNotify_t *)osal_mem_alloc( sizeof ( ZDO_MgmtNwkUpdateNotify_t ) + listCount );

  if ( pRsp )
  {
    pRsp->status = status;
    pRsp->scannedChannels = scannedChannels;
    pRsp->totalTransmissions = totalTransmissions;
    pRsp->transmissionFailures = transmissionFailures;
    pRsp->listCount = listCount;

    // Allocate a buffer big enough to handle the list.
    if ( listCount > 0 )
    {
      osal_memcpy( pRsp->energyValues, msg, listCount );
    }
  }

  return ( pRsp );
}

/*********************************************************************
 * @fn          ZDO_ParseMgmtNwkUpdateReq
 *
 * @brief       This function handles parsing the incoming Management
 *              Network Update request and starts the request (if needed).
 *
 * @param       inMsg - incoming message (request)
 * @param       pReq - pointer to place to parse message to
 *
 * @return      none
 */
void ZDO_ParseMgmtNwkUpdateReq( zdoIncomingMsg_t *inMsg, ZDO_MgmtNwkUpdateReq_t *pReq )
{
  uint8 *msg = inMsg->asdu;

  pReq->channelMask = osal_build_uint32( msg, 4 );
  msg += 4;
  pReq->scanDuration = *msg++;

  if ( pReq->scanDuration <= 0x05 )
  {
    // Request is to scan over channelMask
    pReq->scanCount = *msg;
  }
  else if ( ( pReq->scanDuration == 0xFE ) || ( pReq->scanDuration == 0xFF ) )
  {
    // Request is to change Channel (0xFE) or apsChannelMask and NwkManagerAddr (0xFF)
    pReq->nwkUpdateId = *msg++;

    if ( pReq->scanDuration == 0xFF )
    {
      pReq->nwkManagerAddr = BUILD_UINT16( msg[0], msg[1] );
    }
  }
}

/*********************************************************************
*********************************************************************/
