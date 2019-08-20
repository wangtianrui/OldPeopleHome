/**************************************************************************************************
  Filename:       zcl_closures.h
  Revised:        $Date: 2011-05-02 10:30:51 -0700 (Mon, 02 May 2011) $
  Revision:       $Revision: 25832 $

  Description:    This file contains the ZCL Closures definitions.


  Copyright 2006-2011 Texas Instruments Incorporated. All rights reserved.

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

#ifndef ZCL_CLOSURES_H
#define ZCL_CLOSURES_H

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

/**********************************************/
/*** Shade Configuration Cluster Attributes ***/
/**********************************************/
  // Shade information attributes set
#define ATTRID_CLOSURES_PHYSICAL_CLOSED_LIMIT                          0x0000
#define ATTRID_CLOSURES_MOTOR_STEP_SIZE                                0x0001
#define ATTRID_CLOSURES_STATUS                                         0x0002
/*** Status attribute bit values ***/
#define CLOSURES_STATUS_SHADE_IS_OPERATIONAL                           0x01
#define CLOSURES_STATUS_SHADE_IS_ADJUSTING                             0x02
#define CLOSURES_STATUS_SHADE_DIRECTION                                0x04
#define CLOSURES_STATUS_SHADE_MOTOR_FORWARD_DIRECTION                  0x08
  // Shade settings attributes set
#define ATTRID_CLOSURES_CLOSED_LIMIT                                   0x0010
#define ATTRID_CLOSURES_MODE                                           0x0012
/*** Mode attribute values ***/
#define CLOSURES_MODE_NORMAL_MODE                                      0x00
#define CLOSURES_MODE_CONFIGURE_MODE                                   0x01
// cluster has no specific commands

/**********************************************/
/*** Logical Cluster ID - for mapping only ***/
/***  These are not to be used over-the-air ***/
/**********************************************/
#define ZCL_CLOSURES_LOGICAL_CLUSTER_ID_SHADE_CONFIG                   0x0010


/************************************/
/*** Door Lock Cluster Attributes ***/
/************************************/
#define ATTRID_CLOSURES_LOCK_STATE                         0x0000
#define ATTRID_CLOSURES_LOCK_TYPE                          0x0001
#define ATTRID_CLOSURES_ACTUATOR_ENABLED                   0x0002
#define ATTRID_CLOSURES_DOOR_STATE                         0x0003
#define ATTRID_CLOSURES_NUM_OF_DOOR_OPEN_EVENTS            0x0004
#define ATTRID_CLOSURES_NUM_OF_DOOR_CLOSED_EVENTS          0x0005
#define ATTRID_CLOSURES_OPEN_PERIOD                        0x0006
  
/*** Lock State Attribute types ***/
#define CLOSURES_LOCK_STATE_NOT_FULLY_LOCKED               0x00
#define CLOSURES_LOCK_STATE_LOCKED                         0x01
#define CLOSURES_LOCK_STATE_UNLOCKED                       0x02
  
/*** Lock Type Attribute types ***/
#define CLOSURES_LOCK_TYPE_DEADBOLT                        0x00
#define CLOSURES_LOCK_TYPE_MAGNETIC                        0x01
#define CLOSURES_LOCK_TYPE_OTHER                           0x02

/*** Door State Attribute types ***/
#define CLOSURES_DOOR_STATE_OPEN                           0x00
#define CLOSURES_DOOR_STATE_CLOSED                         0x01
#define CLOSURES_DOOR_STATE_ERROR_JAMMED                   0x02
#define CLOSURES_DOOR_STATE_ERROR_FORCED_OPEN              0x03
#define CLOSURES_DOOR_STATE_ERROR_UNSPECIFIED              0x04

/**********************************/
/*** Door Lock Cluster Commands ***/
/**********************************/
#define COMMAND_CLOSURES_LOCK_DOOR                         0x00
#define COMMAND_CLOSURES_UNLOCK_DOOR                       0x01

#define COMMAND_CLOSURES_LOCK_DOOR_RESPONSE                0x00
#define COMMAND_CLOSURES_UNLOCK_DOOR_RESPONSE              0x01

