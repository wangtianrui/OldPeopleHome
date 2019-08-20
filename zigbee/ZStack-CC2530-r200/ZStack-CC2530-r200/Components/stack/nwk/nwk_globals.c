/**************************************************************************************************
  Filename:       nwk_globals.c
  Revised:        $Date: 2011-05-09 10:21:04 -0700 (Mon, 09 May 2011) $
  Revision:       $Revision: 25918 $

  Description:    User definable Network Parameters.


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
#include "AddrMgr.h"
#include "AssocList.h"
#include "BindingTable.h"
#include "nwk_util.h"
#include "nwk_globals.h"
#include "APS.h"
#include "ssp.h"
#include "rtg.h"
#include "ZDConfig.h"
#include "ZGlobals.h"

#if defined ( LCD_SUPPORTED )
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

// Maximums for the data buffer queue
#define NWK_MAX_DATABUFS_WAITING    8     // Waiting to be sent to MAC
#define NWK_MAX_DATABUFS_SCHEDULED  5     // Timed messages to be sent
#define NWK_MAX_DATABUFS_CONFIRMED  5     // Held after MAC confirms
#define NWK_MAX_DATABUFS_TOTAL      12    // Total number of buffers

// 1-255 (0 -> 256) X RTG_TIMER_INTERVAL
// A known shortcoming is that when a message is enqueued as "hold" for a
// sleeping device, the timer tick may have counted down to 1, so that msg
// will not be held as long as expected. If NWK_INDIRECT_MSG_TIMEOUT is set to 1
// the hold time will vary randomly from 0 - CNT_RTG_TIMER ticks.
// So the hold time will vary within this interval:
// { (NWK_INDIRECT_MSG_TIMEOUT-1)*CNT_RTG_TIMER,
//                                    NWK_INDIRECT_MSG_TIMEOUT*CNT_RTG_TIMER }
#define NWK_INDIRECT_CNT_RTG_TMR    1
// To hold msg for sleeping end devices for 30 secs:
// #define CNT_RTG_TIMER            1
// #define NWK_INDIRECT_MSG_TIMEOUT 30
// To hold msg for sleeping end devices for 30 mins:
// #define CNT_RTG_TIMER            60
// #define NWK_INDIRECT_MSG_TIMEOUT 30
// To hold msg for sleeping end devices for 30 days:
// #define CNT_RTG_TIMER            60
// #define NWK_INDIRECT_MSG_TIMEOUT (30 * 24 * 60)
// Maximum msgs to hold per associated device.
#define NWK_INDIRECT_MSG_MAX_PER    3
// Maximum total msgs to hold for all associated devices.
#define NWK_INDIRECT_MSG_MAX_ALL    \
                            (NWK_MAX_DATABUFS_TOTAL - NWK_INDIRECT_MSG_MAX_PER)

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * NWK GLOBAL VARIABLES
 */

// Variables for MAX list size
CONST uint16 gNWK_MAX_DEVICE_LIST = NWK_MAX_DEVICES;

// Variables for MAX Sleeping End Devices
CONST uint8 gNWK_MAX_SLEEPING_END_DEVICES = NWK_MAX_DEVICES - NWK_MAX_ROUTERS;

// Variables for MAX data buffer levels
CONST uint8 gNWK_MAX_DATABUFS_WAITING = NWK_MAX_DATABUFS_WAITING;
CONST uint8 gNWK_MAX_DATABUFS_SCHEDULED = NWK_MAX_DATABUFS_SCHEDULED;
CONST uint8 gNWK_MAX_DATABUFS_CONFIRMED = NWK_MAX_DATABUFS_CONFIRMED;
CONST uint8 gNWK_MAX_DATABUFS_TOTAL = NWK_MAX_DATABUFS_TOTAL;

CONST uint8 gNWK_INDIRECT_CNT_RTG_TMR = NWK_INDIRECT_CNT_RTG_TMR;
CONST uint8 gNWK_INDIRECT_MSG_MAX_PER = NWK_INDIRECT_MSG_MAX_PER;
CONST uint8 gNWK_INDIRECT_MSG_MAX_ALL = NWK_INDIRECT_MSG_MAX_ALL;

// change this if using a different stack profile...
// Cskip array
uint16 *Cskip;

#if ( STACK_PROFILE_ID == ZIGBEEPRO_PROFILE )
  uint8 CskipRtrs[1] = {0};
  uint8 CskipChldrn[1] = {0};
