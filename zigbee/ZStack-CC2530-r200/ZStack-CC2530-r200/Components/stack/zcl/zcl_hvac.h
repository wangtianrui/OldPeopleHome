/**************************************************************************************************
  Filename:       zcl_hvac.h
  Revised:        $Date: 2011-05-19 11:53:12 -0700 (Thu, 19 May 2011) $
  Revision:       $Revision: 26031 $

  Description:    This file contains the ZCL HVAC definitions.


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

#ifndef ZCL_HVAC_H
#define ZCL_HVAC_H

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

/**************************************************************************/
/***          HVAC: Pump Config and Control Cluster Attributes          ***/
/**************************************************************************/
  // Pump information attribute set
#define ATTRID_HVAC_PUMP_MAX_PRESSURE                                    0x0000
#define ATTRID_HVAC_PUMP_MAX_SPEED                                       0x0001
#define ATTRID_HVAC_PUMP_MAX_FLOW                                        0x0002
#define ATTRID_HVAC_PUMP_MIN_CONST_PRESSURE                              0x0003
#define ATTRID_HVAC_PUMP_MAX_CONST_PRESSURE                              0x0004
#define ATTRID_HVAC_PUMP_MIN_COMP_PRESSURE                               0x0005
#define ATTRID_HVAC_PUMP_MAX_COMP_PRESSURE                               0x0006
#define ATTRID_HVAC_PUMP_MIN_CONST_SPEED                                 0x0007
#define ATTRID_HVAC_PUMP_MAX_CONST_SPEED                                 0x0008
#define ATTRID_HVAC_PUMP_MIN_CONST_FLOW                                  0x0009
#define ATTRID_HVAC_PUMP_MAX_CONST_FLOW                                  0x000A
#define ATTRID_HVAC_PUMP_MIN_CONST_TEMP                                  0x000B
#define ATTRID_HVAC_PUMP_MAX_CONST_TEMP                                  0x000C
  // Pump Dynamic Information attribute set
#define ATTRID_HVAC_PUMP_STATUS                                          0x0010
#define ATTRID_HVAC_PUMP_EFECTIVE_OPERATION_MODE                         0x0011
#define ATTRID_HVAC_PUMP_EFFECTIVE_CONTROL_MODE                          0x0012
#define ATTRID_HVAC_PUMP_CAPACITY                                        0x0013
#define ATTRID_HVAC_PUMP_SPEED                                           0x0014
#define ATTRID_HVAC_PUMP_LIFETIME_RUNNING_HOURS                          0x0015
#define ATTRID_HVAC_PUMP_POWER                                           0x0016
#define ATTRID_HVAC_LIFETIME_ENERGY_CONSUMED                             0x0017
  /*** Pump status attribute values ***/
#define HVAC_PUMP_DEVICE_FAULT                                           0x0000
#define HVAC_PUMP_SUPPLY_FAULT                                           0x0001
#define HVAC_PUMP_SPEED_LOW                                              0x0002
#define HVAC_PUMP_SPEED_HIGH                                             0x0003
#define HVAC_PUMP_LOCAL_OVERRIDE                                         0x0004
#define HVAC_PUMP_RUNNING                                                0x0005
#define HVAC_PUMP_REMOTE_PRESSURE                                        0x0006
#define HVAC_PUMP_REMOTE_FLOW                                            0x0007
#define HVAC_PUMP_REMOTE_TEMPERATURE                                     0x0008
  // Pump Settings attributes set
