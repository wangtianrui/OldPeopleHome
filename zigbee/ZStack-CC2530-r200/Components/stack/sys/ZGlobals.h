/**************************************************************************************************
  Filename:       nwk_globals.h
  Revised:        $Date: 2011-05-27 16:00:27 -0700 (Fri, 27 May 2011) $
  Revision:       $Revision: 26145 $

  Description:    User definable Z-Stack parameters.


  Copyright 2007-2011 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
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

#ifndef ZGLOBALS_H
#define ZGLOBALS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"

/*********************************************************************
 * MACROS
 */
#if defined( BUILD_ALL_DEVICES ) && !defined( ZSTACK_DEVICE_BUILD )
  #define ZSTACK_DEVICE_BUILD  (DEVICE_BUILD_COORDINATOR | DEVICE_BUILD_ROUTER | DEVICE_BUILD_ENDDEVICE)
#endif

// Setup to work with the existing (old) compile flags
#if !defined ( ZSTACK_DEVICE_BUILD )
  #if defined ( ZDO_COORDINATOR )
    #define ZSTACK_DEVICE_BUILD  (DEVICE_BUILD_COORDINATOR)
  #elif defined ( RTR_NWK )
    #define ZSTACK_DEVICE_BUILD  (DEVICE_BUILD_ROUTER)
  #else
    #define ZSTACK_DEVICE_BUILD  (DEVICE_BUILD_ENDDEVICE)
  #endif
#endif

// Use the following to macros to make device type decisions
#define ZG_BUILD_COORDINATOR_TYPE  (ZSTACK_DEVICE_BUILD & DEVICE_BUILD_COORDINATOR)
#define ZG_BUILD_RTR_TYPE          (ZSTACK_DEVICE_BUILD & (DEVICE_BUILD_COORDINATOR | DEVICE_BUILD_ROUTER))
#define ZG_BUILD_ENDDEVICE_TYPE    (ZSTACK_DEVICE_BUILD & DEVICE_BUILD_ENDDEVICE)
#define ZG_BUILD_RTRONLY_TYPE      (ZSTACK_DEVICE_BUILD == DEVICE_BUILD_ROUTER)
#define ZG_BUILD_JOINING_TYPE      (ZSTACK_DEVICE_BUILD & (DEVICE_BUILD_ROUTER | DEVICE_BUILD_ENDDEVICE))

#if ( ZSTACK_DEVICE_BUILD == DEVICE_BUILD_COORDINATOR )
  #define ZG_DEVICE_COORDINATOR_TYPE 1
#else
  #define ZG_DEVICE_COORDINATOR_TYPE (zgDeviceLogicalType == ZG_DEVICETYPE_COORDINATOR)
#endif

#if ( ZSTACK_DEVICE_BUILD == (DEVICE_BUILD_ROUTER | DEVICE_BUILD_COORDINATOR) )
  #define ZG_DEVICE_RTR_TYPE 1
#else
  #define ZG_DEVICE_RTR_TYPE ((zgDeviceLogicalType == ZG_DEVICETYPE_COORDINATOR) || (zgDeviceLogicalType == ZG_DEVICETYPE_ROUTER))
#endif

#if ( ZSTACK_DEVICE_BUILD == DEVICE_BUILD_ENDDEVICE )
  #define ZG_DEVICE_ENDDEVICE_TYPE 1
#else
  #define ZG_DEVICE_ENDDEVICE_TYPE (zgDeviceLogicalType == ZG_DEVICETYPE_ENDDEVICE)
#endif

#define ZG_DEVICE_JOINING_TYPE     ((zgDeviceLogicalType == ZG_DEVICETYPE_ROUTER) || (zgDeviceLogicalType == ZG_DEVICETYPE_ENDDEVICE))

#if ( ZG_BUILD_RTR_TYPE )
  #if ( ZG_BUILD_ENDDEVICE_TYPE )
    #define ZSTACK_ROUTER_BUILD         (ZG_BUILD_RTR_TYPE && ZG_DEVICE_RTR_TYPE)
  #else
    #define ZSTACK_ROUTER_BUILD         1
  #endif
#else
  #define ZSTACK_ROUTER_BUILD           0
#endif

#if ( ZG_BUILD_ENDDEVICE_TYPE )
  #if ( ZG_BUILD_RTR_TYPE )
    #define ZSTACK_END_DEVICE_BUILD     (ZG_BUILD_ENDDEVICE_TYPE && ZG_DEVICE_ENDDEVICE_TYPE)
  #else
    #define ZSTACK_END_DEVICE_BUILD     1
  #endif
#else
  #define ZSTACK_END_DEVICE_BUILD       0
#endif


/*********************************************************************
 * CONSTANTS
 */