#elif ( STACK_PROFILE_ID == HOME_CONTROLS )
  uint8 CskipRtrs[MAX_NODE_DEPTH+1] = {6,6,6,6,6,0};
  uint8 CskipChldrn[MAX_NODE_DEPTH+1] = {20,20,20,20,20,0};
#elif ( STACK_PROFILE_ID == GENERIC_STAR )
  uint8 CskipRtrs[MAX_NODE_DEPTH+1] = {5,5,5,5,5,0};
  uint8 CskipChldrn[MAX_NODE_DEPTH+1] = {5,5,5,5,5,0};
#elif ( STACK_PROFILE_ID == NETWORK_SPECIFIC )
  uint8 CskipRtrs[MAX_NODE_DEPTH+1] = {5,5,5,5,5,0};
  uint8 CskipChldrn[MAX_NODE_DEPTH+1] = {5,5,5,5,5,0};
#endif // STACK_PROFILE_ID

// Minimum lqi value that is required for association
uint8 gMIN_TREE_LINK_COST = MIN_LQI_COST_3;

// Statically defined Associated Device List
associated_devices_t AssociatedDevList[NWK_MAX_DEVICES];

#if defined ( ZIGBEE_STOCHASTIC_ADDRESSING )
  CONST uint16 gNWK_MIN_ROUTER_CHILDREN = NWK_MIN_ROUTER_CHILDREN;
  CONST uint16 gNWK_MIN_ENDDEVICE_CHILDREN = NWK_MIN_ENDDEVICE_CHILDREN;
#else
  CONST uint16 gNWK_MIN_ROUTER_CHILDREN = 0;
  CONST uint16 gNWK_MIN_ENDDEVICE_CHILDREN = 0;
#endif

CONST uint16 gMAX_RTG_ENTRIES = MAX_RTG_ENTRIES;
CONST uint16 gMAX_RTG_SRC_ENTRIES = MAX_RTG_SRC_ENTRIES;
CONST uint8 gMAX_RREQ_ENTRIES = MAX_RREQ_ENTRIES;

CONST uint8 gMAX_NEIGHBOR_ENTRIES = MAX_NEIGHBOR_ENTRIES;

 // Table of neighboring nodes (not including child nodes)
neighborEntry_t neighborTable[MAX_NEIGHBOR_ENTRIES];

CONST uint8 gMAX_SOURCE_ROUTE = MAX_SOURCE_ROUTE;

CONST uint8 gMAX_BROADCAST_QUEUED = MAX_BROADCAST_QUEUED;

CONST uint8 gLINK_DOWN_TRIGGER = LINK_DOWN_TRIGGER;

CONST uint8 gMAX_PASSIVE_ACK_CNT = MAX_PASSIVE_ACK_CNT;

// Routing table
rtgEntry_t rtgTable[MAX_RTG_ENTRIES];

#if defined ( ZIGBEE_SOURCE_ROUTING )
  rtgSrcEntry_t rtgSrcTable[MAX_RTG_SRC_ENTRIES];
  uint16 rtgSrcRelayList[MAX_SOURCE_ROUTE];
#endif

// Table of current RREQ packets in the network
rtDiscEntry_t rtDiscTable[MAX_RREQ_ENTRIES];

// Table of data broadcast packets currently in circulation.
bcastEntry_t bcastTable[MAX_BCAST];

// These 2 arrays are to be used as an array of struct { uint8, uint32 }.
uint8 bcastHoldHandle[MAX_BCAST];
uint32 bcastHoldAckMask[MAX_BCAST];

CONST uint8 gMAX_BCAST = MAX_BCAST;

// For tree addressing, this switch allows the allocation of a
// router address to an end device when end device address are
// all used up.  If this option is enabled, address space
// could be limited.
CONST uint8 gNWK_TREE_ALLOCATE_ROUTERADDR_FOR_ENDDEVICE = FALSE;

#if defined ( ZIGBEE_STOCHASTIC_ADDRESSING )
// number of link status periods after the last received address conflict report
// (network status command)
CONST uint8 gNWK_CONFLICTED_ADDR_EXPIRY_TIME = NWK_CONFLICTED_ADDR_EXPIRY_TIME;
#endif

#if defined ( ZIGBEE_FREQ_AGILITY )
CONST uint8 gNWK_FREQ_AGILITY_ALL_MAC_ERRS = NWK_FREQ_AGILITY_ALL_MAC_ERRS;
#endif

// The time limited to one MTO RReq (Concentrator Announce) in milliseconds.
CONST uint16 gMTO_RREQ_LIMIT_TIME = MTO_RREQ_LIMIT_TIME;