#define ATTRID_HVAC_PUMP_OPERATION_MODE                                  0x0020
#define ATTRID_HVAC_PUMP_CONTROL_MODE                                    0x0021
#define ATTRID_HVAC_PUMP_ALARM_MASK                                      0x0022
//#define ATTRID_HVAC_PUMP_CAPACITY_MIN_PERCENT_CHANGE                     0x0023
//#define ATTRID_HVAC_PUMP_CAPACITY_MIN_ABSOLUTE_CHANGE                    0x0024
/*** Operation mode attribute values ***/
#define HVAC_PUMP_OPERATION_MODE_NORMAL                                  0x00
#define HVAC_PUMP_OPERATION_MODE_MINIMUM                                 0x01
#define HVAC_PUMP_OPERATION_MODE_MAXIMUM                                 0x02
#define HVAC_PUMP_OPERATION_MODE_LOCAL                                   0x03
/*** Control mode attribute values ***/
#define HVAC_PUMP_CONTROL_MODE_CONSTANT_SPEED                            0x00
#define HVAC_PUMP_CONTROL_MODE_CONSTANT_PRESSURE                         0x01
#define HVAC_PUMP_CONTROL_MODE_PR0PORTIONAL_PRESSURE                     0x02
#define HVAC_PUMP_CONTROL_MODE_CONSTANT_FLOW	                         0x03
#define HVAC_PUMP_CONTROL_MODE_CONSTANT_TEMPERATURE	                 0x05
#define HVAC_PUMP_CONTROL_MODE_CONSTANT_AUTOMATIC                        0x07
/*** Alarm Mask attribute values ***/
#define HVAC_PUMP_SUPPLY_VOLTAGE_TOO_LOW                                 0x0001
#define HVAC_PUMP_SUPPLY_VOLTAGE_TOO_HIGH                                0x0002
#define HVAC_PUMP_POWER_MISSING_PHASE                                    0x0004
#define HVAC_PUMP_SYSTEM_PRESSURE_TOO_LOW                                0x0008
#define HVAC_PUMP_SYSTEM_PRESSURE_TOO_HIGH                               0x0010
#define HVAC_PUMP_DRY_RUNNING                                            0x0020
#define HVAC_PUMP_MOTOR_TEMPERATURE_TOO_HIGH                             0x0040
#define HVAC_PUMP_MOTOR_HAS_FATAL_FAILURE                                0x0080
#define HVAC_PUMP_ELECTRONIC_TEMPERATURE_TOO_HIGH                        0x0100
#define HVAC_PUMP_BLOCKED                                                0x0200
#define HVAC_PUMP_SENSOR_FAILURE                                         0x0400
#define HVAC_PUMP_ELECTRONIC_NON_FATAL_FAILURE                           0x0800
#define HVAC_PUMP_ELECTRONIC_FATAL_FAILURE                               0x1000
#define HVAC_PUMP_GENERAL_FAULT                                          0x2000
  // Pump Unit Table attributes set
//#define ATTRID_HVAC_PUMP_POWER_UNIT                                      0x0030
/*** Power Unit attribute values ***/
//#define HVAC_PUMP_POWER_UNIT_POWER_IN_WATTS                              0x00
//#define HVAC_PUMP_POWER_UNIT_POWER_IN_KILOWATTS                          0x01

/**************************************************************************/
/***          HVAC: Thermostat Cluster Attributes                       ***/
/**************************************************************************/
  // Thermostat information attribute set
#define ATTRID_HVAC_THERMOSTAT_LOCAL_TEMPERATURE                         0x0000
#define ATTRID_HVAC_THERMOSTAT_OUTDOOR_TEMPERATURE                       0x0001
#define ATTRID_HVAC_THERMOSTAT_OCCUPANCY                                 0x0002
#define ATTRID_HVAC_THERMOSTAT_ABS_MIN_HEAT_SETPOINT_LIMIT               0x0003
#define ATTRID_HVAC_THERMOSTAT_ABS_MAX_HEAT_SETPOINT_LIMIT               0x0004
#define ATTRID_HVAC_THERMOSTAT_ABS_MIN_COOL_SETPOINT_LIMIT               0x0005
#define ATTRID_HVAC_THERMOSTAT_ABS_MAX_COOL_SETPOINT_LIMIT               0x0006
#define ATTRID_HVAC_THERMOSTAT_PI_COOLING_DEMAND                         0x0007
#define ATTRID_HVAC_THERMOSTAT_PI_HEATING_DEMAND                         0x0008
  
  // Thermostat settings attribute set
#define ATTRID_HVAC_THERMOSTAT_LOCAL_TEMP_CALIBRATION                    0x0010
#define ATTRID_HVAC_THERMOSTAT_OCCUPIED_COOLING_SETPOINT                 0x0011
#define ATTRID_HVAC_THERMOSTAT_OCCUPIED_HEATING_SETPOINT                 0x0012
#define ATTRID_HVAC_THERMOSTAT_UNOCCUPIED_COOLING_SETPOINT               0x0013
#define ATTRID_HVAC_THERMOSTAT_UNOCCUPIED_HEATING_SETPOINT               0x0014
#define ATTRID_HVAC_THERMOSTAT_MIN_HEAT_SETPOINT_LIMIT                   0x0015
#define ATTRID_HVAC_THERMOSTAT_MAX_HEAT_SETPOINT_LIMIT                   0x0016
#define ATTRID_HVAC_THERMOSTAT_MIN_COOL_SETPOINT_LIMIT                   0x0017
#define ATTRID_HVAC_THERMOSTAT_MAX_COOL_SETPOINT_LIMIT                   0x0018
#define ATTRID_HVAC_THERMOSTAT_MIN_SETPOINT_DEAD_BAND                    0x0019
#define ATTRID_HVAC_THERMOSTAT_REMOTE_SENSING                            0x001A
#define ATTRID_HVAC_THERMOSTAT_CTRL_SEQ_OF_OPER                          0x001B
#define ATTRID_HVAC_THERMOSTAT_SYSTEM_MODE                               0x001C
#define ATTRID_HVAC_THERMOSTAT_ALAM_MASK                                 0x001D
  /*** Remote Sensing attribute values ***/
