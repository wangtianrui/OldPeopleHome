/**************************************************************************************************
  Filename:       zcl_ss.h
  Revised:        $Date: 2010-02-09 15:28:14 -0800 (Tue, 09 Feb 2010) $
  Revision:       $Revision: 21679 $

  Description:    This file contains the ZCL Security and Safety definitions.


  Copyright 2006-2010 Texas Instruments Incorporated. All rights reserved.

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

#ifndef ZCL_SS_H
#define ZCL_SS_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "zcl.h"

/*********************************************************************
 * CONSTANTS
 */
					
/************************************/
/***  IAS Zone Cluster Attributes ***/
/************************************/
  // Zone Information attributes set
#define ATTRID_SS_IAS_ZONE_STATE                                         0x0000
#define ATTRID_SS_IAS_ZONE_TYPE                                          0x0001
#define ATTRID_SS_IAS_ZONE_STATUS                                        0x0002
/*** Zone State Attribute values ***/
#define SS_IAS_ZONE_STATE_NOT_ENROLLED                                   0x00
#define SS_IAS_ZONE_STATE_ENROLLED                                       0x01
/*** Zone Type Attribute values ***/
#define SS_IAS_ZONE_TYPE_STANDARD_CIE                                    0x0000
#define SS_IAS_ZONE_TYPE_MOTION_SENSOR                                   0x000D
#define SS_IAS_ZONE_TYPE_CONTACT_SWITCH                                  0x0015
#define SS_IAS_ZONE_TYPE_FIRE_SENSOR                                     0x0028
#define SS_IAS_ZONE_TYPE_WATER_SENSOR                                    0x002A
#define SS_IAS_ZONE_TYPE_GAS_SENSOR                                      0x002B
#define SS_IAS_ZONE_TYPE_PERSONAL_EMERGENCY_DEVICE                       0x002C
#define SS_IAS_ZONE_TYPE_VIBRATION_MOVEMENT_SENSOR                       0x002D
#define SS_IAS_ZONE_TYPE_REMOTE_CONTROL                                  0x010F
#define SS_IAS_ZONE_TYPE_KEY_FOB                                         0x0115
#define SS_IAS_ZONE_TYPE_KEYPAD                                          0x021D
#define SS_IAS_ZONE_TYPE_STANDARD_WARNING_DEVICE                         0x0225
#define SS_IAS_ZONE_TYPE_INVALID_ZONE_TYPE                               0xFFFF
/*** Zone Status Attribute values ***/
#define SS_IAS_ZONE_STATUS_ALARM1_ALARMED                                0x0001
#define SS_IAS_ZONE_STATUS_ALARM2_ALARMED                                0x0002	
#define SS_IAS_ZONE_STATUS_TAMPERED_YES                                  0x0004
#define SS_IAS_ZONE_STATUS_BATTERY_LOW                                   0x0008
#define SS_IAS_ZONE_STATUS_SUPERVISION_REPORTS_YES                       0x0010
#define SS_IAS_ZONE_STATUS_RESTORE_REPORTS_YES                           0x0020
#define SS_IAS_ZONE_STATUS_TROUBLE_YES                                   0x0040
#define SS_IAS_ZONE_STATUS_AC_MAINS_FAULT                                0x0080

  // Zone Settings attributes set
#define ATTRID_SS_IAS_CIE_ADDRESS                                        0x0010
  // commands:
#define COMMAND_SS_IAS_ZONE_STATUS_CHANGE_NOTIFICATION                   0x00
#define COMMAND_SS_IAS_ZONE_STATUS_ENROLL_REQUEST                        0x01
#define COMMAND_SS_IAS_ZONE_STATUS_ENROLL_RESPONSE                       0x00
   // permitted values for Enroll Response Code field
#define SS_IAS_ZONE_STATUS_ENROLL_RESPONSE_CODE_SUCCESS                  0x00
#define SS_IAS_ZONE_STATUS_ENROLL_RESPONSE_CODE_NOT_SUPPORTED            0x01
#define SS_IAS_ZONE_STATUS_ENROLL_RESPONSE_CODE_NO_ENROLL_PERMIT         0x02
#define SS_IAS_ZONE_STATUS_ENROLL_RESPONSE_CODE_TOO_MANY_ZONES           0x03