#define DOORLOCK_RES_PAYLOAD_LEN                           0x01

/**********************************************/
/*** Window Covering Cluster Attribute Sets ***/
/**********************************************/
#define ATTRSET_WINDOW_COVERING_INFO                        0x0000
#define ATTRSET_WINDOW_COVERING_SETTINGS                    0x0010
  
/******************************************/
/*** Window Covering Cluster Attributes ***/
/******************************************/
//Window Covering Information
#define ATTRID_CLOSURES_WINDOW_COVERING_TYPE                ( ATTRSET_WINDOW_COVERING_INFO + 0x0000 )
#define ATTRID_CLOSURES_PHYSICAL_CLOSE_LIMIT_LIFT_CM        ( ATTRSET_WINDOW_COVERING_INFO + 0x0001 )
#define ATTRID_CLOSURES_PHYSICAL_CLOSE_LIMIT_TILT_DDEGREE   ( ATTRSET_WINDOW_COVERING_INFO + 0x0002 )
#define ATTRID_CLOSURES_CURRENT_POSITION_LIFT_CM            ( ATTRSET_WINDOW_COVERING_INFO + 0x0003 )
#define ATTRID_CLOSURES_CURRENT_POSITION_TILT_DDEGREE       ( ATTRSET_WINDOW_COVERING_INFO + 0x0004 )
#define ATTRID_CLOSURES_NUM_OF_ACTUATION_LIFT               ( ATTRSET_WINDOW_COVERING_INFO + 0x0005 )
#define ATTRID_CLOSURES_NUM_OF_ACTUATION_TILT               ( ATTRSET_WINDOW_COVERING_INFO + 0x0006 )
#define ATTRID_CLOSURES_CONFIG_STATUS                       ( ATTRSET_WINDOW_COVERING_INFO + 0x0007 )
#define ATTRID_CLOSURES_CURRENT_POSITION_LIFT_PERCENTAGE    ( ATTRSET_WINDOW_COVERING_INFO + 0x0008 )
#define ATTRID_CLOSURES_CURRENT_POSITION_TILT_PERCENTAGE    ( ATTRSET_WINDOW_COVERING_INFO + 0x0009 )

//Window Covering Setting
#define ATTRID_CLOSURES_INSTALLED_OPEN_LIMIT_LIFT_CM        ( ATTRSET_WINDOW_COVERING_SETTINGS + 0x0000 )
#define ATTRID_CLOSURES_INSTALLED_CLOSED_LIMIT_LIFT_CM      ( ATTRSET_WINDOW_COVERING_SETTINGS + 0x0001 )
#define ATTRID_CLOSURES_INSTALLED_OPEN_LIMIT_TILT_DDEGREE   ( ATTRSET_WINDOW_COVERING_SETTINGS + 0x0002 )
#define ATTRID_CLOSURES_INSTALLED_CLOSED_LIMIT_TILT_DDEGREE ( ATTRSET_WINDOW_COVERING_SETTINGS + 0x0003 )
#define ATTRID_CLOSURES_VELOCITY_LIFT                       ( ATTRSET_WINDOW_COVERING_SETTINGS + 0x0004 )
#define ATTRID_CLOSURES_ACCELERATION_TIME_LIFT              ( ATTRSET_WINDOW_COVERING_SETTINGS + 0x0005 )
#define ATTRID_CLOSURES_DECELERATION_TIME_LIFT              ( ATTRSET_WINDOW_COVERING_SETTINGS + 0x0006 )
#define ATTRID_CLOSURES_WINDOW_COVERING_MODE                ( ATTRSET_WINDOW_COVERING_SETTINGS + 0x0007 )
#define ATTRID_CLOSURES_INTERMEDIATE_SETPOINTS_LIFT         ( ATTRSET_WINDOW_COVERING_SETTINGS + 0x0008 )
#define ATTRID_CLOSURES_INTERMEDIATE_SETPOINTS_TILT         ( ATTRSET_WINDOW_COVERING_SETTINGS + 0x0009 )
  