// Values for ZCD_NV_LOGICAL_TYPE (zgDeviceLogicalType)
#define ZG_DEVICETYPE_COORDINATOR      0x00
#define ZG_DEVICETYPE_ROUTER           0x01
#define ZG_DEVICETYPE_ENDDEVICE        0x02

//#define DEVICE_LOGICAL_TYPE ZG_DEVICETYPE_COORDINATOR

// Default Device Logical Type
#if !defined ( DEVICE_LOGICAL_TYPE )
  #if ( ZG_BUILD_COORDINATOR_TYPE )
    // If capable, default to coordinator
    #define DEVICE_LOGICAL_TYPE ZG_DEVICETYPE_COORDINATOR
  #elif ( ZG_BUILD_RTR_TYPE )
    #define DEVICE_LOGICAL_TYPE ZG_DEVICETYPE_ROUTER
  #elif ( ZG_BUILD_ENDDEVICE_TYPE )
    // Must be an end device
    #define DEVICE_LOGICAL_TYPE ZG_DEVICETYPE_ENDDEVICE
  #else
    #error ZSTACK_DEVICE_BUILD must be defined as something!
  #endif
#endif

// Transmission retries numbers
#if !defined ( MAX_POLL_FAILURE_RETRIES )
  #define MAX_POLL_FAILURE_RETRIES 1
#endif
#if !defined ( MAX_DATA_RETRIES )
  #define MAX_DATA_RETRIES         2
#endif

// NIB parameters
#if !defined ( MAX_BCAST_RETRIES )
  #define MAX_BCAST_RETRIES        2
#endif
#if !defined ( PASSIVE_ACK_TIMEOUT )
  #define PASSIVE_ACK_TIMEOUT      5
#endif
#if !defined ( BCAST_DELIVERY_TIME )
  #define BCAST_DELIVERY_TIME      30
#endif

#if !defined ( ROUTE_DISCOVERY_TIME )
  #define ROUTE_DISCOVERY_TIME     5
#endif

#if !defined ( APS_DEFAULT_MAXBINDING_TIME )
  #define APS_DEFAULT_MAXBINDING_TIME 16000
#endif

#if !defined ( APS_DEFAULT_NONMEMBER_RADIUS )
  #define APS_DEFAULT_NONMEMBER_RADIUS   2
#endif

#if !defined ( APSF_DEFAULT_WINDOW_SIZE )
  #define APSF_DEFAULT_WINDOW_SIZE       1
#endif

#if !defined ( APSF_DEFAULT_INTERFRAME_DELAY )
  #define APSF_DEFAULT_INTERFRAME_DELAY  50
#endif

// Concentrator values
#if !defined ( CONCENTRATOR_ENABLE )
  #define CONCENTRATOR_ENABLE          false // true if concentrator is enabled
#endif

#if !defined ( CONCENTRATOR_DISCOVERY_TIME )
  #define CONCENTRATOR_DISCOVERY_TIME  0
#endif

#if !defined ( CONCENTRATOR_RADIUS )
  #define CONCENTRATOR_RADIUS          0x0a
#endif

#if !defined ( CONCENTRATOR_ROUTE_CACHE )
  #define CONCENTRATOR_ROUTE_CACHE     false // true if concentrator has route cache
#endif

// The hop count radius for concentrator route discoveries
#if !defined ( CONCENTRATOR_RADIUS )
  #define CONCENTRATOR_RADIUS          0x0a
#endif

#if !defined ( START_DELAY )
  #define START_DELAY                  0x0a
#endif

#if !defined ( SAPI_ENDPOINT )
  #define SAPI_ENDPOINT                0xe0
#endif

#define ZG_STARTUP_CLEAR               0x00
#define ZG_STARTUP_SET                 0xFF

#if !defined ( ZG_TRUSTCENTER_ADDR )
  #define ZG_TRUSTCENTER_ADDR            NWK_PAN_COORD_ADDR
#endif

// Backwards compatible - AMI changed name to SE
#if defined ( AMI_PROFILE ) || defined ( SE_PROFILE )
#define TC_LINKKEY_JOIN
#endif

//--------------------------------------------------------------------
// Security modes
//--------------------------------------------------------------------
#define ZG_SECURITY_NONE         0
#define ZG_SECURITY_RESIDENTIAL  1
#define ZG_SECURITY_PRO_STANDARD 1  // For now, same as residential
#define ZG_SECURITY_PRO_HIGH     3
#define ZG_SECURITY_SE_STANDARD  4

#if ( SECURE == 0 )
  #undef ZG_SECURITY_MODE
  #define ZG_SECURITY_MODE ZG_SECURITY_NONE
#else
  #if !defined ( ZG_SECURITY_MODE )
    #if defined ( TC_LINKKEY_JOIN )
      #define ZG_SECURITY_MODE ZG_SECURITY_SE_STANDARD
    #else
      #if !defined ( ZIGBEEPRO )
        #define ZG_SECURITY_MODE ZG_SECURITY_RESIDENTIAL
      #else