// The number of seconds a MTO routing entry will last.
CONST uint8 gMTO_ROUTE_EXPIRY_TIME = MTO_ROUTE_EXPIRY_TIME;

// Route Discovery Request Default Radius
CONST uint8 gDEFAULT_ROUTE_REQUEST_RADIUS = DEFAULT_ROUTE_REQUEST_RADIUS;

// Network message radius
CONST uint8 gDEF_NWK_RADIUS = DEF_NWK_RADIUS;

#if ( ZSTACK_ROUTER_BUILD )
CONST uint16 gLINK_STATUS_JITTER_MASK = LINK_STATUS_JITTER_MASK;
#endif

/*********************************************************************
 * APS GLOBAL VARIABLES
 */

// The Maximum number of binding records
// This number is defined in f8wConfig.cfg - change it there.
CONST uint16 gNWK_MAX_BINDING_ENTRIES = NWK_MAX_BINDING_ENTRIES;

#if defined ( REFLECTOR )
  // The Maximum number of cluster IDs in a binding record
  // This number is defined in f8wConfig.cfg - change it there.
  CONST uint8 gMAX_BINDING_CLUSTER_IDS = MAX_BINDING_CLUSTER_IDS;

  CONST uint16 gBIND_REC_SIZE = sizeof( BindingEntry_t );

  // Binding Table
  BindingEntry_t BindingTable[NWK_MAX_BINDING_ENTRIES];
#endif

// Maximum number allowed in the groups table.
CONST uint8 gAPS_MAX_GROUPS = APS_MAX_GROUPS;

// APS End Device Broadcast Table
#if ( ZG_BUILD_ENDDEVICE_TYPE )
  apsEndDeviceBroadcast_t apsEndDeviceBroadcastTable[APS_MAX_ENDDEVICE_BROADCAST_ENTRIES];
  uint8 gAPS_MAX_ENDDEVICE_BROADCAST_ENTRIES = APS_MAX_ENDDEVICE_BROADCAST_ENTRIES;
#endif

/*********************************************************************
 * SECURITY GLOBAL VARIABLES
 */

// This is the default pre-configured key,
// change this to make a unique key
// SEC_KEY_LEN is defined in ssp.h.

#if defined ( DEFAULT_KEY )
CONST uint8 defaultKey[SEC_KEY_LEN] = DEFAULT_KEY;
#else
CONST uint8 defaultKey[SEC_KEY_LEN] =
{
#if defined ( APP_TP ) || defined ( APP_TP2 )
  // Key for ZigBee Conformance Testing
  0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb,
  0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
#else
  // Key for In-House Testing
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
#endif
};
#endif

// This is the default pre-configured Trust Center Link key,
// change this to make a unique key, SEC_KEY_LEN is defined in ssp.h.
CONST uint8 defaultTCLinkKey[SEC_KEY_LEN] = DEFAULT_TC_LINK_KEY;

/*********************************************************************
 * GLOBAL VARIABLES - Statistics
 */

#if defined ( PACKET_FILTER_STATS )
  uint32 apsInvalidPackets = 0;
  uint32 apsSecurityFailures = 0;
  uint32 nwkInvalidPackets = 0;
  uint32 nwkSecurityFailures = 0;
#endif

/*********************************************************************
 * STATUS STRINGS
 */
#if defined ( LCD_SUPPORTED )
  const char PingStr[]         = "Ping Rcvd from";
  const char AssocCnfStr[]     = "Assoc Cnf";
  const char SuccessStr[]      = "Success";
  const char EndDeviceStr[]    = "EndDevice:";
  const char ParentStr[]       = "Parent:";
  const char ZigbeeCoordStr[]  = "ZigBee Coord";
  const char NetworkIDStr[]    = "Network ID:";
  const char RouterStr[]       = "Router:";
  const char OrphanRspStr[]    = "Orphan Response";
  const char SentStr[]         = "Sent";
  const char FailedStr[]       = "Failed";
  const char AssocRspFailStr[] = "Assoc Rsp fail";
  const char AssocIndStr[]     = "Assoc Ind";
  const char AssocCnfFailStr[] = "Assoc Cnf fail";
  const char EnergyLevelStr[]  = "Energy Level";
  const char ScanFailedStr[]   = "Scan Failed";
#endif

/*********************************************************************
 * @fn       nwk_globals_init()
 *
 * @brief
 *
 *   Initialize nwk layer globals.  These are the system defaults and
 *   should be changed by the user here.  The default definitions are
 *   defined in nwk.h or NLMEDE.h.
 *
 * @param   none
 *
 * @return  none
 */