/************************************/
/***  IAS ACE Cluster Attributes  ***/
/************************************/
  // cluster has no attributes

  // command IDs received by Server
  //note: EMERGENCY, FIRE,PANIC, GET_ZONE_ID_MAP cmds have no payload
#define COMMAND_SS_IAS_ACE_ARM                                           0x00  
#define COMMAND_SS_IAS_ACE_BYPASS                                        0x01  
#define COMMAND_SS_IAS_ACE_EMERGENCY                                     0x02  
#define COMMAND_SS_IAS_ACE_FIRE                                          0x03  
#define COMMAND_SS_IAS_ACE_PANIC                                         0x04  
#define COMMAND_SS_IAS_ACE_GET_ZONE_ID_MAP                               0x05  
#define COMMAND_SS_IAS_ACE_GET_ZONE_INFORMATION                          0x06  
  // Cmds, generated by Server
#define COMMAND_SS_IAS_ACE_ARM_RESPONSE                                  0x00
#define COMMAND_SS_IAS_ACE_GET_ZONE_ID_MAP_RESPONSE                      0x01 
#define COMMAND_SS_IAS_ACE_GET_ZONE_INFORMATION_RESPONSE                 0x02 
/*** Arm Mode field permitted values ***/
#define SS_IAS_ACE_ARM_DISARM                                            0x00
#define SS_IAS_ACE_ARM_DAY_HOME_ZONES_ONLY                               0x01
#define SS_IAS_ACE_ARM_NIGHT_SLEEP_ZONES_ONLY                            0x02
#define SS_IAS_ACE_ARM_ALL_ZONES                                         0x03
/*** Arm Notification field permitted values ***/
#define SS_IAS_ACE_ARM_NOTIFICATION_ALL_ZONES_DISARMED                   0x00
#define SS_IAS_ACE_ARM_NOTIFICATION_DAY_HOME_ZONES_ONLY                  0x01
#define SS_IAS_ACE_ARM_NOTIFICATION_NIGHT_SLEEP_ZONES_ONLY               0x02
#define SS_IAS_ACE_ARM_NOTIFICATION_ALL_ZONES_ARMED                      0x03

/************************************/
/***  IAS WD Cluster Attributes   ***/
/************************************/
   // Maximum Duration attribute
#define ATTRID_SS_IAS_WD_MAXIMUM_DURATION                                0x0000
  // commands:
#define COMMAND_SS_IAS_WD_START_WARNING                                  0x00
#define COMMAND_SS_IAS_WD_SQUAWK                                         0x01
/***  warningMode field values ***/
#define SS_IAS_START_WARNING_WARNING_MODE_STOP                           0
#define SS_IAS_START_WARNING_WARNING_MODE_BURGLAR                        1
#define SS_IAS_START_WARNING_WARNING_MODE_FIRE                           2
#define SS_IAS_START_WARNING_WARNING_MODE_EMERGENCY                      3
/*** start warning: strobe field values ***/
#define SS_IAS_START_WARNING_STROBE_NO_STROBE_WARNING                    0
#define SS_IAS_START_WARNING_STROBE_USE_STPOBE_IN_PARALLEL_TO_WARNING    1

/*** squawkMode field values ***/
#define SS_IAS_SQUAWK_SQUAWK_MODE_SYSTEM_ALARMED_NOTIFICATION_SOUND      0
#define SS_IAS_SQUAWK_SQUAWK_MODE_SYSTEM_DISARMED_NOTIFICATION_SOUND     1
/*** squawk strobe field values ***/
#define SS_IAS_SQUAWK_STROBE_NO_STROBE_SQUAWK                            0
#define SS_IAS_SQUAWK_STROBE_USE_STROBE_BLINK_IN_PARALLEL_TO_SQUAWK      1
/*** squawk level field values ***/
#define SS_IAS_SQUAWK_SQUAWK_LEVEL_LOW_LEVEL_SOUND                       0
#define SS_IAS_SQUAWK_SQUAWK_LEVEL_MEDIUM_LEVEL_SOUND                    1
#define SS_IAS_SQUAWK_SQUAWK_LEVEL_HIGH_LEVEL_SOUND                      2
#define SS_IAS_SQUAWK_SQUAWK_LEVEL_VERY_HIGH_LEVEL_SOUND                 3