/*** Window Covering Type Attribute types ***/
#define CLOSURES_WINDOW_COVERING_TYPE_ROLLERSHADE                       0x00
#define CLOSURES_WINDOW_COVERING_TYPE_ROLLERSHADE_2_MOTOR               0x01
#define CLOSURES_WINDOW_COVERING_TYPE_ROLLERSHADE_EXTERIOR              0x02
#define CLOSURES_WINDOW_COVERING_TYPE_ROLLERSHADE_EXTERIOR_2_MOTOR      0x03
#define CLOSURES_WINDOW_COVERING_TYPE_DRAPERY                           0x04
#define CLOSURES_WINDOW_COVERING_TYPE_AWNING                            0x05
#define CLOSURES_WINDOW_COVERING_TYPE_SHUTTER                           0x06
#define CLOSURES_WINDOW_COVERING_TYPE_TILT_BLIND_TILT_ONLY              0x07
#define CLOSURES_WINDOW_COVERING_TYPE_TILT_BLIND_LIFT_AND_TILT          0x08
#define CLOSURES_WINDOW_COVERING_TYPE_PROJECTOR_SCREEN                  0x09


/****************************************/
/*** Window Covering Cluster Commands ***/
/****************************************/
#define COMMAND_CLOSURES_UP_OPEN                            ( 0x00 )
#define COMMAND_CLOSURES_DOWN_CLOSE                         ( 0x01 )
#define COMMAND_CLOSURES_STOP                               ( 0x02 )
#define COMMAND_CLOSURES_GO_TO_LIFT_SETPOINT                ( 0x03 )
#define COMMAND_CLOSURES_GO_TO_LIFT_VALUE                   ( 0x04 )
#define COMMAND_CLOSURES_GO_TO_LIFT_PERCENTAGE              ( 0x05 )
#define COMMAND_CLOSURES_GO_TO_TILT_SETPOINT                ( 0x06 )
#define COMMAND_CLOSURES_GO_TO_TILT_VALUE                   ( 0x07 )
#define COMMAND_CLOSURES_GO_TO_TILT_PERCENTAGE              ( 0x08 )
#define COMMAND_CLOSURES_PROGRAM_SETPOINT                   ( 0x09 )

#define ZCL_WC_GOTOSETPOINTREQ_PAYLOADLEN                   ( 1 )
#define ZCL_WC_GOTOVALUEREQ_PAYLOADLEN                      ( 2 )
#define ZCL_WC_GOTOPERCENTAGEREQ_PAYLOADLEN                 ( 1 )
#define ZCL_WC_PROGRAMSETPOINTREQ_VER1_PAYLOADLEN           ( 4 )
#define ZCL_WC_PROGRAMSETPOINTREQ_VER2_PAYLOADLEN           ( 1 )

/*********************************************************************
 * TYPEDEFS
 */


/*** Window Covering Cluster - Bits in Config/Status Attribute ***/
typedef struct
{
  uint8 Operational : 1;              // Window Covering is operational or not
  uint8 Online : 1;                   // Window Covering is enabled for transmitting over the Zigbee network or not
  uint8 CommandsReversed : 1;         // Identifies the direction of rotation for the Window Covering
  uint8 LiftControl : 1;              // Identifies, lift control supports open loop or closed loop
  uint8 TiltControl : 1;              // Identifies, tilt control supports open loop or closed loop
  uint8 LiftEncoderControlled : 1;    // Identifies, lift control uses Timer or Encoder
  uint8 TiltEncoderControlled : 1;    // Identifies, tilt control uses Timer or Encoder
  uint8 Reserved : 1;
}zclClosuresWcInfoConfigStatus_t;

/*** Window Covering Cluster - Bits in Mode Attribute ***/
typedef struct
{
  uint8 MotorReverseDirection : 1;    // Defines the direction of the motor rotation
  uint8 RunInCalibrationMode : 1;     // Defines Window Covering is in calibration mode or in normal mode
  uint8 RunInMaintenanceMode : 1;     // Defines motor is running in maintenance mode or in normal mode
  uint8 LEDFeedback : 1;              // Enables or Disables feedback LED
  uint8 Reserved : 4;
}zclClosuresWcSetMode_t;