void nwk_globals_init( void )
{
  AddrMgrInit( NWK_MAX_ADDRESSES );

#if !defined ( ZIGBEE_STOCHASTIC_ADDRESSING )
  if ( ZSTACK_ROUTER_BUILD )
  {
    // Initialize the Cskip Table
    Cskip = osal_mem_alloc(sizeof(uint16) *(MAX_NODE_DEPTH+1));
    RTG_FillCSkipTable(CskipChldrn, CskipRtrs, MAX_NODE_DEPTH, Cskip);
  }
#endif

  // To compile out the Link Status Feature, set NWK_LINK_STATUS_PERIOD
  // to 0 (compiler flag).
  if ( ZSTACK_ROUTER_BUILD && NWK_LINK_STATUS_PERIOD )
  {
    NLME_InitLinkStatus();
  }

#if defined ( ZIGBEE_FREQ_AGILITY )
  NwkFreqAgilityInit();
#endif
}

/*********************************************************************
 * @fn       NIB_init()
 *
 * @brief
 *
 *   Initialize attribute values in NIB
 *
 * @param   none
 *
 * @return  none
 */
void NIB_init()
{
  _NIB.SequenceNum = LO_UINT16(osal_rand());

  _NIB.nwkProtocolVersion = ZB_PROT_VERS;
  _NIB.MaxDepth = MAX_NODE_DEPTH;

#if ( NWK_MODE == NWK_MODE_MESH )
  _NIB.beaconOrder = BEACON_ORDER_NO_BEACONS;
  _NIB.superFrameOrder = BEACON_ORDER_NO_BEACONS;
#endif

  // BROADCAST SETTINGS:
  // *******************
  //   Broadcast Delivery Time
  //     - set to multiples of 100ms
  //     - should be 500ms more than the retry time
  //       -  "retry time" = PassiveAckTimeout * (MaxBroadcastRetries + 1)
  //   Passive Ack Timeout
  //     - set to multiples of 100ms
  _NIB.BroadcastDeliveryTime = zgBcastDeliveryTime;
  _NIB.PassiveAckTimeout     = zgPassiveAckTimeout;
  _NIB.MaxBroadcastRetries   = zgMaxBcastRetires;

  _NIB.ReportConstantCost = 0;
  _NIB.RouteDiscRetries = 0;
  _NIB.SecureAllFrames = USE_NWK_SECURITY;
  _NIB.nwkAllFresh = NWK_ALL_FRESH;

  if ( ZG_SECURE_ENABLED )
  {
    _NIB.SecurityLevel = SECURITY_LEVEL;
  }
  else
  {
    _NIB.SecurityLevel = 0;
  }

#if defined ( ZIGBEEPRO )
  _NIB.SymLink = FALSE;
#else
  _NIB.SymLink = TRUE;
#endif

  _NIB.CapabilityFlags = ZDO_Config_Node_Descriptor.CapabilityFlags;

  _NIB.TransactionPersistenceTime = zgIndirectMsgTimeout;

  _NIB.RouteDiscoveryTime = zgRouteDiscoveryTime;
  _NIB.RouteExpiryTime = zgRouteExpiryTime;

  _NIB.nwkDevAddress = INVALID_NODE_ADDR;
  _NIB.nwkLogicalChannel = 0;
  _NIB.nwkCoordAddress = INVALID_NODE_ADDR;
  osal_memset( _NIB.nwkCoordExtAddress, 0, Z_EXTADDR_LEN );
  _NIB.nwkPanId = INVALID_NODE_ADDR;

  osal_cpyExtAddr( _NIB.extendedPANID, zgExtendedPANID );

  _NIB.nwkKeyLoaded = FALSE;

#if defined ( ZIGBEE_STOCHASTIC_ADDRESSING )
  _NIB.nwkAddrAlloc  = NWK_ADDRESSING_STOCHASTIC;
  _NIB.nwkUniqueAddr = FALSE;
#else
  _NIB.nwkAddrAlloc  = NWK_ADDRESSING_DISTRIBUTED;
  _NIB.nwkUniqueAddr = TRUE;
#endif

  _NIB.nwkLinkStatusPeriod = NWK_LINK_STATUS_PERIOD;
  _NIB.nwkRouterAgeLimit = NWK_ROUTE_AGE_LIMIT;

  //MTO and source routing
  _NIB.nwkConcentratorDiscoveryTime = zgConcentratorDiscoveryTime;
  _NIB.nwkIsConcentrator = zgConcentratorEnable;
  _NIB.nwkConcentratorRadius = zgConcentratorRadius;

#if defined ( ZIGBEE_MULTICAST )
  _NIB.nwkUseMultiCast = TRUE;
#else
  _NIB.nwkUseMultiCast = FALSE;
#endif

#if defined ( NV_RESTORE )
  if ( osal_nv_read( ZCD_NV_NWKMGR_ADDR, 0, sizeof( _NIB.nwkManagerAddr ),
                     &_NIB.nwkManagerAddr ) != SUCCESS )
#endif
  {
    _NIB.nwkManagerAddr = 0x0000;
  }

  _NIB.nwkUpdateId = 0;
  _NIB.nwkTotalTransmissions = 0;

  if ( ZSTACK_ROUTER_BUILD )
  {
#if defined ( ZIGBEE_STOCHASTIC_ADDRESSING )
    NLME_InitStochasticAddressing();
#else
    NLME_InitTreeAddressing();
#endif
  }
}