// The maximum number of entries in the Zone table
#define ZCL_SS_MAX_ZONES                                                 256
#define ZCL_SS_MAX_ZONE_ID                                               254
  
/*********************************************************************
 * TYPEDEFS
 */
/*** ZCL IAS Zone Cluster: Zone Status Change Notification Cmd payload ***/
typedef struct
{
  uint8 zoneStatus;
  uint8 extendedStatus;
} zclCmdSSIASZoneStatusChangeNotification_t;

/***  ZCL IAS Zone Cluster: Zone Enroll Request Cmd payload ***/
typedef struct
{
  uint16 zoneType;
  uint16 manufacturerCode;
} zclCmdSSIASZoneEnrollRequest_t;

/***  ZCL IAS Zone Cluster: Zone Enroll Response Cmd payload ***/
typedef struct
{
  uint8 responseCode;
  uint8 zoneID;
} zclCmdSSIASZoneEnrollResponse_t;

/***  typedef for IAS ACE Zone table ***/
typedef struct
{
  uint8  zoneID;
  uint16 zoneType;
  uint8  zoneAddress[8];
} IAS_ACE_ZoneTable_t;

/***  ZCL IAS ACE Cluster: ARM Cmd payload ***/
typedef uint8 zclCmdSSIASACEArmPayload_t;

/*** ZCL IAS ACE Cluster: BYPASS Cmd payload ***/
typedef struct
{
  uint8       numZones;
  uint8       zoneID[];
} zclCmdSSIASACEBypassPayload_t;

/*** ZCL IAS ACE Cluster: GET_ZONE_INFORNMATION cmd payload: ***/
typedef uint8 zclCmdSSIASACEGetZoneInformationPayload_t;

/***  ZCL IAS ACE Cluster: ARM Response Cmd payload ***/
typedef uint8 zclCmdSSIASACEArmResponsePayload_t;

/*** ZCL IAS ACE Cluster: Get Zone ID Map response Cmd payload ***/
typedef uint16 zclCmdSSIASZoneIDMapSection_t[16];

/*** ZCL IAS ACE Cluster: Get Zone Information Response Cmd payload ***/
typedef struct
{
  uint8  zoneID;
  uint16 zoneType;
  uint8 *ieeeAddress;
} zclCmdSSIASACEGetZoneInformationResponsePayload_t;

/***  ZCL WD Cluster: COMMAND_WD_START_WARNING Cmd payload	***/
typedef struct
{
  unsigned int warnMode:4;    // Warning Mode
  unsigned int warnStrobe:2;  // Strobe
  unsigned int reserved:2;    // Reserved for future use
} warningbits_t;

typedef union
{
  warningbits_t  warningbits;
  uint8          warningbyte;
} warning_t;

typedef struct
{
  warning_t   warningmessage;
  uint16      duration; 
} zclCmdSSWDStartWarningPayload_t;

/***  ZCL WD Cluster: COMMAND_WD_SQUAWK Cmd payload ***/
typedef struct
{
  unsigned int squawkMode:4;
  unsigned int strobe:1;
  unsigned int reserved:1;
  unsigned int squawkLevel:2;
} squawkbits_t;

typedef union
{
  squawkbits_t  squawkbits;
  uint8         squawkbyte;
} zclCmdSSWDSquawkPayload_t;

/*** Structures used for callback functions ***/

typedef struct
{
  uint16 zoneStatus;     // current zone status - bit map
  uint8  extendedStatus; // bit map, currently set to All zeroes ( reserved )
} zclZoneChangeNotif_t;

typedef struct
{
  afAddrType_t *srcAddr;         // initiator's address
  uint8        zoneID;           // allocated zone ID
  uint16       zoneType;         // current value of Zone Type attribute
  uint16       manufacturerCode; // manufacturer Code from node descriptor for the device
} zclZoneEnrollReq_t;

typedef struct
{
  uint8 responseCode; // value of  response Code
  uint8 zoneID;       // index to the zone table of the CIE
} zclZoneEnrollRsp_t;

typedef struct
{
  uint8 numberOfZones; // number of zones ( one byte)
  uint8 *bypassBuf;    // zone IDs array of 256 entries one byte each
} zclACEBypass_t;

typedef struct
{
  uint8 zoneID;    // index to the zone table of the CIE
  uint16 zoneType; // value of Zone Type atribute
  uint8 *ieeeAddr; // pointer to 64 bit address
} zclACEGetZoneInfoRsp_t;