//        #define ZG_SECURITY_MODE ZG_SECURITY_PRO_HIGH
        #define ZG_SECURITY_MODE ZG_SECURITY_PRO_STANDARD  // Default for Pro Release is Standard (residential)
      #endif
    #endif
  #endif
#endif

//--------------------------------------------------------------------
// Security settings
//--------------------------------------------------------------------
#if !defined ( ZG_SECURE_DYNAMIC )
#define ZG_SECURE_DYNAMIC 1
#endif

#if ( ZG_SECURE_DYNAMIC == 1 )
  #define ZG_SECURE_ENABLED ( SECURE && ( zgSecurityMode != ZG_SECURITY_NONE ) )
  #define ZG_CHECK_SECURITY_MODE ( zgSecurityMode )
#else
  #define ZG_SECURE_ENABLED ( SECURE )
  #define ZG_CHECK_SECURITY_MODE ( ZG_SECURITY_MODE )
#endif

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * NWK GLOBAL VARIABLES
 */

extern uint16 zgPollRate;
extern uint16 zgQueuedPollRate;
extern uint16 zgResponsePollRate;
extern uint16 zgRejoinPollRate;

// Variables for number of transmission retries
extern uint8 zgMaxDataRetries;
extern uint8 zgMaxPollFailureRetries;

extern uint32 zgDefaultChannelList;
extern uint8  zgDefaultStartingScanDuration;

extern uint8 zgStackProfile;

extern uint8 zgIndirectMsgTimeout;
extern uint8 zgSecurityMode;
extern uint8 zgSecurePermitJoin;
extern uint16 zgTrustCenterAddr;
extern uint8 zgRouteDiscoveryTime;
extern uint8 zgRouteExpiryTime;

extern uint8 zgExtendedPANID[];

extern uint8 zgMaxBcastRetires;
extern uint8 zgPassiveAckTimeout;
extern uint8 zgBcastDeliveryTime;

extern uint8 zgNwkMode;

extern uint8 zgConcentratorEnable;
extern uint8 zgConcentratorDiscoveryTime;
extern uint8 zgConcentratorRadius;
extern uint8 zgConcentratorRC;
extern uint8 zgNwkSrcRtgExpiryTime;

/*********************************************************************
 * APS GLOBAL VARIABLES
 */

extern uint8  zgApscMaxFrameRetries;
extern uint16 zgApscAckWaitDurationPolled;
extern uint8  zgApsAckWaitMultiplier;
extern uint16 zgApsDefaultMaxBindingTime;
extern uint8  zgApsUseExtendedPANID[Z_EXTADDR_LEN];
extern uint8  zgApsUseInsecureJoin;
extern uint8  zgApsNonMemberRadius;

/*********************************************************************
 * SECURITY GLOBAL VARIABLES
 */

extern uint8 zgPreConfigKeys;
extern uint8 zgUseDefaultTCLK;

/*********************************************************************
 * ZDO GLOBAL VARIABLES
 */

extern uint16 zgConfigPANID;
extern uint8  zgDeviceLogicalType;
extern uint8  zgNwkMgrMinTransmissions;
#if !defined MT_TASK
extern uint8 zgZdoDirectCB;
#endif

/*********************************************************************
 * APPLICATION VARIABLES
 */

extern uint8 zgNwkMgrMode;

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Initialize the Z-Stack Globals.
 */
extern uint8 zgInit( void );

/*
 * Initialize the RAM Items table with NV values.
 *    setDefault - if calling from outside ZGlobal use FALSE.
 */
extern void zgInitItems( uint8 setDefault );

/*
 * Get Startup Options (ZCD_NV_STARTUP_OPTION NV Item)
 */
extern uint8 zgReadStartupOptions( void );

/*
 * Write Startup Options (ZCD_NV_STARTUP_OPTION NV Item)
 *
 *      action - ZG_STARTUP_SET set bit, ZG_STARTUP_CLEAR to clear bit.
 *               The set bit is an OR operation, and the clear bit is an
 *               AND ~(bitOptions) operation.
 *      bitOptions - which bits to perform action on:
 *                      ZCD_STARTOPT_DEFAULT_CONFIG_STATE
 *                      ZDC_STARTOPT_DEFAULT_NETWORK_STATE
 *
 * Returns - ZSUCCESS if successful
 */
extern uint8 zgWriteStartupOptions( uint8 action, uint8 bitOptions );

/*
 *  Set RAM variables from set-NV, if it exist in the zgItemTable
 */
extern void zgSetItem( uint16 id, uint16 len, void *buf );

/*********************************************************************
*********************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* ZGLOBALS_H */