/*********************************************************************
 * @fn       nwk_Status()
 *
 * @brief
 *
 *   Status report.
 *
 * @param   statusCode
 * @param   statusValue
 *
 * @return  none
 */
void nwk_Status( uint16 statusCode, uint16 statusValue )
{
#if defined ( LCD_SUPPORTED )
  switch ( statusCode )
  {
    case NWK_STATUS_COORD_ADDR:
      if ( ZSTACK_ROUTER_BUILD )
      {
        HalLcdWriteString( (char*)ZigbeeCoordStr, HAL_LCD_LINE_1 );
        HalLcdWriteStringValue( (char*)NetworkIDStr, statusValue, 16, HAL_LCD_LINE_2 );
        BuzzerControl( BUZZER_BLIP );
      }
      break;

    case NWK_STATUS_ROUTER_ADDR:
      if ( ZSTACK_ROUTER_BUILD )
      {
        HalLcdWriteStringValue( (char*)RouterStr, statusValue, 16, HAL_LCD_LINE_1 );
      }
      break;

    case NWK_STATUS_ORPHAN_RSP:
      if ( ZSTACK_ROUTER_BUILD )
      {
        if ( statusValue == ZSuccess )
          HalLcdWriteScreen( (char*)OrphanRspStr, (char*)SentStr );
        else
          HalLcdWriteScreen( (char*)OrphanRspStr, (char*)FailedStr );
      }
      break;

    case NWK_ERROR_ASSOC_RSP:
      if ( ZSTACK_ROUTER_BUILD )
      {
        HalLcdWriteString( (char*)AssocRspFailStr, HAL_LCD_LINE_1 );
        HalLcdWriteValue( (uint32)(statusValue), 16, HAL_LCD_LINE_2 );
      }
      break;

    case NWK_STATUS_ED_ADDR:
      if ( ZSTACK_END_DEVICE_BUILD )
      {
        HalLcdWriteStringValue( (char*)EndDeviceStr, statusValue, 16, HAL_LCD_LINE_1 );
      }
      break;

    case NWK_STATUS_PARENT_ADDR:
            HalLcdWriteStringValue( (char*)ParentStr, statusValue, 16, HAL_LCD_LINE_2 );
      break;

    case NWK_STATUS_ASSOC_CNF:
      HalLcdWriteScreen( (char*)AssocCnfStr, (char*)SuccessStr );
      break;

    case NWK_ERROR_ASSOC_CNF_DENIED:
      HalLcdWriteString((char*)AssocCnfFailStr, HAL_LCD_LINE_1 );
      HalLcdWriteValue( (uint32)(statusValue), 16, HAL_LCD_LINE_2 );
      break;

    case NWK_ERROR_ENERGY_SCAN_FAILED:
      HalLcdWriteScreen( (char*)EnergyLevelStr, (char*)ScanFailedStr );
      break;
  }
#endif
}

/*********************************************************************
 * @fn       nwk_UpdateStatistics()
 *
 * @brief   Update network layer statistic counters
 *
 * @param   statisticCode
 *
 * @return  none
 */
void nwk_UpdateStatistics( uint8 statisticCode )
{
#if defined ( PACKET_FILTER_STATS )
  switch ( statisticCode )
  {
    case STAT_NWK_INVALID_PACKET:
      nwkInvalidPackets++;
      break;

    case STAT_NWK_SECURITY_FAILURE:
      nwkInvalidPackets++;
      nwkSecurityFailures++;
      break;

    case STAT_APS_INVALID_PACKET:
      apsInvalidPackets++;
      break;

    case STAT_APS_SECURITY_FAILURE:
      apsSecurityFailures++;
      break;
  }
#endif
}

/*********************************************************************
*********************************************************************/