#define HVAC_THERMOSTAT_REMOTE_SENSING_LOCAL_TEMPERATURE                 0x0001
#define HVAC_THERMOSTAT_REMOTE_SENSING_OUTDOOR_TEMPERATURE               0x0002
#define HVAC_THERMOSTAT_REMOTE_SENSING_OCCUPANCY                         0x0004
  /*** Control Sequence of Operation attribute values ***/
#define HVAC_THERMOSTAT_CTRL_SEQ_OF_OPER_COOLING_ONLY                    0x00
#define HVAC_THERMOSTAT_CTRL_SEQ_OF_OPER_COOLING_WITH_REHEAT             0x01
#define HVAC_THERMOSTAT_CTRL_SEQ_OF_OPER_HEATING_ONLY                    0x02
#define HVAC_THERMOSTAT_CTRL_SEQ_OF_OPER_HEATING_WITH_REHEAT             0x03
#define HVAC_THERMOSTAT_CTRL_SEQ_OF_OPER_COOLING_HEATING                 0x04
#define HVAC_THERMOSTAT_CTRL_SEQ_OF_OPER_COOLING_HEATING_WITH_REHEAT     0x05
  /*** System Mode attribute values ***/
#define HVAC_THERMOSTAT_SYSTEM_MODE_OFF                                  0x00
#define HVAC_THERMOSTAT_SYSTEM_MODE_AUTO                                 0x01 
#define HVAC_THERMOSTAT_SYSTEM_MODE_COOL                                 0x03
#define HVAC_THERMOSTAT_SYSTEM_MODE_HEAT                                 0x04
#define HVAC_THERMOSTAT_SYSTEM_MODE_EMERGENCY_HEATING                    0x05
#define HVAC_THERMOSTAT_SYSTEM_MODE_PRECOOLING                           0x06
#define HVAC_THERMOSTAT_SYSTEM_MODE_FAN_ONLY                             0x07
  /*** Alarm Mask attribute values ***/
#define HVAC_THERMOSTAT_INITIALIZATION_FAILURE                           0x01  
#define HVAC_THERMOSTAT_HARDWARE_FAILURE                                 0x02
#define HVAC_THERMOSTAT_SELF_CALIBRATION_FAILURE                         0x03
  
  // command IDs received by Server
#define COMMAND_THERMOSTAT_SETPOINT_RAISE_LOWER                          0x00
  
  // Mode field values for the setpoint raise/lower command
#define HVAC_THERMOSTAT_MODE_HEAT                                        0x00
#define HVAC_THERMOSTAT_MODE_COOL                                        0x01 
#define HVAC_THERMOSTAT_MODE_BOTH                                        0x02

/**************************************************************************/
/***          HVAC: Fan Control Cluster Attributes                      ***/
/**************************************************************************/
  // Fan Control attribute set
#define ATTRID_HVAC_FAN_CTRL_FAN_MODE                                    0x0000
#define ATTRID_HVAC_FAN_CTRL_FAN_SEQUENCE                                0x0001
  /*** Fan Mode attribute values ***/
#define HVAC_FAN_CTRL_FAN_MODE_OFF                                       0x00 
#define HVAC_FAN_CTRL_FAN_MODE_LOW                                       0x01
#define HVAC_FAN_CTRL_FAN_MODE_MEDIUM                                    0x02 
#define HVAC_FAN_CTRL_FAN_MODE_HIGH                                      0x03
#define HVAC_FAN_CTRL_FAN_MODE_ON                                        0x04
#define HVAC_FAN_CTRL_FAN_MODE_AUTO                                      0x05
#define HVAC_FAN_CTRL_FAN_MODE_SMART                                     0x06
  /*** Fan Mode Sequence attribute values ***/
#define HVAC_FAN_CTRL_FAN_MODE_SEQ_LOW_MED_HI                            0x00  
#define HVAC_FAN_CTRL_FAN_MODE_SEQ_LOW_HI                                0x01
#define HVAC_FAN_CTRL_FAN_MODE_SEQ_LOW_MED_HI_AUTO                       0x02
#define HVAC_FAN_CTRL_FAN_MODE_SEQ_LOW_HI_AUTO                           0x03 
#define HVAC_FAN_CTRL_FAN_MODE_SEQ_ON_AUTO                               0x04 

/**************************************************************************/
/***          HVAC: Dehumidifcation Control Cluster Attributes          ***/
/**************************************************************************/
  // Dehumidifcation Control Information attribute set
#define ATTRID_HVAC_DEHUMIDIFICATION_CTRL_RELATIVE_HUMIDITY              0x0000
#define ATTRID_HVAC_DEHUMIDIFICATION_CTRL_DEHUMID_COOLING                0x0001

  // Dehumidifcation Control Settings attribute set