typedef struct
{
  warning_t warnings; // bitfiels ( one byte )
  uint16 duration;    // warning duration in seconds
} zclWDStartWarning_t;


// This callback is called to process a Change Notification command
//  zoneStatus - current zone status - bit map
//  extendedStatus - bit map, currently set to All zeroes ( reserved )
typedef void (*zclSS_ChangeNotification_t)( zclZoneChangeNotif_t *pCmd );

// This callback is called to process a Enroll Request command
//  srcAddr - initiator's address
//  zoneID - allocated zone ID
//  zoneType - current value of Zone Type attribute
//  manufacturerCode - manufacturer Code from node descriptor for the device
typedef void (*zclSS_EnrollRequest_t)( zclZoneEnrollReq_t *pReq );

// This callback is called to process a Enroll Response command
//  responseCode -  value of  response Code
//  zoneID  - index to the zone table of the CIE
typedef void (*zclSS_EnrollResponse_t)( zclZoneEnrollRsp_t *pRsp );

// This callback is called to process an Arm command
//  armMode -  value of arm mode
//  returns Arm Notification
typedef uint8 (*zclSS_ACE_Arm_t)( uint8 armMode );

// This callback is called to process a Bypass command
//  numberOfZones - number of zones ( one byte)
//  bypassBuf - zone IDs array of 256 entries one byte each
typedef void (*zclSS_ACE_Bypass_t)( zclACEBypass_t *pCmd );

// This callback is called to process an Emergency command
//  no payload
typedef void (*zclSS_ACE_Emergency_t)( void );

// This callback is called to process a Fire command
// no payload
typedef void (*zclSS_ACE_Fire_t)( void );

// This callback is called to process a Panic command
// no payload
typedef void (*zclSS_ACE_Panic_t)( void );

// This callback is called to process a Get Zone ID Map command
// no payload
typedef void (*zclSS_ACE_GetZoneIDMap_t)( void );

// This callback is called to process a Get Zone Information command
//  zoneID  - index to the zone table of the CIE
typedef void (*zclSS_ACE_GetZoneInformation_t)( uint8 zoneID );

// This callback is called to process an Arm Response command
//  armNotification -  value of arm notification field
typedef void (*zclSS_ACE_ArmResponse_t)( uint8 armNotification );

// This callback is called to process a Get Zone ID Map Response command
//  pointer to the zone ID Map: table of 16 uint16
typedef void (*zclSS_ACE_GetZoneIDMapResponse_t)( uint16 *zoneIDMap );

// This callback is called to process a Get Zone Information Response command
//  zoneID  - index to the zone table of the CIE
//  zoneType - value of Zone Type atribute
//  *ieeeAddr  - pointer to 64 bit address
typedef void (*zclSS_ACE_GetZoneInformationResponse_t)( zclACEGetZoneInfoRsp_t *pRsp );

// This callback is called to process a Start Warning command
//  warnings - bitfiels ( one byte )
//  duration  - in seconds
typedef void (*zclSS_WD_StartWarning_t)( zclWDStartWarning_t *pCmd );

// This callback is called to process a Squawk command
//  squawks  - bitfields ( one byte )
typedef void (*zclSS_WD_Squawk_t)( zclCmdSSWDSquawkPayload_t squawks );


// Register Callbacks table entry - enter function pointers for callbacks that
// the application would like to receive
typedef struct			
{
  zclSS_ChangeNotification_t               pfnChangeNotification;             // Change Notification command
  zclSS_EnrollRequest_t                    pfnEnrollRequest;                  // Enroll Request command
  zclSS_EnrollResponse_t                   pfnEnrollResponse;                 // Enroll Reponse command
  zclSS_ACE_Arm_t                          pfnACE_Arm;                        // Arm command
  zclSS_ACE_Bypass_t                       pfnACE_Bypass;                     // Bypass command
  zclSS_ACE_Emergency_t                    pfnACE_Emergency;                  // Emergency command
  zclSS_ACE_Fire_t                         pfnACE_Fire;                       // Fire command
  zclSS_ACE_Panic_t                        pfnACE_Panic;                      // Panic command
 // zclSS_ACE_GetZoneIDMap_t                 pfnACE_GetZoneIDMap;               // Get Zone ID Map command
 // zclSS_ACE_GetZoneInformation_t           pfnACE_GetZoneInformation;         // Get Zone Information Command
  zclSS_ACE_ArmResponse_t                  pfnACE_ArmResponse;                // ArmResponse command
  zclSS_ACE_GetZoneIDMapResponse_t         pfnACE_GetZoneIDMapResponse;       // Get Zone ID Map Response command
  zclSS_ACE_GetZoneInformationResponse_t   pfnACE_GetZoneInformationResponse; // Get Zone Information Response command
  zclSS_WD_StartWarning_t                  pfnWD_StartWarning;                // Start Warning command
  zclSS_WD_Squawk_t                        pfnWD_Squawk;                      // Squawk command

} zclSS_AppCallbacks_t;