/*** Window Covering Cluster - Setpoint type ***/
typedef enum
{
  lift = 0,
  tilt = 1,
}setpointType_t;

/*** Window Covering Cluster - Setpoint version ***/
typedef enum
{
  programSetpointVersion1 = 1,
  programSetpointVersion2,
}setpointVersion_t;

/*** Window Covering - Program Setpoint Command payload struct ***/
typedef struct
{
  setpointVersion_t version;        // Version of the Program Setpoint command
  uint8 setpointIndex;              // Index of the Setpoint
  uint16 setpointValue;             // Value of the Setpoint
  setpointType_t setpointType;      // Type of the Setpoint; it should be either lift or tilt
}programSetpointPayload_t;

/*** Window Covering Command - General struct ***/
typedef struct
{
  afAddrType_t            *srcAddr;   // requestor's address
  uint8                   cmdID;      // Command id
  uint8                   seqNum;     // Sequence number received with the message
  
  union                               // Payload
  {
    uint8 indexOfLiftSetpoint;
    uint8 percentageLiftValue;
    uint16 liftValue;
    uint8 indexOfTiltSetpoint;
    uint8 percentageTiltValue;
    uint16 TiltValue;
    programSetpointPayload_t programSetpoint;
  }un;
}zclWindowCovering_t;

//This callback is called to process an incoming Door Lock/Unlock command
// cmd - Which is the received command, either Door Lock or Door Unlock command
// seqNum - Transaction Sequence Number
// srcAddr - requestor's address
typedef void (*zclClosures_DoorLock_t) ( uint8 cmd, afAddrType_t *srcAddr, uint8 seqNum );

//This callback is called to process an incoming Door Lock/Unlock response
// cmd - Which is the received command, either Door Lock or Door Unlock response
// seqNum - Transaction Sequence Number
// srcAddr - requestor's address
typedef void (*zclClosures_DoorLockRes_t) ( uint8 cmd, afAddrType_t *srcAddr, uint8 seqNum );

//This callback is called to process an incoming Window Covering cluster basic commands
typedef void (*zclClosures_WindowCoveringSimple_t) ( void );

//This callback is called to process an incoming Window Covering cluster goto percentage commands
// percentage - value to process
// return TRUE on success
typedef bool (*zclClosures_WindowCoveringGotoPercentage_t) ( uint8 percentage );

//This callback is called to process an incoming Window Covering cluster goto value commands
// value - value to process
// return TRUE on success
typedef bool (*zclClosures_WindowCoveringGotoValue_t) ( uint16 value );

//This callback is called to process an incoming Window Covering cluster goto setpoint commands
// index - value to process
// return ZCL status: ZCL_STATUS_SUCCESS, ZCL_STATUS_INVALID_VALUE or ZCL_STATUS_NOT_FOUND.
typedef uint8 (*zclClosures_WindowCoveringGotoSetpoint_t) ( uint8 index );

//This callback is called to process an incoming Window Covering cluster program setpoint commands
// setpoint - values to process, including version, type, index and value
// return TRUE on success, FALSE in case of insufficient space
typedef bool (*zclClosures_WindowCoveringProgramSetpoint_t) ( programSetpointPayload_t *setpoint );

// Register Callbacks table entry - enter function pointers for callbacks that
// the application would like to receive
typedef struct			
{
  zclClosures_DoorLock_t                      pfnDoorLock;        // Door Lock cluster commands
  zclClosures_DoorLockRes_t                   pfnDoorLockRes;     // Door Lock cluster response
  zclClosures_WindowCoveringSimple_t          pfnWindowCoveringUpOpen;
  zclClosures_WindowCoveringSimple_t          pfnWindowCoveringDownClose;
  zclClosures_WindowCoveringSimple_t          pfnWindowCoveringStop;
  zclClosures_WindowCoveringGotoSetpoint_t    pfnWindowCoveringGotoLiftSetpoint;
  zclClosures_WindowCoveringGotoValue_t       pfnWindowCoveringGotoLiftValue;
  zclClosures_WindowCoveringGotoPercentage_t  pfnWindowCoveringGotoLiftPercentage;
  zclClosures_WindowCoveringGotoSetpoint_t    pfnWindowCoveringGotoTiltSetpoint;
  zclClosures_WindowCoveringGotoValue_t       pfnWindowCoveringGotoTiltValue;
  zclClosures_WindowCoveringGotoPercentage_t  pfnWindowCoveringGotoTiltPercentage; 
  zclClosures_WindowCoveringProgramSetpoint_t pfnWindowCoveringProgramSetpoint;
} zclClosures_AppCallbacks_t;