#define ATTRID_HVAC_DEHUMIDIFICATION_CTRL_RHDEHUMID_SETPOINT             0x0010
#define ATTRID_HVAC_DEHUMIDIFICATION_CTRL_RELATIVE_HUMIDITY_MODE         0x0011
#define ATTRID_HVAC_DEHUMIDIFICATION_CTRL_DEHUMID_LOCKOUT                0x0012
#define ATTRID_HVAC_DEHUMIDIFICATION_CTRL_DEHUMID_HYSTERESIS             0x0013
#define ATTRID_HVAC_DEHUMIDIFICATION_CTRL_DEHUMID_MAX_COOL               0x0014
#define ATTRID_HVAC_DEHUMIDIFICATION_CTRL_RELATIVE_HUMIDITY_DISPLAY      0x0015
   /*** Relative Humidity Mode attribute values ***/
#define HVAC_DEHUMIDIFICATION_CTRL_RELATIVE_HUMIDITY_MEASURED_LOCALLY    0x00
#define HVAC_DEHUMIDIFICATION_CTRL_RELATIVE_HUMIDITY_UPDATED_OVER_NET    0x00
   /*** Dehumidification Lockout attribute values ***/
#define HVAC_DEHUMIDIFICATION_CTRL_DEHIMID_NOT_ALLOWED                   0x00
#define HVAC_DEHUMIDIFICATION_CTRL_DEHIMID_ALLOWED                       0x01
     /*** Relative Humidity Display attribute values ***/
#define HVAC_DEHUMIDIFICATION_CTRL_RELATIVE_HUMIDITY_NOT_DISPLAYED       0x00
#define HVAC_DEHUMIDIFICATION_CTRL_RELATIVE_HUMIDITY_DISPLAYED           0x01

/**************************************************************************/
/***          HVAC: Thermostat User Interface Config Cluster Attributes ***/
/**************************************************************************/
  // Thermostat User Interface Config attribute set
#define ATTRID_HVAC_THERMOSTAT_UI_CONFIG_TEMP_DISPLAY_MODE               0x0000
#define ATTRID_HVAC_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT                  0x0001 
  /*** Display Mode attribute values ***/
#define ATTRID_HVAC_THERMOSTAT_UI_CONFIG_DISPLAY_MODE_IN_C               0x00
#define ATTRID_HVAC_THERMOSTAT_UI_CONFIG_DISPLAY_MODE_IN_F               0x01 
  
  /*** Keypad Lockout attribute values ***/
#define ATTRID_HVAC_THERMOSTAT_UI_KEYPAD_NO_LOCOUT                       0x00
#define ATTRID_HVAC_THERMOSTAT_UI_KEYPAD_LEVEL1_LOCOUT                   0x01
#define ATTRID_HVAC_THERMOSTAT_UI_KEYPAD_LEVEL2_LOCOUT                   0x02
#define ATTRID_HVAC_THERMOSTAT_UI_KEYPAD_LEVEL3_LOCOUT                   0x03
#define ATTRID_HVAC_THERMOSTAT_UI_KEYPAD_LEVEL4_LOCOUT                   0x04
#define ATTRID_HVAC_THERMOSTAT_UI_KEYPAD_LEVEL5_LOCOUT                   0x05

/*********************************************************************
 * VARIABLES
 */


/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * TYPEDEFS
 */
  
  /*** ZCL HVAC Thermostat Cluster: Setpoint Raise/Lower Cmd payload ***/
typedef struct
{
  uint8 mode;  // which setpoint is to be configured
  int8 amount; // amount setpoint(s) are to be increased (or decreased) by, in steps of 0.1°C
} zclCmdThermostatSetpointRaiseLowerPayload_t;

// This callback is called to process a Setpoint Raise/Lower command
//  mode - which setpoint to be configured
//  amount  - the amount the setpoints are to be increased (or decreased) by, in steps of 0.1°C
typedef void (*zclHVAC_SetpointRaiseLower_t)( zclCmdThermostatSetpointRaiseLowerPayload_t *pCmd );

// Register Callbacks table entry - enter function pointers for callbacks that
// the application would like to receive
typedef struct			
{
  zclHVAC_SetpointRaiseLower_t pfnHVAC_SetpointRaiseLower;
} zclHVAC_AppCallbacks_t;


/*********************************************************************
 * VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Register for callbacks from this cluster library
 */
extern ZStatus_t zclHVAC_RegisterCmdCallbacks( uint8 endpoint, zclHVAC_AppCallbacks_t *callbacks );

/*
 * Send a Setpoint Raise/Lower Command
 */
ZStatus_t zclHVAC_SendSetpointRaiseLower( uint8 srcEP, afAddrType_t *dstAddr,
                                          uint8 mode, int8 amount,
                                          uint8 disableDefaultRsp, uint8 seqNum );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ZCL_HVAC_H */