/*********************************************************************
 * FUNCTION MACROS
 */

#ifdef ZCL_ACE
/*
 *  Send an Emergency Cmd  ( IAS ACE Cluster )
 *  Use like:
 *      ZStatus_t zclSS_Send_IAS_ACE_EmergencyCmd( uint16 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclSS_Send_IAS_ACE_EmergencyCmd(a,b,c,d) zcl_SendCommand( (a), (b), ZCL_CLUSTER_ID_SS_IAS_ACE,\
                                                      COMMAND_SS_IAS_ACE_EMERGENCY, TRUE,\
                                                      ZCL_FRAME_CLIENT_SERVER_DIR, (c), 0, (d), 0, NULL )

/*
 *  Send a Fire Cmd  ( IAS ACE Cluster )
 *  Use like:
 *      ZStatus_t zclSS_Send_IAS_ACE_FireCmd( uint16 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclSS_Send_IAS_ACE_FireCmd(a,b,c,d) zcl_SendCommand( (a), (b), ZCL_CLUSTER_ID_SS_IAS_ACE,\
                                                      COMMAND_SS_IAS_ACE_FIRE, TRUE,\
                                                      ZCL_FRAME_CLIENT_SERVER_DIR, (c), 0, (d), 0, NULL )

/*
 *  Send a Panic Cmd  ( IAS ACE Cluster )
 *  Use like:
 *      ZStatus_t zclSS_Send_IAS_ACE_PanicCmd( uint16 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclSS_Send_IAS_ACE_PanicCmd(a,b,c,d) zcl_SendCommand( (a), (b), ZCL_CLUSTER_ID_SS_IAS_ACE,\
                                                      COMMAND_SS_IAS_ACE_PANIC, TRUE,\
                                                      ZCL_FRAME_CLIENT_SERVER_DIR, (c), 0, (d), 0, NULL )

/*
 *  Send a GetZoneIDMap Cmd  ( IAS ACE Cluster )
 *  Use like:
 *      ZStatus_t zclSS_Send_IAS_ACE_GetZoneIDMapCmd( uint16 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclSS_Send_IAS_ACE_GetZoneIDMapCmd(a,b,c,d) zcl_SendCommand( (a), (b), ZCL_CLUSTER_ID_SS_IAS_ACE,\
                                                      COMMAND_SS_IAS_ACE_GET_ZONE_ID_MAP, TRUE,\
                                                      ZCL_FRAME_CLIENT_SERVER_DIR, (c), 0, (d),  0, NULL )
#endif // ZCL_ACE

/*********************************************************************
 * VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */


 /*
  * Register for callbacks from this cluster library
  */
extern ZStatus_t zclSS_RegisterCmdCallbacks( uint8 endpoint, zclSS_AppCallbacks_t *callbacks );

#ifdef ZCL_ZONE
 /*
  * Call to send out a Change Notification Command
  *      zoneStatus - current zone status - bit map
  *      extendedStatus - bit map, currently set to All zeros ( reserved)
  */
extern ZStatus_t zclSS_IAS_Send_ZoneStatusChangeNotificationCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                             uint16 zoneStatus, uint8 extendedStatus, 
                                                             uint8 disableDefaultRsp, uint8 seqNum );

 /*
  * Call to send out a Enroll Request Command
  *      zoneType - current value of Zone Type attribute
  *      manufacturerCode -  manufacturer Code from node descriptor for the device
  */