/*********************************************************************
 * VARIABLES
 */


/*********************************************************************
 * FUNCTIONS
 */

 /*
  * Register for callbacks from this cluster library
  */
extern ZStatus_t zclClosures_RegisterCmdCallbacks( uint8 endpoint, zclClosures_AppCallbacks_t *callbacks );

extern ZStatus_t zclClosures_SendDoorLockRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                                  uint8 cmd, uint16 clusterID,
                                                  uint8 disableDefaultRsp, uint8 seqNum ); 
extern ZStatus_t zclClosures_SendDoorLockResponse( uint8 srcEP, afAddrType_t *dstAddr,
                                                   uint8 cmd, uint16 clusterID,
                                                   uint8 disableDefaultRsp, uint8 seqNum, uint8 status ); 
extern ZStatus_t zclClosures_WCSimpleRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                              uint8 cmd, uint8 disableDefaultRsp, uint8 seqNum );
extern ZStatus_t zclClosures_WCSendGoToSetpointRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                                        uint8 cmd, uint8 disableDefaultRsp, 
                                                        uint8 seqNum, uint8 SetpointIndex );
extern ZStatus_t zclClosures_WCSendGoToValueRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                                     uint8 cmd, uint8 disableDefaultRsp, 
                                                     uint8 seqNum, uint16 Value );
extern ZStatus_t zclClosures_WCSendGoToPercentageRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                                          uint8 cmd, uint8 disableDefaultRsp, 
                                                          uint8 seqNum, uint8 percentageValue );
extern ZStatus_t zclClosures_WCSendProgramSetpointRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                                           uint8 cmd, uint8 disableDefaultRsp, 
                                                           uint8 seqNum, programSetpointPayload_t *programSetpoint );

/*********************************************************************
 * FUNCTION MACROS
 */
/*
 *  Send a Door Lock Command
 *  Use like:
 *      ZStatus_t zclClosures_SendDoorLock( uint8 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum )
 */
#define zclClosures_SendDoorLock(a, b, c, d) zclClosures_SendDoorLockRequest((a), (b), COMMAND_CLOSURES_LOCK_DOOR, ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK, (c), (d))

/*
 *  Send a Door Lock Response
 *  Use like:
 *      ZStatus_t zclClosures_SendDoorLockRes( uint8 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum, uint8 status )
 */
#define zclClosures_SendDoorLockRsp(a, b, c, d, e) zclClosures_SendDoorLockResponse( (a), (b), COMMAND_CLOSURES_LOCK_DOOR_RESPONSE, ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK, (c), (d), (e) )

/*
 *  Send a Door Unlock Command
 *  Use like:
 *      ZStatus_t zclClosures_SendDoorUnlock( uint8 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum )
 */
#define zclClosures_SendDoorUnlock(a, b, c, d) zclClosures_SendDoorLockRequest((a), (b), COMMAND_CLOSURES_UNLOCK_DOOR, ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK, (c), (d))

/*
 *  Send a Door Lock Response
 *  Use like:
 *      ZStatus_t zclClosures_SendDoorUnlockRes( uint8 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum, uint8 status )
 */
#define zclClosures_SendDoorUnlockRsp(a, b, c, d, e) zclClosures_SendDoorLockResponse( (a), (b), COMMAND_CLOSURES_UNLOCK_DOOR_RESPONSE, ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK, (c), (d), (e) )

/*
 *  Send a Up/Open Request Command
 *  Use like:
 *      ZStatus_t zclClosures_SendUpOpen( uint8 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum )
 */
#define zclClosures_SendUpOpen(a, b, c, d) zclClosures_WCSimpleRequest( (a), (b), COMMAND_CLOSURES_UP_OPEN, (c), (d) )
/*
 *  Send a Down/Close Request Command
 *  Use like:
 *      ZStatus_t zclClosures_SendDownClose( uint8 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum )
 */
#define zclClosures_SendDownClose(a, b, c, d) zclClosures_WCSimpleRequest( (a), (b), COMMAND_CLOSURES_DOWN_CLOSE, (c), (d) )

/*
 *  Send a Stop Request Command
 *  Use like:
 *      ZStatus_t zclClosures_SendStop( uint8 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum )
 */
#define zclClosures_SendStop(a, b, c, d) zclClosures_WCSimpleRequest( (a), (b), COMMAND_CLOSURES_STOP, (c), (d) )

/*
 *  Send a GoToLiftSetpoint Request Command
 *  Use like:
 *      ZStatus_t zclClosures_SendGoToLiftSetpoint( uint8 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum, uint8 liftSetpoint )
 */
#define zclClosures_SendGoToLiftSetpoint(a, b, c, d, e) zclClosures_WCSendGoToSetpointRequest( (a), (b), COMMAND_CLOSURES_GO_TO_LIFT_SETPOINT, (c), (d), (e) )

/*
 *  Send a GoToLiftValue Request Command
 *  Use like:
 *      ZStatus_t zclClosures_SendGoToLiftValue( uint8 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum, uint16 liftValue )
 */
#define zclClosures_SendGoToLiftValue(a, b, c, d, e) zclClosures_WCSendGoToValueRequest((a), (b), COMMAND_CLOSURES_GO_TO_LIFT_VALUE, (c), (d), (e))

/*
 *  Send a GoToLiftPercentage Request Command
 *  Use like:
 *      ZStatus_t zclClosures_SendGoToLiftPercentage( uint8 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum, uint8 percentageLiftValue )
 */
#define zclClosures_SendGoToLiftPercentage(a, b, c, d, e) zclClosures_WCSendGoToLiftPercentageRequest((a), (b), COMMAND_CLOSURES_GO_TO_LIFT_PERCENTAGE, (c), (d), (e))

/*
 *  Send a GoToTiltSetpoint Request Command
 *  Use like:
 *      ZStatus_t zclClosures_SendGoToTiltSetpoint( uint8 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum, uint8 tiltSetpoint)
 */
#define zclClosures_SendGoToTiltSetpoint(a, b, c, d, e) zclClosures_WCSendGoToSetpointRequest( (a), (b), COMMAND_CLOSURES_GO_TO_TILT_SETPOINT, (c), (d), (e) )

/*
 *  Send a GoToTiltValue Request Command
 *  Use like:
 *      ZStatus_t zclClosures_SendGoToTiltValue( uint8 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum, uint16 tiltValue)
 */
#define zclClosures_SendGoToTiltValue(a, b, c, d, e) zclClosures_WCSendGoToValueRequest((a), (b), COMMAND_CLOSURES_GO_TO_TILT_VALUE, (c), (d), (e))

/*
 *  Send a GoToTiltPercentage Request Command
 *  Use like:
 *      ZStatus_t zclClosures_SendGoToTiltPercentage( uint8 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum, uint8 percentageTiltValue)
 */
#define zclClosures_SendGoToTiltPercentage(a, b, c, d, e) zclClosures_WCSendGoToTiltPercentageRequest((a), (b), COMMAND_CLOSURES_GO_TO_TILT_PERCENTAGE, (c), (d), (e))

/*
 *  Send a GoToTiltPercentage Request Command
 *  Use like:
 *      ZStatus_t zclClosures_SendProgramSetpoint( uint8 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum, programSetpointPayload_t *programSetpoint )
 */
#define zclClosures_SendProgramSetpoint(a, b, c, d, e) zclClosures_WCSendProgramSetpointRequest((a), (b), COMMAND_CLOSURES_PROGRAM_SETPOINT, (c), (d), (e))

#ifdef __cplusplus
}
#endif

#endif /* ZCL_CLOSURES_H */