extern ZStatus_t zclSS_IAS_Send_ZoneStatusEnrollRequestCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                      uint16 zoneType, uint16 manufacturerCode, 
                                                      uint8 disableDefaultRsp, uint8 seqNum );

 /*
  * Call to send out a Enroll Response Command
  *      responseCode -  value of response Code
  *      zoneID  - index to the zone table of the CIE
  */
extern ZStatus_t zclSS_IAS_Send_ZoneStatusEnrollResponseCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                             uint8 responseCode, uint8 zoneID, 
                                                             uint8 disableDefaultRsp, uint8 seqNum );
#endif // ZCL_ZONE

#ifdef ZCL_ACE
 /*			
  * Call to send out a Arm  Command  ( IAS ACE Cluster )
  *      armMode - uint8
  */
extern ZStatus_t zclSS_Send_IAS_ACE_ArmCmd( uint8 srcEP, afAddrType_t *dstAddr, 
                          uint8 armMode, uint8 disableDefaultRsp, uint8 seqNum );

 /*			
  * Call to send out a Bypass Command ( IAS ACE Cluster )
  *      numberOfZones uint8 from 0 to 255
  *      *bypassBuf - 256 bytes deep buffer  TBD / nnl
  */
extern ZStatus_t zclSS_Send_IAS_ACE_BypassCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                               uint8 numberOfZones, uint8 *bypassBuf, 
                                               uint8 disableDefaultRsp, uint8 seqNum );

 /*			
  * Call to send out a Get Zone Information Command  ( IAS ACE Cluster )
  *     zoneID - 8 bit value from 0 to 255
  */
extern ZStatus_t zclSS_Send_IAS_ACE_GetZoneInformationCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                          uint8 zoneID, uint8 disableDefaultRsp, uint8 seqNum );
 /*			
  * Call to send out a ArmResponse  Command  ( IAS ACE Cluster )
  *      armNotification - uint8
  */
extern ZStatus_t zclSS_Send_IAS_ACE_ArmResponse( uint8 srcEP, afAddrType_t *dstAddr,
                       uint8 armNotification, uint8 disableDefaultRsp, uint8 seqNum );

 /*			
  * Call to send out a Get Zone ID Map Response Command  ( IAS ACE Cluster )
  *     *zoneIDMap - pointer to an array of 16 uint16s
  */
extern ZStatus_t zclSS_Send_IAS_ACE_GetZoneIDMapResponseCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                       uint16 *zoneIDMap, uint8 disableDefaultRsp, uint8 seqNum );

 /*			
  * Call to send out a Get Zone Information Response Command  ( IAS ACE Cluster )
  * zoneID - 8 bit value from 0 to 255
  * zoneType - 16 bit
  * *ieeeAddress - pointer to 64 bit address
  */
extern ZStatus_t zclSS_Send_IAS_ACE_GetZoneInformationResponseCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                    uint8 zoneID, uint16 zoneType, uint8 *ieeeAddress, 
                                                    uint8 disableDefaultRsp, uint8 seqNum );
#endif // ZCL_ACE

#ifdef ZCL_WD
 /*			
  * Call to send out a Start Warning Command  ( IAS WD Cluster )
  *  *warning - ptr to bitfields and duration
  */
extern ZStatus_t zclSS_Send_IAS_WD_StartWarningCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                    zclCmdSSWDStartWarningPayload_t *warning, 
                                                    uint8 disableDefaultRsp, uint8 seqNum );

 /*			
  * Call to send out a Squawk Command  ( IAS WD Cluster )
  *   squawk - ptr to bitfields (one byte worth)
  */
extern ZStatus_t zclSS_Send_IAS_WD_SquawkCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                              zclCmdSSWDSquawkPayload_t *squawk,
                                              uint8 disableDefaultRsp, uint8 seqNum );
#endif // ZCL_WD

#if defined(ZCL_ZONE) || defined(ZCL_ACE)
 /*			
  * Call to update Zone Address for zoneID
  *   ieeeAddr - ptr to IEEE address
  */
extern void zclSS_UpdateZoneAddress( uint8 endpoint, uint8 zoneID, uint8 *ieeeAddr );


 /*			
  * Call to remove a zone with endpoint and zoneID
  *   zoneID - zone to be removed
  */
extern uint8 zclSS_RemoveZone( uint8 endpoint, uint8 zoneID );
#endif // ZCL_ZONE || ZCL_ACE

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ZCL_SS_H */
