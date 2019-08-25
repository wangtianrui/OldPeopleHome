/**************************************************************************************************
  Filename:       zcl_general.h
  Revised:        $Date: 2011-04-29 10:18:57 -0700 (Fri, 29 Apr 2011) $
  Revision:       $Revision: 25820 $

  Description:    This file contains the ZCL General definitions.


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

#ifndef ZCL_GENERAL_H
#define ZCL_GENERAL_H

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
#define ZCL_SCENE_NAME_LEN                                16

/********************************/
/*** Basic Cluster Attributes ***/
/********************************/
  // Basic Device Information
#define ATTRID_BASIC_ZCL_VERSION                          0x0000
#define ATTRID_BASIC_APPL_VERSION                         0x0001
#define ATTRID_BASIC_STACK_VERSION                        0x0002
#define ATTRID_BASIC_HW_VERSION                           0x0003
#define ATTRID_BASIC_MANUFACTURER_NAME                    0x0004
#define ATTRID_BASIC_MODEL_ID                             0x0005
#define ATTRID_BASIC_DATE_CODE                            0x0006
#define ATTRID_BASIC_POWER_SOURCE                         0x0007
  // Basic Device Settings
#define ATTRID_BASIC_LOCATION_DESC                        0x0010
#define ATTRID_BASIC_PHYSICAL_ENV                         0x0011
#define ATTRID_BASIC_DEVICE_ENABLED                       0x0012
#define ATTRID_BASIC_ALARM_MASK                           0x0013

/*** Power Source Attribute values ***/
  // Bits b0-b6 represent the primary power source of the device
#define POWER_SOURCE_UNKNOWN                              0x00
#define POWER_SOURCE_MAINS_1_PHASE                        0x01
#define POWER_SOURCE_MAINS_3_PHASE                        0x02
#define POWER_SOURCE_BATTERY                              0x03
#define POWER_SOURCE_DC                                   0x04
#define POWER_SOURCE_EMERG_MAINS_CONST_PWR                0x05
#define POWER_SOURCE_EMERG_MAINS_XFER_SW                  0x06
  // Bit b7 indicates whether the device has a secondary power source in the
  // form of a battery backup

/*** Power Source Attribute bits  ***/
#define POWER_SOURCE_PRIMARY                              0x7F
#define POWER_SOURCE_SECONDARY                            0x80

/*** Physical Environment Attribute values ***/
#define PHY_UNSPECIFIED_ENV                               0x00
  // Specified per Profile 0x01-0x7F
#define PHY_UNKNOWN_ENV                                   0xFF

/*** Device Enable Attribute values ***/
#define DEVICE_DISABLED                                   0x00
#define DEVICE_ENABLED                                    0x01

/*** Alarm Mask Attribute bits ***/
#define ALARM_MASK_GEN_HW_FAULT                           0x01
#define ALARM_MASK_GEN_SW_FAULT                           0x02

/******************************/
/*** Basic Cluster Commands ***/
/******************************/
#define COMMAND_BASIC_RESET_FACT_DEFAULT                  0x00

/**********************************************/
/*** Power Configuration Cluster Attributes ***/
/**********************************************/
  // Mains Information
#define ATTRID_POWER_CFG_MAINS_VOLTAGE                    0x0000
#define ATTRID_POWER_CFG_MAINS_FREQUENCY                  0x0001
  // Mains Settings
#define ATTRID_POWER_CFG_MAINS_ALARM_MASK                 0x0010
#define ATTRID_POWER_CFG_MAINS_VOLT_MIN_THRES             0x0011
#define ATTRID_POWER_CFG_MAINS_VOLT_MAX_THRES             0x0012
#define ATTRID_POWER_CFG_MAINS_DWELL_TRIP_POINT           0x0013
  // Battery Information
#define ATTRID_POWER_CFG_BATTERY_VOLTAGE                  0x0020
  // Battery Settings
#define ATTRID_POWER_CFG_BAT_MANU                         0x0030
#define ATTRID_POWER_CFG_BAT_SIZE                         0x0031
#define ATTRID_POWER_CFG_BAT_AHR_RATING                   0x0032
#define ATTRID_POWER_CFG_BAT_QUANTITY                     0x0033
#define ATTRID_POWER_CFG_BAT_RATED_VOLTAGE                0x0034
#define ATTRID_POWER_CFG_BAT_ALARM_MASK                   0x0035
#define ATTRID_POWER_CFG_BAT_VOLT_MIN_THRES               0x0036

/*** Mains Alarm Mask Attribute bit ***/
#define MAINS_ALARM_MASK_VOLT_2_LOW                       0x01
#define MAINS_ALARM_MASK_VOLT_2_HI                        0x02

/*** Battery Size Attribute values ***/
#define BAT_SIZE_NO_BATTERY                               0x00
#define BAT_SIZE_BUILT_IN                                 0x01
#define BAT_SIZE_OTHER                                    0x02
#define BAT_SIZE_AA                                       0x03
#define BAT_SIZE_AAA                                      0x04
#define BAT_SIZE_C                                        0x05
#define BAT_SIZE_D                                        0x06
#define BAT_SIZE_UNKNOWN                                  0xFF

/*** Batter Alarm Mask Attribute bit ***/
#define BAT_ALARM_MASK_VOLT_2_LOW                         0x01

/********************************************/
/*** Power Configuration Cluster Commands ***/
/********************************************/
  // No cluster specific commands

/***********************************************************/
/*** Device Temperature Configuration Cluster Attributes ***/
/***********************************************************/
  // Device Temperature Information
#define ATTRID_DEV_TEMP_CURRENT                           0x0000
#define ATTRID_DEV_TEMP_MIN_EXPERIENCED                   0x0001
#define ATTRID_DEV_TEMP_MAX_EXPERIENCED                   0x0002
#define ATTRID_DEV_TEMP_OVER_TOTAL_DWELL                  0x0003
  // Device Temperature Settings
#define ATTRID_DEV_TEMP_ALARM_MASK                        0x0010
#define ATTRID_DEV_TEMP_LOW_THRES                         0x0011
#define ATTRID_DEV_TEMP_HI_THRES                          0x0012
#define ATTRID_DEV_TEMP_LOW_DWELL_TRIP_POINT              0x0013
#define ATTRID_DEV_TEMP_HI_DWELL_TRIP_POINT               0x0014

/*** Device Temp Alarm_Mask Attribute bits ***/
#define DEV_TEMP_ALARM_MASK_2_LOW                         0x01
#define DEV_TEMP_ALARM_MASK_2_HI                          0x02

/*********************************************************/
/*** Device Temperature Configuration Cluster Commands ***/
/*********************************************************/
  // No cluster specific commands

/***********************************/
/*** Identify Cluster Attributes ***/
/***********************************/
#define ATTRID_IDENTIFY_TIME                             0x0000

/*********************************/
/*** Identify Cluster Commands ***/
/*********************************/
#define COMMAND_IDENTIFY                                 0x00
#define COMMAND_IDENTIFY_QUERY                           0x01

#define COMMAND_IDENTIFY_QUERY_RSP                       0x00

/********************************/
/*** Group Cluster Attributes ***/
/********************************/
#define ATTRID_GROUP_NAME_SUPPORT                         0x0000

/******************************/
/*** Group Cluster Commands ***/
/******************************/
#define COMMAND_GROUP_ADD                                 0x00
#define COMMAND_GROUP_VIEW                                0x01
#define COMMAND_GROUP_GET_MEMBERSHIP                      0x02
#define COMMAND_GROUP_REMOVE                              0x03
#define COMMAND_GROUP_REMOVE_ALL                          0x04
#define COMMAND_GROUP_ADD_IF_IDENTIFYING                  0x05

#define COMMAND_GROUP_ADD_RSP                             0x00
#define COMMAND_GROUP_VIEW_RSP                            0x01
#define COMMAND_GROUP_GET_MEMBERSHIP_RSP                  0x02
#define COMMAND_GROUP_REMOVE_RSP                          0x03

/*********************************/
/*** Scenes Cluster Attributes ***/
/*********************************/
  // Scene Management Information
#define ATTRID_SCENES_COUNT                               0x0000
#define ATTRID_SCENES_CURRENT_SCENE                       0x0001
#define ATTRID_SCENES_CURRENT_GROUP                       0x0002
#define ATTRID_SCENES_SCENE_VALID                         0x0003
#define ATTRID_SCENES_NAME_SUPPORT                        0x0004
#define ATTRID_SCENES_LAST_CFG_BY                         0x0005

/*******************************/
/*** Scenes Cluster Commands ***/
/*******************************/
#define COMMAND_SCENE_ADD                                 0x00
#define COMMAND_SCENE_VIEW                                0x01
#define COMMAND_SCENE_REMOVE                              0x02
#define COMMAND_SCENE_REMOVE_ALL                          0x03
#define COMMAND_SCENE_STORE                               0x04
#define COMMAND_SCENE_RECALL                              0x05
#define COMMAND_SCENE_GET_MEMBERSHIP                      0x06

#define COMMAND_SCENE_ADD_RSP                             0x00
#define COMMAND_SCENE_VIEW_RSP                            0x01
#define COMMAND_SCENE_REMOVE_RSP                          0x02
#define COMMAND_SCENE_REMOVE_ALL_RSP                      0x03
#define COMMAND_SCENE_STORE_RSP                           0x04
#define COMMAND_SCENE_GET_MEMBERSHIP_RSP                  0x06

/*********************************/
/*** On/Off Cluster Attributes ***/
/*********************************/
#define ATTRID_ON_OFF                                     0x0000

/*******************************/
/*** On/Off Cluster Commands ***/
/*******************************/
#define COMMAND_OFF                                       0x00
#define COMMAND_ON                                        0x01
#define COMMAND_TOGGLE                                    0x02

/****************************************/
/*** On/Off Switch Cluster Attributes ***/
/****************************************/
  // Switch Information
#define ATTRID_ON_OFF_SWITCH_TYPE                         0x0000
  // Switch Settings
#define ATTRID_ON_OFF_SWITCH_ACTIONS                      0x0010

/*** On Off Switch Type attribute values ***/
#define ON_OFF_SWITCH_TYPE_TOGGLE                         0x00
#define ON_OFF_SWITCH_TYPE_MOMENTARY                      0x01

/*** On Off Switch Actions attribute values ***/
#define ON_OFF_SWITCH_ACTIONS_0                           0x00
#define ON_OFF_SWITCH_ACTIONS_1                           0x01
#define ON_OFF_SWITCH_ACTIONS_2                           0x02

/**************************************/
/*** On/Off Switch Cluster Commands ***/
/**************************************/
  // No cluster specific commands

/****************************************/
/*** Level Control Cluster Attributes ***/
/****************************************/
#define ATTRID_LEVEL_CURRENT_LEVEL                        0x0000
#define ATTRID_LEVEL_REMAINING_TIME                       0x0001
#define ATTRID_LEVEL_ON_OFF_TRANSITION_TIME               0x0010
#define ATTRID_LEVEL_ON_LEVEL                             0x0011

/**************************************/
/*** Level Control Cluster Commands ***/
/**************************************/
#define COMMAND_LEVEL_MOVE_TO_LEVEL                       0x00
#define COMMAND_LEVEL_MOVE                                0x01
#define COMMAND_LEVEL_STEP                                0x02
#define COMMAND_LEVEL_STOP                                0x03
#define COMMAND_LEVEL_MOVE_TO_LEVEL_WITH_ON_OFF           0x04
#define COMMAND_LEVEL_MOVE_WITH_ON_OFF                    0x05
#define COMMAND_LEVEL_STEP_WITH_ON_OFF                    0x06
#define COMMAND_LEVEL_STOP_WITH_ON_OFF                    0x07

/*** Level Control Move (Mode) Command values ***/
#define LEVEL_MOVE_UP                                     0x00
#define LEVEL_MOVE_DOWN                                   0x01

/*** Level Control Step (Mode) Command values ***/
#define LEVEL_STEP_UP                                     0x00
#define LEVEL_STEP_DOWN                                   0x01

/*********************************/
/*** Alarms Cluster Attributes ***/
/*********************************/
  // Alarm Information
#define ATTRID_ALARM_COUNT                                0x0000

/*******************************/
/*** Alarms Cluster Commands ***/
/*******************************/
#define COMMAND_ALARMS_RESET                              0x00
#define COMMAND_ALARMS_RESET_ALL                          0x01
#define COMMAND_ALARMS_GET                                0x02
#define COMMAND_ALARMS_RESET_LOG                          0x03

#define COMMAND_ALARMS_ALARM                              0x00
#define COMMAND_ALARMS_GET_RSP                            0x01

/*******************************/
/*** Time Cluster Attributes ***/
/*******************************/
#define ATTRID_TIME_TIME                                  0x00
#define ATTRID_TIME_STATUS                                0x01
#define ATTRID_TIME_ZONE                                  0x02
#define ATTRID_TIME_DST_START                             0x03
#define ATTRID_TIME_DST_END                               0x04
#define ATTRID_TIME_DST_SHIFT                             0x05
#define ATTRID_TIME_STANDARD_TIME                         0x06
#define ATTRID_TIME_LOCAL_TIME                            0x07
#define ATTRID_TIME_LAST_SET_TIME                         0x08
#define ATTRID_TIME_VALID_UNTIL_TIME                      0x09

/*** TimeStatus Attribute bits ***/
#define TIME_STATUS_MASTER                                0x01
#define TIME_STATUS_SYNCH                                 0x02
#define TIME_STATUS_MASTER_ZONE_DST                       0x04

/*****************************/
/*** Time Cluster Commands ***/
/*****************************/
  // No cluster specific commands

/***********************************/
/*** RSSI Location Cluster Attributes ***/
/***********************************/
  // Location Information
#define ATTRID_LOCATION_TYPE                              0x0000
#define ATTRID_LOCATION_METHOD                            0x0001
#define ATTRID_LOCATION_AGE                               0x0002
#define ATTRID_LOCATION_QUALITY_MEASURE                   0x0003
#define ATTRID_LOCATION_NUM_DEVICES                       0x0004

  // Location Settings
#define ATTRID_LOCATION_COORDINATE1                       0x0010
#define ATTRID_LOCATION_COORDINATE2                       0x0011
#define ATTRID_LOCATION_COORDINATE3                       0x0012
#define ATTRID_LOCATION_POWER                             0x0013
#define ATTRID_LOCATION_PATH_LOSS_EXPONENT                0x0014
#define ATTRID_LOCATION_REPORT_PERIOD                     0x0015
#define ATTRID_LOCATION_CALC_PERIOD                       0x0016
#define ATTRID_LOCATION_NUM_RSSI_MEASUREMENTS             0x0017

/*** Location Type attribute bits ***/
#define LOCATION_TYPE_ABSOLUTE                            0x01
#define LOCATION_TYPE_2_D                                 0x02
#define LOCATION_TYPE_COORDINATE_SYSTEM                   0x0C

/*** Location Method attribute values ***/
#define LOCATION_METHOD_LATERATION                        0x00
#define LOCATION_METHOD_SIGNPOSTING                       0x01
#define LOCATION_METHOD_RF_FINGER_PRINT                   0x02
#define LOCATION_METHOD_OUT_OF_BAND                       0x03

/*********************************/
/*** Location Cluster Commands ***/
/*********************************/
#define COMMAND_LOCATION_SET_ABSOLUTE                      0x00
#define COMMAND_LOCATION_SET_DEV_CFG                       0x01
#define COMMAND_LOCATION_GET_DEV_CFG                       0x02
#define COMMAND_LOCATION_GET_DATA                          0x03

#define COMMAND_LOCATION_DEV_CFG_RSP                       0x00
#define COMMAND_LOCATION_DATA_RSP                          0x01
#define COMMAND_LOCATION_DATA_NOTIF                        0x02
#define COMMAND_LOCATION_COMPACT_DATA_NOTIF                0x03
#define COMMAND_LOCATION_RSSI_PING                         0x04

/**********************************************************/
/*** Input, Output and Value (Basic) Cluster Attributes ***/
/**********************************************************/
#define ATTRID_IOV_BASIC_ACTIVE_TEXT                        0x0004
#define ATTRID_IOV_BASIC_STATE_TEXT                         0x000E
#define ATTRID_IOV_BASIC_DESCRIPTION                        0x001C
#define ATTRID_IOV_BASIC_INACTIVE_TEXT                      0x002E
#define ATTRID_IOV_BASIC_MAX_PRESENT_VALUE                  0x0041
#define ATTRID_IOV_BASIC_MIN_OFF_TIME                       0x0042
#define ATTRID_IOV_BASIC_MIM_ON_TIME                        0x0043
#define ATTRID_IOV_BASIC_MIN_PRESENT_VALUE                  0x0045
#define ATTRID_IOV_BASIC_NUM_OF_STATES                      0x004A
#define ATTRID_IOV_BASIC_OUT_OF_SERVICE                     0x0051
#define ATTRID_IOV_BASIC_POLARITY                           0x0054
#define ATTRID_IOV_BASIC_PRESENT_VALUE                      0x0055
#define ATTRID_IOV_BASIC_PRIORITY_ARRAY                     0x0057
#define ATTRID_IOV_BASIC_RELIABILITY                        0x0067
#define ATTRID_IOV_BASIC_RELINQUISH_DEFAULT                 0x0068
#define ATTRID_IOV_BASIC_RESOLUTION                         0x006A
#define ATTRID_IOV_BASIC_STATUS_FLAG                        0x006F
#define ATTRID_IOV_BASIC_ENGINEERING_UNITS                  0x0075
#define ATTRID_IOV_BASIC_APP_TYPE                           0x0100

/*** StatusFlags attribute bits ***/
#define STATUS_FLAGS_IN_ALARM                               0x01
#define STATUS_FLAGS_FAULT                                  0x02
#define STATUS_FLAGS_OVERRIDDEN                             0x04
#define STATUS_FLAGS_OUT_OF_SERVICE                         0x08

/*** Reliability attribute types ***/
#define RELIABILITY_NO_FAULT_DETECTED                       0x00
#define RELIABILITY_NO_SENSOR                               0x01
#define RELIABILITY_OVER_RANGE                              0x02
#define RELIABILITY_UNDER_RANGE                             0x03
#define RELIABILITY_OPEN_LOOP                               0x04
#define RELIABILITY_SHORTED_LOOP                            0x05
#define RELIABILITY_NO_OUTPUT                               0x06
#define RELIABILITY_UNRELIABLE_OTHER                        0x07
#define RELIABILITY_PROCESS_ERROR                           0x08
#define RELIABILITY_MULTI_STATE_FAULT                       0x09
#define RELIABILITY_CONFIG_ERROR                            0x0A

/*** EngineeringUnits attribute values ***/
// Values 0x0000 to 0x00fe are reserved for the list of engineering units with
// corresponding values specified in Clause 21 of the BACnet standard.

#define ENGINEERING_UNITS_OTHER                             0x00FF

// Values 0x0100 to 0xffff are available for proprietary use.

/*** Polarity attribute values ***/
#define POLARITY_NORMAL                                     0x00
#define POLARITY_REVERSE                                    0x01

/*** ApplicationType attribute bits ***/
// ApplicationType is subdivided into Group, Type and an Index number.

// Application Group = Bits 24 - 31. An indication of the cluster this
// attribute is part of.
#define APP_GROUP                                           0xFF000000

// Application Type = Bits 16 - 23. For Analog clusters, the physical
// quantity that the Present Value attribute of the cluster represents.
// For Binary and Multistate clusters, the application usage domain.
#define APP_TYPE                                            0x00FF0000

// Application Index = Bits 0 - 15. The specific application usage of
// the cluster
#define APP_INDEX                                           0x0000FFFF

/*** Application Groups ***/
#define APP_GROUP_AI                                        0x00 // Analog Input
#define APP_GROUP_AO                                        0x01 // Analog Output
#define APP_GROUP_AV                                        0x02 // Analog Value
#define APP_GROUP_BI                                        0x03 // Binary Input
#define APP_GROUP_BO                                        0x04 // Binary Output
#define APP_GROUP_BV                                        0x05 // Binary Value
#define APP_GROUP_MI                                        0x0D // Multistate Input
#define APP_GROUP_MO                                        0x0E // Multistate Output
#define APP_GROUP_MV                                        0x13 // Multistate Value

/*** Application Types ***/

// Analog Input (AI) Types:
//   Group = 0x00.
//   Types = 0x00 - 0x0E.
//   Types 0x0F to 0xFE are reserved, Type = 0xFF indicates other.
#define APP_TYPE_AI_TEMP_C                                  0x00 // Temperature in degrees C
#define APP_TYPE_AI_HUMIDITY_PERCENT                        0x01 // Relative humidity in %
#define APP_TYPE_AI_PRESSURE_PASCAL                         0x02 // Pressure in Pascal
#define APP_TYPE_AI_FLOW_LIT_SEC                            0x03 // Flow in liters/second
#define APP_TYPE_AI_PERCENT                                 0x04 // Percentage %
#define APP_TYPE_AI_PARTS_PER_MIL                           0x05 // Parts per Million PPM
#define APP_TYPE_AI_ROTATION_SPEED                          0x06 // Rotational Speed in RPM
#define APP_TYPE_AI_CURRENT_AMPS                            0x07 // Current in Amps
#define APP_TYPE_AI_FREQUENCY_HZ                            0x08 // Frequency in Hz
#define APP_TYPE_AI_PWR_WATTS                               0x09 // Power in Watts
#define APP_TYPE_AI_PWR_KW                                  0x0A // Power in kW
#define APP_TYPE_AI_ENERGY_KWH                              0x0B // Energy in kWH
#define APP_TYPE_AI_COUNT                                   0x0C // Count - Unitless
#define APP_TYPE_AI_ENTHALPY_KJ_KG                          0x0D // Enthalpy in KJoules/Kg
#define APP_TYPE_AI_TIME_SEC                                0x0E // Time in Seconds

// Analog Output (AO) Types:
//   Group = 0x01.
//   Types = 0x00 - 0x0E.
//   Types 0x0F to 0xFE are reserved, Type = 0xFF indicates other.
#define APP_TYPE_AO_TEMP_C                                  0x00 // Temperature in degrees C
#define APP_TYPE_AO_HUMIDITY_PERCENT                        0x01 // Relative Humidity in %
#define APP_TYPE_AO_PRESSURE_PASCAL                         0x02 // Pressure in Pascal
#define APP_TYPE_AO_FLOW_LIT_SEC                            0x03 // Flow in liters/second
#define APP_TYPE_AO_PERCENT                                 0x04 // Percentage %
#define APP_TYPE_AO_PARTS_PER_MIL                           0x05 // Parts per Million PPM
#define APP_TYPE_AO_ROTATION_SPEED                          0x06 // Rotational Speed in RPM
#define APP_TYPE_AO_CURRENT_AMPS                            0x07 // Current in Amps
#define APP_TYPE_AO_FREQUENCY_HZ                            0x08 // Frequency in Hz
#define APP_TYPE_AO_PWR_WATTS                               0x09 // Power in Watts
#define APP_TYPE_AO_PWR_KW                                  0x0A // Power in kW
#define APP_TYPE_AO_ENERGY_KWH                              0x0B // Energy in kWH
#define APP_TYPE_AO_COUNT                                   0x0C // Count - Unitless
#define APP_TYPE_AO_ENTHALPY_KJ_KG                          0x0D // Enthalpy in KJoules/Kg
#define APP_TYPE_AO_TIME_SEC                                0x0E // Time in Seconds

// Analog Value (AV) Types:
//   Group = 0x02.
//   Types = 0x00 - 0x03.
//   Types 0x04 to 0xFE are reserved, Type = 0xFF indicates other.
#define APP_TYPE_AV_TEMP_C                                  0x00 // Temperature in Degrees C
#define APP_TYPE_AV_AREA_SQ_METER                           0x01 // Area in Square Metres
#define APP_TYPE_AV_MULTIPLIER_NUM                          0x02 // Multiplier - Number
#define APP_TYPE_AV_FLOW_LIT_SEC                            0x03 // Flow in Litres/Second

// Binary Input (BI) Types:
//   Group = 0x03.
//   Types = 0x00 - 0x01.
//   Types 0x02 to 0xFE are reserved, Type = 0xFF indicates other.
//   Present Value = 0 represents False, Off, Normal
//   Present Value = 1 represents True, On, Alarm
#define APP_TYPE_BI_DOMAIN_HVAC                             0x00 // Application Domain HVAC
#define APP_TYPE_BI_DOMAIN_SEC                              0x01 // Application Domain Security

// Binary Output (BO) Types:
//   Group = 0x04.
//   Types = 0x00 - 0x01.
//   Types 0x02 to 0xFE are reserved, Type = 0xFF indicates other.
//   Present Value = 0 represents False, Off, Normal
//   Present Value = 1 represents True, On, Alarm
#define APP_TYPE_BO_DOMAIN_HVAC                             0x00 // Application Domain HVAC
#define APP_TYPE_BO_DOMAIN_SEC                              0x02 // Application Domain Security

// Binary Value (BV) Types:
//   Group = 0x05.
//   Type = 0x00.
//   Types 0x01 to 0xFE are reserved, Type = 0xFF indicates other.
//   Present Value = 0 represents False, Off, Normal
//   Present Value = 1 represents True, On, Alarm
#define APP_TYPE_BV                                         0x00 // Type = 0x00

// Multistate Input (MI) Types:
//   Group = 0x0D.
//   Type = 0x00.
//   Types 0x01 to 0xFE are reserved, Type = 0xFF indicates other.
#define APP_TYPE_MI_DOMAIN_HVAC                             0x00 // Application Domain HVAC

// Multistate Output (MO) Types:
//   Group = 0x0E.
//   Type = 0x00.
//   Types 0x01 to 0xFE are reserved, Type = 0xFF indicates other.
#define APP_TYPE_MO_DOMAIN_HVAC                             0x00 // Application Domain HVAC

// Multistate Value (MV) Types:
//   Group = 0x13.
//   Type = 0x00.
//   Types 0x01 to 0xFE are reserved, Type = 0xFF indicates other.
#define APP_TYPE_MV_DOMAIN_HVAC                             0x00 // Application Domain HVAC

/*** Application Indexes ***/

// Analog Input (AI) Indexes
//   Group = 0x00.

// AI Temperature in degrees C Indexes:
//   Type = 0x00.
//   Indexes = 0x0000 - 0x003C.
//   Indexed 0x003D - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AI_TEMP_2_PIPE_ENTER_WATER                0x00 // 2 Pipe Entering Water Temperature AI

// AI Relative humidity in % Indexes:
//   Type = 0x01.
//   Indexes = 0x0000 - 0x0008.
//   Indexed 0x0009 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AI_HUMIDITY_DISCHARGE                     0x00 // Discharge Humidity AI

// AI Pressure in Pascal Indexes:
//   Type = 0x02.
//   Indexes = 0x0000 - 0x001E.
//   Indexed 0x001F - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AI_PRESSURE_BOIL_PUMP_DIFF                0x00 // Boiler Pump Differential Pressure AI

// AI Flow in liters/second Indexes:
//   Type = 0x03.
//   Indexes = 0x0000 - 0x0015.
//   Indexed 0x0016 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AI_FLOW_CHILLED_WATER                     0x00 // Chilled Water Flow AI

// AI Percentage % Indexes:
//   Type = 0x04.
//   Index = 0x0000.
//   Indexed 0x0001 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AI_PERCENT_CHILLER_FULL_LOAD_AMP          0x00 // Chiller % Full Load Amperage AI

// AI Parts per Million PPM Indexes:
//   Type = 0x05.
//   Indexes = 0x0000 - 0x0001.
//   Indexed 0x0002 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AI_PARTS_RETURN_CO2                       0x00 // Return Carbon Dioxide AI

// AI Rotational Speed in RPM Indexes:
//   Type = 0x06.
//   Indexes = 0x0000 - 0x0007.
//   Indexed 0x0008 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AI_ROTATION_EXHAUST_FAN_REMOTE            0x00 // Exhaust Fan Remote Speed AI

// AI Current in Amps Indexes:
//   Type = 0x07.
//   Index = 0x0000.
//   Indexed 0x0001 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AI_CURRENT_AMPS_CHILLER                   0x00 // Chiller Amps AI

// AI Frequency in Hz Indexes:
//   Type = 0x08.
//   Index = 0x0000.
//   Indexed 0x0001 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AI_FREQUENCY_SPEED_DRIVE_OUTPUT           0x00 // Variable Speed Drive Output Frequency AI

// AI Power in Watts Indexes:
//   Type = 0x09.
//   Index = 0x0000.
//   Indexed 0x0001 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AI_PWR_WATTS_CONSUMPTION                  0x00 // Power Consumption AI

// AI Power in kW Indexes:
//   Type = 0x0A.
//   Indexes = 0x0000 - 0x0001.
//   Indexed 0x0002 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AI_PWR_KW_ABSOLUTE                        0x00 // Absolute Power AI

// AI Energy in kWH Indexes:
//   Type = 0x0B.
//   Index = 0x0000.
//   Indexed 0x0001 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AI_ENERGY_KWH_SPEED_DRIVE                 0x00 // Variable Speed Drive Kilowatt Hours AI

// AI Count - Unitless Indexes:
//   Type = 0x0C.
//   Index = 0x0000.
//   Indexed 0x0001 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AI_COUNT                                  0x00 // Count

// AI Enthalpy in KJoules/Kg Indexes:
//   Type = 0x0D.
//   Indexes = 0x0000 - 0x0002.
//   Indexed 0x0003 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AI_ENTHALPY_OUTDOOR_AIR                   0x00 // Outdoor Air Enthalpy AI

// AI Time in Seconds Indexes:
//   Type = 0x0E.
//   Index = 0x0000.
//   Indexed 0x0001 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AI_TIME_RELATIVE                          0x00 // Relative time AI


// Analog Output (AO) types
//   Group = 0x01.

// AO Temperature in degrees C Indexes:
//   Type = 0x00.
//   Indexes = 0x0000 - 0x0009.
//   Indexed 0x000A - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AO_TEMP_BOILER                            0x00 // Boiler AO

// AO Relative Humidity in % Indexes:
//   Type = 0x01.
//   Indexes = 0x0000 - 0x0001.
//   Indexed 0x0002 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AO_HUMIDITY_HUMIDIFICATION                0x00 // Humidification AO

// AO Pressure in Pascal Indexes:
//   Type = 0x02.
//   Indexed 0x0000 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.

// AO Flow in liters/second Indexes:
//   Type = 0x03.
//   Indexed 0x0000 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.

// AO Percentage % Indexes:
//   Type = 0x04.
//   Indexes = 0x0000 - 0x002D.
//   Indexed 0x002E - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AO_PERCENT_FACE_BYPASS_DAMPER             0x00 // Face & Bypass Damper AO

// AO Parts per Million PPM Indexes:
//   Type = 0x05.
//   Index = 0x0000.
//   Indexed 0x0001 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AO_PARTS_SPACE_CO2_LIMIT                  0x00 // Space Carbon Dioxide limit AO

// AO Rotational Speed in RPM Indexes:
//   Type = 0x06.
//   Indexes = 0x0000 - 0x0004.
//   Indexed 0x0005 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AO_ROTATION_EXHAUST_FAN_SPEED             0x00 // Exhaust Fan Speed AO

// AO Current in Amps Indexes:
//   Type = 0x07.
//   Indexed 0x0000 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.

// AO Frequency in Hz Indexes:
//   Type = 0x08.
//   Indexed 0x0000 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.

// AO Power in Watts Indexes:
//   Type = 0x09.
//   Indexed 0x0000 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.

// AO Power in kW Indexes:
//   Type = 0x0A.
//   Indexed 0x0000 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.

// AO Energy in kWH Indexes:
//   Type = 0x0B.
//   Indexed 0x0000 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.

// AO Count - Unitless Indexes:
//   Type = 0x0C.
//   Indexed 0x0000 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.

// AO Enthalpy in KJoules/Kg Indexes:
//   Type = 0x0D.
//   Indexed 0x0000 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.

// AO Time in Seconds Indexes:
//   Type = 0x0E.
//   Index = 0x0000.
//   Indexed 0x0001 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AO_TIME_RELATIVE                          0x00 // Relative time AO


// Analog Value (AV) types
//   Group = 0x02.

// AV Temperature in Degrees C Indexes:
//   Type = 0x00.
//   Indexes = 0x0000 - 0x000F.
//   Indexed 0x0010 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AV_TEMP_SETPOINT_OFFSET                   0x00 // Setpoint Offset AV

// AV Area in Square Metres Indexes:
//   Type = 0x01.
//   Index = 0x0000.
//   Indexed 0x0001 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AV_AREA_DUCT                              0x00 // Duct Area AV

// AV Multiplier - Number Indexes:
//   Type = 0x02.
//   Index = 0x0000.
//   Indexed 0x0001 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AV_MULTIPLIER_GAIN                        0x00 // Gain multiplier AV

// AV Flow in Litres/Second Indexes:
//   Type = 0x03.
//   Indexes = 0x0000 - 0x0005.
//   Indexed 0x0006 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_AV_FLOW_MIN_AIR                           0x00 // Minimum Air Flow AV


// Binary Input (BI) types
//   Group = 0x03.

// BI Application Domain HVAC Indexes:
//   Type = 0x00.
//   Indexes = 0x0000 - 0x0094.
//   Indexed 0x0095 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_BI_DOMAIN_HVAC_2_PIPE_PUMP_STAT           0x00 // 2 Pipe Pump Status BI

// BI Application Domain Security Indexes:
//   Type = 0x01.
//   Indexes = 0x0000 - 0x0008.
//   Indexed 0x0009 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_BI_DOMAIN_SEC_GLASS_BREAK_DETECT          0x00 // Glass Breakage Detection


// Binary Output (BO) types
//   Group = 0x04.

// BO Application Domain HVAC Indexes:
//   Type = 0x00.
//   Indexes = 0x0000 - 0x0076.
//   Indexed 0x0078 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_BO_DOMAIN_HVAC_2_PIPE_CIR_PUMP            0x00 // 2 Pipe Circulation Pump BO

// BO Application Domain Security Indexes:
//   Type = 0x02.
//   Indexes = 0x0000 - 0x0003.
//   Indexed 0x0004 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_BO_DOMAIN_SEC_ARM_DISARM_CMD              0x00 // Arm Disarm Command BO


// Binary Value (BV) types
//   Group = 0x05.

// BV Type Indexes:
//   Type = 0x00.
//   Indexed 0x0000 - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.

// Multistate Input (MI) types
//   Group = 0x0D.

// MI Application Domain HVAC Indexes:
//   Type = 0x00.
//   Indexes = 0x0000 - 0x000B.
//   Indexed 0x000C - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_MI_DOMAIN_HVAC_OFF_ON_AUTO                0x00 // Off, On, Auto


// Multistate Output (MO)types
//   Group = 0x0E.

// MO Application Domain HVAC Indexes:
//   Type = 0x00.
//   Indexes = 0x0000 - 0x000B.
//   Indexed 0x000C - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_MO_DOMAIN_HVAC_OFF_ON_AUTO                0x00 // Off, On, Auto


// Multistate Value (MV) types
//   Group = 0x13.

// MV Application Domain HVAC Indexes:
//   Type = 0x00.
//   Indexes = 0x0000 - 0x000B.
//   Indexed 0x000C - 0x01FF are reserved, 0x0200 - 0xFFFE are Vendor
//   defined, Index = 0xFFFF indicates other.
#define APP_INDEX_MV_DOMAIN_HVAC_OFF_ON_AUTO                0x00 // Off, On, Auto


// The maximum number of characters to allow in a scene's name
// remember that the first byte is the length
#define ZCL_GEN_SCENE_NAME_LEN                           16

// The maximum length of the scene extension field:
//   2 + 1 + 1 for On/Off cluster (onOff attibute)
//   2 + 1 + 1 for Level Control cluster (currentLevel attribute)
//   2 + 1 + 4 for Color Control cluster (currentX/currentY attributes)
//   2 + 1 + 1 for Door Lock cluster (Lock State attribute)
//   2 + 1 + 2 for Window Covering cluster (LiftPercentage/TiltPercentage attributes)
#define ZCL_GEN_SCENE_EXT_LEN                            24

// The maximum number of entries in the Scene table
#define ZCL_GEN_MAX_SCENES                               16

/*********************************************************************
 * TYPEDEFS
 */

// The format of a Scene Table Entry
typedef struct
{
  uint16 groupID;                   // The group ID for which this scene applies
  uint8 ID;                         // Scene ID
  uint16 transTime;                 // Time to take to transition to this scene
  uint8 name[ZCL_GEN_SCENE_NAME_LEN]; // Scene name
  uint8 extLen;                     // Length of extension fields
  uint8 extField[ZCL_GEN_SCENE_EXT_LEN]; // Extension fields
} zclGeneral_Scene_t;

// The format of an Alarm Table entry
typedef struct
{
  uint8 code;             // Identifying code for the cause of the alarm
  uint16 clusterID;       // The id of the cluster whose attribute generated this alarm
  uint32 timeStamp;       // The time at which the alarm occured
} zclGeneral_Alarm_t;

/*** RSSI Location Cluster Data Types ***/
// Set Absolute Location Command format
typedef struct
{
  int16   coordinate1;
  int16   coordinate2;
  int16   coordinate3;
  int16   power;
  uint16  pathLossExponent;
} zclLocationAbsolute_t;

// Set Device Configuration Command format
typedef struct
{
  int16   power;
  uint16  pathLossExponent;
  uint16  calcPeriod;
  uint8   numMeasurements;
  uint16  reportPeriod;
} zclLocationDevCfg_t;

// Get Location Data Command format
typedef struct
{
  unsigned int absOnly:1;       // Absolute Only
  unsigned int recalc:1;        // Re-calculate
  unsigned int brdcastIndic:1;  // Broadcast Indicator
  unsigned int brdcastRsp:1;    // Broadcast Response
  unsigned int compactRsp:1;    // Compact Response
  unsigned int reserved:3;      // Reserved for future use
} locationbits_t;

typedef union
{
  locationbits_t  locBits;
  uint8           locByte;
} location_t;

typedef struct
{
  location_t   bitmap;
  uint8        numResponses;
  uint8        targetAddr[8];
  // shorthand access
#define absoluteOnly      bitmap.locBits.absOnly
#define recalculate       bitmap.locBits.recalc
#define brdcastIndicator  bitmap.locBits.brdcastIndic
#define brdcastResponse   bitmap.locBits.brdcastRsp
#define compactResponse   bitmap.locBits.compactRsp
} zclLocationGetData_t;

// Device Configuration Response Command format
typedef struct
{
  uint8               status;
  zclLocationDevCfg_t data;
} zclLocationDevCfgRsp_t;

// Calculated Location Data type
typedef struct
{
  uint8   locationMethod;
  uint8   qualityMeasure;
  uint16  locationAge;
} zclLocationCalculated_t;

// Location Data Type
typedef struct
{
  uint8                    type;
  zclLocationAbsolute_t    absLoc;
  zclLocationCalculated_t  calcLoc;
} zclLocationData_t;

// Location Data Response Command format
typedef struct
{
  uint8              status;
  zclLocationData_t  data;
} zclLocationDataRsp_t;

/*** Structures used for callback functions ***/
typedef struct
{
  afAddrType_t *srcAddr;     // requestor's address
  uint16       identifyTime; // number of seconds the device will continue to identify itself
} zclIdentify_t;

typedef struct
{
  afAddrType_t *srcAddr; // requestor's address
  uint16       timeout;  // number of seconds the device will continue to identify itself
} zclIdentifyQueryRsp_t;

typedef struct
{
  uint8  level;          // new level to move to
  uint16 transitionTime; // time to take to move to the new level (in seconds)
  uint8  withOnOff;      // with On/off command
} zclLCMoveToLevel_t;

typedef struct
{
  uint8 moveMode;  // move mode which is either LEVEL_MOVE_STOP, LEVEL_MOVE_UP,
                   // LEVEL_MOVE_ON_AND_UP, LEVEL_MOVE_DOWN, or LEVEL_MOVE_DOWN_AND_OFF
  uint8 rate;      // rate of movement in steps per second
  uint8 withOnOff; // with On/off command
} zclLCMove_t;

typedef struct
{
  uint8  stepMode;       // step mode which is either LEVEL_STEP_UP, LEVEL_STEP_ON_AND_UP,
                         // LEVEL_STEP_DOWN, or LEVEL_STEP_DOWN_AND_OFF
  uint8  amount;         // number of levels to step
  uint16 transitionTime; // time, in 1/10ths of a second, to take to perform the step
  uint8  withOnOff;      // with On/off command
} zclLCStep_t;

typedef struct
{
  afAddrType_t *srcAddr; // requestor's address
  uint8        cmdID;    // which group message - COMMAND_GROUP_ADD_RSP, COMMAND_GROUP_VIEW_RSP,
                         // COMMAND_GROUP_REMOVE_RSP or COMMAND_GROUP_GET_MEMBERSHIP_RSP
  uint8        status;   // GROUP_STATUS_SUCCESS, GROUP_STATUS_TABLE_FULL,
                         // GROUP_STATUS_ALREADY_IN_TABLE, or GROUP_STATUS_NOT_IN_TABLE. Not
                         // valid for COMMAND_GROUP_GET_MEMBERSHIP_RSP
  uint8        grpCnt;   // number of groups contained in group list
  uint16       *grpList; // what group IDs the action was performed on
  uint8        capacity; // remaining capacity of group table
  uint8        *grpName; // only valid for COMMAND_GROUP_VIEW_RSP
} zclGroupRsp_t;

typedef struct
{
   afAddrType_t       *srcAddr; // requestor's address
   zclGeneral_Scene_t *scene;   // pointer to the scene structure
} zclSceneReq_t;

typedef struct
{
  afAddrType_t       *srcAddr;   // requestor's address
  uint8              cmdID;      // which response - COMMAND_SCENE_ADD_RSP, COMMAND_SCENE_VIEW_RSP,
                                 // COMMAND_SCENE_REMOVE_RSP, COMMAND_SCENE_REMOVE_ALL_RSP,
                                 // COMMAND_SCENE_STORE_RSP or COMMAND_SCENE_GET_MEMBERSHIPSHIP_RSP
  uint8              status;     // response status
  uint8              sceneCnt;   // number of scenes in the scene list (only valid for
                                 // COMMAND_SCENE_GET_MEMBERSHIPSHIP_RSP)
  uint8              *sceneList; // list of scene IDs (only valid for COMMAND_SCENE_GET_MEMBERSHIPSHIP_RSP)
  uint8              capacity;   // remaining capacity of the scene table (only valid for
                                 // COMMAND_SCENE_GET_MEMBERSHIPSHIP_RSP)
  zclGeneral_Scene_t *scene;     // pointer to the scene structure
} zclSceneRsp_t;

typedef struct
{
  afAddrType_t *srcAddr;  // requestor's address
  uint8        cmdID;     // COMMAND_ALARMS_ALARM or COMMAND_ALARMS_GET_RSP
  uint8        status;    // response status (only applicable to COMMAND_ALARMS_GET_RSP)
  uint8        alarmCode; // response status (only applicable to COMMAND_ALARMS_GET_RSP)
  uint16       clusterID; // the id of the cluster whose attribute generated this alarm
  uint32       timeStamp; // the time at which the alarm occurred (only applicable to
                          // COMMAND_ALARMS_GET_RSP)
} zclAlarm_t;

typedef struct
{
  afAddrType_t            *srcAddr;  // requestor's address
  uint8                   cmdID;     // COMMAND_LOCATION_SET_ABSOLUTE, COMMAND_LOCATION_SET_DEV_CFG,
                                     // COMMAND_LOCATION_GET_DEV_CFG or COMMAND_LOCATION_GET_DATA
  union
  {
    zclLocationAbsolute_t absLoc;    // Absolute Location info (only valid for COMMAND_LOCATION_SET_ABSOLUTE)
    zclLocationGetData_t  loc;       // Get Location info (only valid for COMMAND_LOCATION_GET_DATA)
    zclLocationDevCfg_t   devCfg;    // Device Config info (only valid for COMMAND_LOCATION_SET_DEV_CFG)
    uint8                 *ieeeAddr; // Device's IEEE Addr (only valid for COMMAND_LOCATION_GET_DEV_CFG)
  } un;
  uint8                   seqNum;    // Sequence number received with the message  (only valid for GET commands)
} zclLocation_t;

typedef struct
{
  afAddrType_t             *srcAddr;     // requestor's address
  uint8                    cmdID;        // COMMAND_LOCATION_DEV_CFG_RSP, COMMAND_LOCATION_DATA_RSP,
                                         // COMMAND_LOCATION_DATA_NOTIF, COMMAND_LOCATION_COMPACT_DATA_NOTIF
                                         // or COMMAND_LOCATION_RSSI_PING
  union
  {
    zclLocationDataRsp_t   loc;          // the Location Data Response command (applicable to Data Response/Notification)
    zclLocationDevCfgRsp_t devCfg;       // the Device Configuration Response command (only applicable to
                                         // COMMAND_LOCATION_DEV_CFG_RSP)
    uint8                  locationType; // location type (only applicable to COMMAND_LOCATION_RSSI_PING)
  } un;
} zclLocationRsp_t;

// This callback is called to process an incoming Reset to Factory Defaults
// command. On receipt of this command, the device resets all the attributes
// of all its clusters to their factory defaults.
typedef void (*zclGCB_BasicReset_t)( void );

// This callback is called to process an incoming Identify command.
//   srcAddr - requestor's address
//   identifyTime - number of seconds the device will continue to identify itself
typedef void (*zclGCB_Identify_t)( zclIdentify_t *pCmd );

// This callback is called to process an incoming Identify Query Response command.
//   srcAddr - requestor's address
//   timeout - number of seconds the device will continue to identify itself
typedef void (*zclGCB_IdentifyQueryRsp_t)( zclIdentifyQueryRsp_t *pRsp );

// This callback is called to process an incoming On, Off or Toggle command.
//   cmd - received command, which will be either COMMAND_ON, COMMAND_OFF
//         or COMMAND_TOGGLE.
typedef void (*zclGCB_OnOff_t)( uint8 cmd );

// This callback is called to process a Level Control - Move to Level command
//   level - new level to move to
//   tansitionTime - time to take to move to the new level (in seconds)
//   withOnOff - with On/off command
typedef void (*zclGCB_LevelControlMoveToLevel_t)( zclLCMoveToLevel_t *pCmd );

// This callback is called to process a Level Control - Move command
//   moveMode - move mode which is either LEVEL_MOVE_STOP, LEVEL_MOVE_UP,
//              LEVEL_MOVE_ON_AND_UP, LEVEL_MOVE_DOWN, or LEVEL_MOVE_DOWN_AND_OFF
//   rate - rate of movement in steps per second.
//   withOnOff - with On/off command
typedef void (*zclGCB_LevelControlMove_t)( zclLCMove_t *pCmd );

// This callback is called to process a Level Control - Step command
//   stepMode - step mode which is either LEVEL_STEP_UP, LEVEL_STEP_ON_AND_UP,
//              LEVEL_STEP_DOWN, or LEVEL_STEP_DOWN_AND_OFF
//   amount - number of levels to step
//   transitionTime - time, in 1/10ths of a second, to take to perform the step
//   withOnOff - with On/off command
typedef void (*zclGCB_LevelControlStep_t)( zclLCStep_t *pCmd );

// This callback is called to process a Level Control - Stop command
typedef void (*zclGCB_LevelControlStop_t)( void );

// This callback is called to process an received Group Response message.
// This means that this app sent the request message.
//   srcAddr - requestor's address
//   cmdID - which group message - COMMAND_GROUP_ADD_RSP, COMMAND_GROUP_VIEW_RSP,
//           COMMAND_GROUP_REMOVE_RSP or COMMAND_GROUP_GET_MEMBERSHIP_RSP
//   status - GROUP_STATUS_SUCCESS, GROUP_STATUS_TABLE_FULL,
//            GROUP_STATUS_ALREADY_IN_TABLE, or GROUP_STATUS_NOT_IN_TABLE. Not
//            valid for COMMAND_GROUP_GET_MEMBERSHIP_RSP
//   grpCnt - number of groups contained in group list
//   grpList - what group IDs the action was performed on
//   capacity - remaining capacity of group table
//   grpName - only valid for COMMAND_GROUP_VIEW_RSP
typedef void (*zclGCB_GroupRsp_t)( zclGroupRsp_t *pRsp );

// This callback is called to process an incoming Scene Store request.
// The app will fill in the "extField" with what is needed to restore its
// current settings.
//   srcAddr - requestor's address
//   scene - pointer to the scene structure
//   returns TRUE if extField is filled in, FALSE if not used.
typedef uint8 (*zclGCB_SceneStoreReq_t)( zclSceneReq_t *pReq );

// This callback is called to process an incoming Scene Recall request
// The app will use what's in the "extField" to restore to these settings.
//   srcAddr - requestor's address
//   scene - pointer to the scene structure
typedef void (*zclGCB_SceneRecallReq_t)( zclSceneReq_t *pReq );

// This callback is called to process an incoming Scene responses. This means
// that this app sent the request for this response.
//   srcAddr - requestor's address
//   cmdID - which response - COMMAND_SCENE_ADD_RSP, COMMAND_SCENE_VIEW_RSP,
//            COMMAND_SCENE_REMOVE_RSP, COMMAND_SCENE_REMOVE_ALL_RSP,
//            COMMAND_SCENE_STORE_RSP or COMMAND_SCENE_GET_MEMBERSHIPSHIP_RSP
//   status - response status
//   sceneCnt - number of scenes in the scene list (only valid for
//              COMMAND_SCENE_GET_MEMBERSHIPSHIP_RSP)
//   sceneList - list of scene IDs (only valid for COMMAND_SCENE_GET_MEMBERSHIPSHIP_RSP)
//   capacity - remaining capacity of the scene table (only valid for
//              COMMAND_SCENE_GET_MEMBERSHIPSHIP_RSP)
//   scene - pointer to the scene structure

typedef void (*zclGCB_SceneRsp_t)( zclSceneRsp_t *pRsp );

// This callback is called to process an incoming Alarm request or response command.
//   srcAddr - requestor's address
//   cmdID - COMMAND_ALARMS_ALARM or COMMAND_ALARMS_GET_RSP
//   status - response status (only applicable to COMMAND_ALARMS_GET_RSP)
//   alarmCode - identifying code for the cause of the alarm
//   clusterID - the id of the cluster whose attribute generated this alarm
//   timeStamp - the time at which the alarm occurred (only applicable to
//               COMMAND_ALARMS_GET_RSP)
typedef void (*zclGCB_Alarm_t)( zclAlarm_t *pAlarm );

// This callback is called to to process an incoming RSSI Location command.
//   srcAddr - requestor's address
//   cmdID - COMMAND_LOCATION_SET_ABSOLUTE, COMMAND_LOCATION_SET_DEV_CFG,
//           COMMAND_LOCATION_GET_DEV_CFG or COMMAND_LOCATION_GET_DATA
//   absLoc - Absolute Location info (only valid for COMMAND_LOCATION_SET_ABSOLUTE)
//   loc - Get Location info (only valid for COMMAND_LOCATION_GET_DATA)
//   devCfg - Device Config info (only valid for COMMAND_LOCATION_SET_DEV_CFG)
//   ieeeAddr - Device's IEEE Addr (only valid for COMMAND_LOCATION_GET_DEV_CFG)
//   seqNum - Sequence number received with the message  (only valid for GET commands)
typedef void (*zclGCB_Location_t)( zclLocation_t *pCmd );

// This callback is called to process an incoming RSSI Location response command.
// This means  that this app sent the request for this response.
//   srcAddr - requestor's address
//   cmdID - COMMAND_LOCATION_DEV_CFG_RSP, COMMAND_LOCATION_DATA_RSP,
//           COMMAND_LOCATION_DATA_NOTIF, COMMAND_LOCATION_COMPACT_DATA_NOTIF
//           or COMMAND_LOCATION_RSSI_PING
//   locRsp - the Location Data Response command (applicable to Data Response/Notification)
//   devCfgRsp - the Device Configuration Response command (only applicable to
//               COMMAND_LOCATION_DEV_CFG_RSP)
//   locationType - location type (only applicable to COMMAND_LOCATION_RSSI_PING)
typedef void (*zclGCB_LocationRsp_t)( zclLocationRsp_t *pRsp );

// Register Callbacks table entry - enter function pointers for callbacks that
// the application would like to receive
typedef struct
{
  zclGCB_BasicReset_t               pfnBasicReset;                // Basic Cluster Reset command
  zclGCB_Identify_t                 pfnIdentify;                  // Identify command
  zclGCB_IdentifyQueryRsp_t         pfnIdentifyQueryRsp;          // Identify Query Response command
  zclGCB_OnOff_t                    pfnOnOff;                     // On/Off cluster commands
  zclGCB_LevelControlMoveToLevel_t  pfnLevelControlMoveToLevel;   // Level Control Move to Level command
  zclGCB_LevelControlMove_t         pfnLevelControlMove;          // Level Control Move command
  zclGCB_LevelControlStep_t         pfnLevelControlStep;          // Level Control Step command
  zclGCB_LevelControlStop_t         pfnLevelControlStop;          // Level Control Stop command
  zclGCB_GroupRsp_t                 pfnGroupRsp;                  // Group Response commands
  zclGCB_SceneStoreReq_t            pfnSceneStoreReq;             // Scene Store Request command
  zclGCB_SceneRecallReq_t           pfnSceneRecallReq;            // Scene Recall Request command
  zclGCB_SceneRsp_t                 pfnSceneRsp;                  // Scene Response command
  zclGCB_Alarm_t                    pfnAlarm;                     // Alarm (Response) commands
  zclGCB_Location_t                 pfnLocation;                  // RSSI Location command
  zclGCB_LocationRsp_t              pfnLocationRsp;               // RSSI Location Response command
} zclGeneral_AppCallbacks_t;

/*********************************************************************
 * FUNCTION MACROS
 */
#ifdef ZCL_BASIC
/*
 *  Send a Reset to Factory Defaults Command - COMMAND_BASIC_RESET_FACTORY_DEFAULTS
 *  Use like:
 *      ZStatus_t zclGeneral_SendBasicResetFactoryDefaults( uint16 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendBasicResetFactoryDefaults(a,b,c,d) zcl_SendCommand( (a), (b), ZCL_CLUSTER_ID_GEN_BASIC, COMMAND_BASIC_RESET_FACT_DEFAULT, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, (c), 0, (d), 0, NULL )
#endif // ZCL_BASIC

#ifdef ZCL_IDENTIFY
/*
 * Send a Identify Query command
 *  Use like:
 *      ZStatus_t zclGeneral_SendIdentifyQuery( uint8 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendIdentifyQuery(a,b,c,d) zcl_SendCommand( (a), (b), ZCL_CLUSTER_ID_GEN_IDENTIFY, COMMAND_IDENTIFY_QUERY, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, (c), 0, (d), 0, NULL )
#endif // ZCL_IDENTIFY

#ifdef ZCL_GROUPS
/*
 *  Send a Group Add Command
 *  Use like:
 *      ZStatus_t zclGeneral_SendGroupAdd( uint8 srcEP, afAddrType_t *dstAddr, uint16 groupID, uint8 *groupName, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendGroupAdd(a,b,c,d,e,f) zclGeneral_SendAddGroupRequest( (a), (b), COMMAND_GROUP_ADD, (c), (d), (e), (f) )

/*
 *  Send a Group View Command
 *  Use like:
 *      ZStatus_t zclGeneral_SendGroupView( uint8 srcEP, afAddrType_t *dstAddr, uint16 groupID, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendGroupView(a,b,c,d,e) zclGeneral_SendGroupRequest( (a), (b), COMMAND_GROUP_VIEW, (c), (d), (e) )

/*
 *  Send a Group Get Membership Command
 *  Use like:
 *      ZStatus_t zclGeneral_SendGroupGetMembership( uint8 srcEP, afAddrType_t *dstAddr, uint8 grpCnt, uint16 *grpList, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define  zclGeneral_SendGroupGetMembership(a,b,c,d,e,f) zclGeneral_SendGroupGetMembershipRequest( (a), (b), COMMAND_GROUP_GET_MEMBERSHIP, FALSE, ZCL_FRAME_CLIENT_SERVER_DIR, 0, (c), (d), (e), (f) )

/*
 *  Send a Group Remove Command
 *  Use like:
 *      ZStatus_t zclGeneral_SendGroupRemove( uint8 srcEP, afAddrType_t *dstAddr, uint16 groupID, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendGroupRemove(a,b,c,d,e) zclGeneral_SendGroupRequest( (a), (b), COMMAND_GROUP_REMOVE, (c), (d), (e) )

/*
 *  Send a Group Remove ALL Command - COMMAND_GROUP_REMOVE_ALL
 *  Use like:
 *      ZStatus_t zclGeneral_SendGroupRemoveAll( uint16 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendGroupRemoveAll(a,b,c,d) zcl_SendCommand( (a), (b), ZCL_CLUSTER_ID_GEN_GROUPS, COMMAND_GROUP_REMOVE_ALL, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, (c), 0, (d), 0, NULL )

/*
 *  Send a Group Add If Identifying Command
 *  Use like:
 *      ZStatus_t zclGeneral_SendGroupAddIfIdentifying( uint8 srcEP, afAddrType_t *dstAddr, uint16 groupID, uint8 *groupName, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendGroupAddIfIdentifying(a,b,c,d,e,f) zclGeneral_SendAddGroupRequest( (a), (b), COMMAND_GROUP_ADD_IF_IDENTIFYING, (c), (d), (e), (f) )

/*
 *  Send a Group Add Response Command
 *  Use like:
 *      ZStatus_t zclGeneral_SendGroupAddResponse( uint8 srcEP, afAddrType_t *dstAddr, uint8 status, uint16 groupID, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendGroupAddResponse(a,b,c,d,e,f) zclGeneral_SendGroupResponse( (a), (b), COMMAND_GROUP_ADD_RSP, (c), (d), (e), (f) )

/*
 *  Send a Group Get Membership Response Command
 *  Use like:
 *      ZStatus_t zclGeneral_SendGroupGetMembershipResponse( uint8 srcEP, afAddrType_t *dstAddr, uint8 capacity, uint8 grpCnt, uint16 *grpList, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define  zclGeneral_SendGroupGetMembershipResponse(a,b,c,d,e,f,g) zclGeneral_SendGroupGetMembershipRequest( (a), (b), COMMAND_GROUP_GET_MEMBERSHIP_RSP, TRUE, ZCL_FRAME_SERVER_CLIENT_DIR, (c), (d), (e), (f), (g) )

/*
 *  Send a Group Remove Response Command
 *  Use like:
 *      ZStatus_t zclGeneral_SendGroupRemoveResponse( uint8 srcEP, afAddrType_t *dstAddr, uint8 status, uint16 groupID, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendGroupRemoveResponse(a,b,c,d,e,f) zclGeneral_SendGroupResponse( (a), (b), COMMAND_GROUP_REMOVE_RSP, (c), (d), (e), (f) )
#endif // ZCL_GROUPS

#ifdef ZCL_SCENES
/*
 *  Send a Scene View Command
 *  Use like:
 *      ZStatus_t zclGeneral_SendSceneView( uint8 srcEP, afAddrType_t *dstAddr, uint16 groupID, uint8 sceneID, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendSceneView(a,b,c,d,e,f) zclGeneral_SendSceneRequest( (a), (b), COMMAND_SCENE_VIEW, (c), (d), (e), (f) )

/*
 *  Send a Scene Remove Command
 *  Use like:
 *      ZStatus_t zclGeneral_SendSceneRemove( uint8 srcEP, afAddrType_t *dstAddr, uint16 groupID, uint8 sceneID, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendSceneRemove(a,b,c,d,e,f) zclGeneral_SendSceneRequest( (a), (b), COMMAND_SCENE_REMOVE, (c), (d), (e), (f) )

/*
 *  Send a Scene Store Command
 *  Use like:
 *      ZStatus_t zclGeneral_SendSceneStore( uint8 srcEP, afAddrType_t *dstAddr, uint16 groupID, uint8 sceneID, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendSceneStore(a,b,c,d,e,f) zclGeneral_SendSceneRequest( (a), (b), COMMAND_SCENE_STORE, (c), (d), (e), (f) )

/*
 *  Send a Scene Recall Command
 *  Use like:
 *      ZStatus_t zclGeneral_SendSceneRecall( uint8 srcEP, afAddrType_t *dstAddr, uint16 groupID, uint8 sceneID, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendSceneRecall(a,b,c,d,e,f) zclGeneral_SendSceneRequest( (a), (b), COMMAND_SCENE_RECALL, (c), (d), (e), (f) )

/*
 *  Send a Scene Remove ALL Command - COMMAND_SCENE_REMOVE_ALL
 *  Use like:
 *      ZStatus_t zclGeneral_SendSceneRemoveAll( uint16 srcEP, afAddrType_t *dstAddr, uint16 groupID, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendSceneRemoveAll(a,b,c,d,e) zclGeneral_SendSceneRequest( (a), (b), COMMAND_SCENE_REMOVE_ALL, (c), 0, (d), (e) )

/*
 *  Send a Scene Get Membership Command - COMMAND_SCENE_GET_MEMBERSHIPSHIP
 *  Use like:
 *      ZStatus_t zclGeneral_SendSceneGetMembership( uint16 srcEP, afAddrType_t *dstAddr, uint16 groupID, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendSceneGetMembership(a,b,c,d,e) zclGeneral_SendSceneRequest( (a), (b), COMMAND_SCENE_GET_MEMBERSHIP, (c), 0, (d), (e) )

/*
 *  Send a Scene Remove ALL Response Command - COMMAND_SCENE_ADD_RSP
 *  Use like:
 *      ZStatus_t zclGeneral_SendSceneAddResponse( uint16 srcEP, afAddrType_t *dstAddr, uint8 status, uint16 groupID, uint8 sceneID, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendSceneAddResponse(a,b,c,d,e,f,g) zclGeneral_SendSceneResponse( (a), (b), COMMAND_SCENE_ADD_RSP, (c), (d), (e), (f), (g) )

/*
 *  Send a Scene Remove Response Command - COMMAND_SCENE_REMOVE_RSP
 *  Use like:
 *      ZStatus_t zclGeneral_SendSceneRemoveResponse( uint16 srcEP, afAddrType_t *dstAddr, uint8 status, uint16 groupID, uint8 sceneID, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendSceneRemoveResponse(a,b,c,d,e,f,g) zclGeneral_SendSceneResponse( (a), (b), COMMAND_SCENE_REMOVE_RSP, (c), (d), (e), (f), (g) )

/*
 *  Send a Scene Remove All Response Command - COMMAND_SCENE_REMOVE_ALL_RSP
 *  Use like:
 *      ZStatus_t zclGeneral_SendSceneRemoveAllResponse( uint16 srcEP, afAddrType_t *dstAddr, uint8 status, uint16 groupID, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendSceneRemoveAllResponse(a,b,c,d,e,f) zclGeneral_SendSceneResponse( (a), (b), COMMAND_SCENE_REMOVE_ALL_RSP, (c), (d), 0, (e), (f) )

/*
 *  Send a Scene Remove ALL Response Command - COMMAND_SCENE_STORE_RSP
 *  Use like:
 *      ZStatus_t zclGeneral_SendSceneStoreResponse( uint16 srcEP, afAddrType_t *dstAddr, uint8 status, uint16 groupID, uint8 sceneID, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendSceneStoreResponse(a,b,c,d,e,f,g) zclGeneral_SendSceneResponse( (a), (b), COMMAND_SCENE_STORE_RSP, (c), (d), (e), (f), (g) )
#endif // ZCL_SCENES

#ifdef ZCL_ON_OFF
/*
 *  Send an On Off Command - COMMAND_ONOFF_OFF
 *  Use like:
 *      ZStatus_t zclGeneral_SendOnOff_CmdOff( uint16 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendOnOff_CmdOff(a,b,c,d) zcl_SendCommand( (a), (b), ZCL_CLUSTER_ID_GEN_ON_OFF, COMMAND_OFF, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, (c), 0, (d), 0, NULL )

/*
 *  Send an On Off Command - COMMAND_ONOFF_ON
 *  Use like:
 *      ZStatus_t zclGeneral_SendOnOff_CmdOn( uint16 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendOnOff_CmdOn(a,b,c,d) zcl_SendCommand( (a), (b), ZCL_CLUSTER_ID_GEN_ON_OFF, COMMAND_ON, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, (c), 0, (d), 0, NULL )

/*
 *  Send an On Off Command - COMMAND_ONOFF_TOGGLE
 *  Use like:
 *      ZStatus_t zclGeneral_SendOnOff_CmdToggle( uint16 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendOnOff_CmdToggle(a,b,c,d) zcl_SendCommand( (a), (b), ZCL_CLUSTER_ID_GEN_ON_OFF, COMMAND_TOGGLE, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, (c), 0, (d), 0, NULL )
#endif // ZCL_ON_OFF

#ifdef ZCL_LEVEL_CTRL
/*
 *  Send a Level Control Move to Level Command - COMMAND_LEVEL_MOVE_TO_LEVEL
 *  Use like:
 *      ZStatus_t zclGeneral_SendLevelControlMoveToLevel( uint16 srcEP, afAddrType_t *dstAddr, uint8 level, uint16 transTime, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendLevelControlMoveToLevel(a,b,c,d,e,f) zclGeneral_SendLevelControlMoveToLevelRequest( (a), (b), COMMAND_LEVEL_MOVE_TO_LEVEL, (c), (d), (e) ,(f) )

/*
 * Send a Level Control Move Command - COMMAND_LEVEL_MOVE
 *  Use like:
 *      ZStatus_t zclGeneral_SendLevelControlMoveRequest( uint8 srcEP, afAddrType_t *dstAddr, uint8 moveMode, uint8 rate, uint8 disableDefaultRsp, uint8 seqNum )
 */
#define zclGeneral_SendLevelControlMove(a,b,c,d,e,f) zclGeneral_SendLevelControlMoveRequest( (a), (b), COMMAND_LEVEL_MOVE, (c), (d), (e), (f) )


/*
 * Send out a Level Control Step Command - COMMAND_LEVEL_STEP
 *  Use like:
 *      ZStatus_t zclGeneral_SendLevelControlStep( uint8 srcEP, afAddrType_t *dstAddr, uint8 stepMode, uint8 stepSize, uint16 transTime, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendLevelControlStep(a,b,c,d,e,f,g) zclGeneral_SendLevelControlStepRequest( (a), (b), COMMAND_LEVEL_STEP, (c), (d), (e), (f), (g) )

/*
 * Send out a Level Control Stop Command - COMMAND_LEVEL_STOP
 *  Use like:
 *      ZStatus_t zclGeneral_SendLevelControlStop( uint8 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendLevelControlStop(a,b,c,d) zcl_SendCommand( (a), (b), ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL, COMMAND_LEVEL_STOP, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, (c), 0, (d), 0, NULL )

/*
 *  Send a Level Control Move to Level with On/Off Command - COMMAND_LEVEL_MOVE_TO_LEVEL_WITH_ON_OFF
 *  Use like:
 *      ZStatus_t zclGeneral_SendLevelControlMoveToLevelWithOnOff( uint16 srcEP, afAddrType_t *dstAddr, uint8 level, uint16 transTime, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendLevelControlMoveToLevelWithOnOff(a,b,c,d,e,f) zclGeneral_SendLevelControlMoveToLevelRequest( (a), (b), COMMAND_LEVEL_MOVE_TO_LEVEL_WITH_ON_OFF, (c), (d), (e) ,(f) )

/*
 * Send a Level Control Move with On/Off Command - COMMAND_LEVEL_MOVE_WITH_ON_OFF
 *  Use like:
 *      ZStatus_t zclGeneral_SendLevelControlMoveWithOnOff( uint8 srcEP, afAddrType_t *dstAddr, uint8 moveMode, uint8 rate, uint8 disableDefaultRsp, uint8 seqNum )
 */
#define zclGeneral_SendLevelControlMoveWithOnOff(a,b,c,d,e,f) zclGeneral_SendLevelControlMoveRequest( (a), (b), COMMAND_LEVEL_MOVE_WITH_ON_OFF, (c), (d), (e), (f) )


/*
 * Send out a Level Control Step with On/Off Command - COMMAND_LEVEL_STEP_WITH_ON_OFF
 *  Use like:
 *      ZStatus_t zclGeneral_SendLevelControlStepWithOnOff( uint8 srcEP, afAddrType_t *dstAddr, uint8 stepMode, uint8 stepSize, uint16 transTime, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendLevelControlStepWithOnOff(a,b,c,d,e,f,g) zclGeneral_SendLevelControlStepRequest( (a), (b), COMMAND_LEVEL_STEP_WITH_ON_OFF, (c), (d), (e), (f), (g) )

/*
 * Send out a Level Control Stop with On/Off Command - COMMAND_LEVEL_STOP_WITH_ON_OFF
 *  Use like:
 *      ZStatus_t zclGeneral_SendLevelControlStopWithOnOff( uint8 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendLevelControlStopWithOnOff(a,b,c,d) zcl_SendCommand( (a), (b), ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL, COMMAND_LEVEL_STOP_WITH_ON_OFF, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, (c), 0, (d), 0, NULL )
#endif // ZCL_LEVEL_CTRL

#ifdef ZCL_ALARMS
/*
 *  Send an Alarm Reset  Command - COMMAND_ALARMS_RESET
 *  Use like:
 *      ZStatus_t zclGeneral_SendAlarmReset( uint16 srcEP, afAddrType_t *dstAddr, uint8 alarmCode, uint16 clusterID, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendAlarmReset(a,b,c,d,e,f) zclGeneral_SendAlarmRequest( (a), (b), COMMAND_ALARMS_RESET, (c), (d), (e), (f) )

/*
 *  Send an Alarm Reset ALL Command - COMMAND_ALARMS_RESET_ALL
 *  Use like:
 *      ZStatus_t zclGeneral_SendAlarmResetAll( uint16 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendAlarmResetAll(a,b,c,d) zcl_SendCommand( (a), (b), ZCL_CLUSTER_ID_GEN_ALARMS, COMMAND_ALARMS_RESET_ALL, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, (c), 0, (d), 0, NULL )


/*
 *  Send an Alarm Get Command - COMMAND_ALARMS_GET
 *  Use like:
 *      ZStatus_t zclGeneral_SendAlarmGet uint16 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendAlarmGet(a,b,c,d) zcl_SendCommand( (a), (b), ZCL_CLUSTER_ID_GEN_ALARMS, COMMAND_ALARMS_GET, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, (c), 0, (d), 0, NULL )

/*
 *  Send an Alarm Reset Log Command - COMMAND_ALARMS_RESET_LOG
 *  Use like:
 *      ZStatus_t zclGeneral_SendAlarmResetLog( uint16 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendAlarmResetLog(a,b,c,d) zcl_SendCommand( (a), (b), ZCL_CLUSTER_ID_GEN_ALARMS, COMMAND_ALARMS_RESET_LOG, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, (c), 0, (d), 0, NULL )

/*
 *  Send an Alarm Command - COMMAND_ALARMS_ALARM
 *  Use like:
 *      ZStatus_t zclGeneral_SendAlarm( uint16 srcEP, afAddrType_t *dstAddr, uint8 alarmCode, uint16 clusterID, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendAlarm(a,b,c,d,e,f) zclGeneral_SendAlarmRequest( (a), (b), COMMAND_ALARMS_ALARM, (c), (d), (e), (f) )
#endif // ZCL_ALARMS

#ifdef ZCL_LOCATION
/*
 *  Send a Location Data Response Command - COMMAND_LOCATION_DATA_RSP
 *  Use like:
 *      ZStatus_t zclGeneral_SendLocationDataResponse( uint16 srcEP, afAddrType_t *dstAddr, zclLocationDataRsp_t *locData, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendLocationDataResponse(a,b,c,d,e) zclGeneral_SendLocationData( (a), (b), COMMAND_LOCATION_DATA_RSP, ((c)->status), (&((c)->data)), (d), (e) )

/*
 *  Send a Location Data Notification Command - COMMAND_LOCATION_DATA_NOTIFICATION
 *  Use like:
 *      ZStatus_t zclGeneral_SendLocationDataNotif( uint16 srcEP, afAddrType_t *dstAddr, zclLocationData_t *locData, uint8 seqNum );
 */
#define zclGeneral_SendLocationDataNotif(a,b,c,d) zclGeneral_SendLocationData( (a), (b), COMMAND_LOCATION_DATA_NOTIF, 0, (c), (d) )

/*
 *  Send a Location Data Compact Notification Command - COMMAND_LOCATION_COMPACT_DATA_NOTIFICATION
 *  Use like:
 *      ZStatus_t zclGeneral_SendLocationDataCompactNotif( uint16 srcEP, afAddrType_t *dstAddr, zclLocationData_t *locData, uint8 seqNum );
 */
#define zclGeneral_SendLocationDataCompactNotif(a,b,c,d) zclGeneral_SendLocationData( (a), (b), COMMAND_LOCATION_DATA_COMPACT_NOTIF, 0, (c), (d) )

/*
 *  Send an RSSI Ping Command - COMMAND_LOCATION_RSSI_PING
 *  Use like:
 *      ZStatus_t zclGeneral_SendRSSIPing( uint16 srcEP, afAddrType_t *dstAddr, uint8 locationType, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclGeneral_SendRSSIPing(a,b,c,d,e) zcl_SendCommand( (a), (b), ZCL_CLUSTER_ID_GEN_LOCATION, COMMAND_LOCATION_RSSI_PING, TRUE, ZCL_FRAME_SERVER_CLIENT_DIR, (d), 0, (e), 1, (c) )
#endif // ZCL_LOCATION

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Register for callbacks from this cluster library
 */
extern ZStatus_t zclGeneral_RegisterCmdCallbacks( uint8 endpoint, zclGeneral_AppCallbacks_t *callbacks );

#ifdef ZCL_LEVEL_CTRL
/*
 * Call to send out a Level Control Move to Level Request
 *      cmd - Move or Move with On/Off
 *      level - what level to move to
 *      transitionTime - how long to take to get to the level (in seconds).
 */
extern ZStatus_t zclGeneral_SendLevelControlMoveToLevelRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                                         uint8 cmd, uint8 level, uint16 transTime,
                                                         uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Call to send out a Level Control Move Request
 *      cmd - Step or Step with On/Off
 *      moveMode - LEVEL_MOVE_UP or
 *                 LEVEL_MOVE_DOWN
 *      rate - number of steps to take per second
 */
extern ZStatus_t zclGeneral_SendLevelControlMoveRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                                  uint8 cmd, uint8 moveMode, uint8 rate,
                                                  uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Call to send out a Level Control Step Request
 *      cmd - Step or Step with On/Off
 *      stepMode - LEVEL_STEP_UP or
 *                 LEVEL_STEP_DOWN
 *      amount - number of levels to step
 *      transitionTime - time to take to perform a single step
 */
extern ZStatus_t zclGeneral_SendLevelControlStepRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                uint8 cmd, uint8 stepMode, uint8 stepSize, uint16 transTime,
                                uint8 disableDefaultRsp, uint8 seqNum );
#endif // ZCL_LEVEL_CTRL

#ifdef ZCL_GROUPS
/*
 * Send Group Response (not Group View Response)
 *  - Use MACROS instead:
 *         zclGeneral_SendGroupAddResponse or zclGeneral_SendGroupRemoveResponse
 */
extern ZStatus_t zclGeneral_SendGroupResponse( uint8 srcEP, afAddrType_t *dstAddr,
                                               uint8 cmd, uint8 status, uint16 groupID,
                                               uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Call to send Group Response Command
 */
extern ZStatus_t zclGeneral_SendGroupViewResponse( uint8 srcEP, afAddrType_t *dstAddr,
                                                   uint8 status, aps_Group_t *grp,
                                                   uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Call to send Group Membership Command
 */
extern ZStatus_t zclGeneral_SendGroupGetMembershipRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                     uint8 cmd, uint8 rspCmd, uint8 direction, uint8 capacity,
                                     uint8 grpCnt, uint16 *grpList, uint8 disableDefaultRsp, uint8 seqNum );
#endif // ZCL_GROUPS

#ifdef ZCL_SCENES
/*
 * Add a scene for an endpoint
 */
extern ZStatus_t zclGeneral_AddScene( uint8 endpoint, zclGeneral_Scene_t *scene );

/*
 * Find a scene with endpoint and sceneID
 */
extern zclGeneral_Scene_t *zclGeneral_FindScene( uint8 endpoint, uint16 groupID, uint8 sceneID );

/*
 * Get all the scenes with groupID
 */
extern uint8 zclGeneral_FindAllScenesForGroup( uint8 endpoint, uint16 groupID, uint8 *sceneList );

/*
 * Remove a scene with endpoint and sceneID
 */
extern uint8 zclGeneral_RemoveScene( uint8 endpoint, uint16 groupID, uint8 sceneID );

/*
 * Remove all scenes for an endpoint
 */
extern void zclGeneral_RemoveAllScenes( uint8 endpoint, uint16 groupID );

/*
 * Count the number of scenes for an endpoint
 */
extern uint8 zclGeneral_CountScenes( uint8 endpoint );

/*
 * Count the number of scenes
 */
extern uint8 zclGeneral_CountAllScenes( void );

/*
 * Read callback function for the Scene Count attribute.
 */
extern ZStatus_t zclGeneral_ReadSceneCountCB( uint16 clusterId, uint16 attrId, 
                                              uint8 oper, uint8 *pValue, uint16 *pLen );
/*
 * Add Scene Request message
 */
extern ZStatus_t zclGeneral_SendAddScene( uint8 srcEP, afAddrType_t *dstAddr,
                                          zclGeneral_Scene_t *scene,
                                          uint8 disableDefaultRsp, uint8 seqNum );
/*
 * Send a Scene command (request) - not Scene Add
 */
extern ZStatus_t zclGeneral_SendSceneRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                              uint8 cmd, uint16 groupID, uint8 sceneID,
                                              uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Send Scene response messages for either COMMAND_SCENE_ADD_RSP,
 *         COMMAND_SCENE_REMOVE_RSP or COMMAND_SCENE_STORE_RSP
 */
extern ZStatus_t zclGeneral_SendSceneResponse( uint8 srcEP, afAddrType_t *dstAddr,
                                          uint8 cmd, uint8 status, uint16 groupID,
                                          uint8 sceneID, uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Send Scene View response message
 */
extern ZStatus_t zclGeneral_SendSceneViewResponse( uint8 srcEP, afAddrType_t *dstAddr,
                                                   uint8 status, zclGeneral_Scene_t *scene,
                                                   uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Send Scene Get Membership response message
 */
extern ZStatus_t zclGeneral_SendSceneGetMembershipResponse( uint8 srcEP, afAddrType_t *dstAddr,
                              uint8 sceneStatus, uint8 capacity, uint8 sceneCnt, uint8 *sceneList,
                              uint16 groupID, uint8 disableDefaultRsp, uint8 seqNum );
#endif // ZCL_SCENES

#ifdef ZCL_GROUPS
/*
 * Send a Group command (request) - not Group Add or Remove All
 */
extern ZStatus_t zclGeneral_SendGroupRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                              uint8 cmd, uint16 groupID,
                                              uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Send a Group Add command (request)
 *       groupName - pointer to Group Name.  This is a Zigbee
 *          string data type, so the first byte is the length of the
 *          name (in bytes), then the name.
 */
extern ZStatus_t zclGeneral_SendAddGroupRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                        uint8 cmd, uint16 groupID, uint8 *groupName,
                                        uint8 disableDefaultRsp, uint8 seqNum );
#endif // ZCL_GROUPS

#ifdef ZCL_IDENTIFY
/*
 * Send a Identify message
 */
extern ZStatus_t zclGeneral_SendIdentify( uint8 srcEP, afAddrType_t *dstAddr,
                               uint16 identifyTime, uint8 disableDefaultRsp, uint8 seqNum );
/*
 * Send a Identify Query Response message
 */
extern ZStatus_t zclGeneral_SendIdentifyQueryResponse( uint8 srcEP, afAddrType_t *dstAddr,
                                    uint16 timeout, uint8 disableDefaultRsp, uint8 seqNum );
#endif // ZCL_IDENTIFY

#ifdef ZCL_ALARMS
/*
 * Send out an Alarm Request Command
 */
extern ZStatus_t zclGeneral_SendAlarmRequest( uint8 srcEP, afAddrType_t *dstAddr,
                                              uint8 cmd, uint8 alarmCode, uint16 clusterID,
                                              uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Send out an Alarm Get Response Command
 */
extern ZStatus_t zclGeneral_SendAlarmGetRespnose( uint8 srcEP, afAddrType_t *dstAddr,
                                     uint8 status, uint8 alarmCode, uint16 clusterID,
                                     uint32 timeStamp, uint8 disableDefaultRsp, uint8 seqNum );
#endif // ZCL_ALARMS

#ifdef ZCL_LOCATION
/*
 * Send a Set Absolute Location message
 */
extern ZStatus_t zclGeneral_SendLocationSetAbsolute( uint8 srcEP, afAddrType_t *dstAddr,
                                                     zclLocationAbsolute_t *absLoc,
                                                     uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Send a Set Device Configuration message
 */
extern ZStatus_t zclGeneral_SendLocationSetDevCfg( uint8 srcEP, afAddrType_t *dstAddr,
                                                   zclLocationDevCfg_t *devCfg,
                                                   uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Send a Get Device Configuration message
 */
extern ZStatus_t zclGeneral_SendLocationGetDevCfg( uint8 srcEP, afAddrType_t *dstAddr,
                             uint8 *targetAddr, uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Send a Get Location Data message
 */
extern ZStatus_t zclGeneral_SendLocationGetData( uint8 srcEP, afAddrType_t *dstAddr,
                                                 zclLocationGetData_t *locData,
                                                 uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Send a Set Device Configuration Response message
 */
extern ZStatus_t zclGeneral_SendLocationDevCfgResponse( uint8 srcEP, afAddrType_t *dstAddr,
                                                        zclLocationDevCfgRsp_t *devCfg,
                                                        uint8 disableDefaultRsp, uint8 seqNum );

/*
 * Send a Location Data Response, Location Data Notification or Compact Location
 * Data Notification message.
 */
extern ZStatus_t zclGeneral_SendLocationData( uint8 srcEP, afAddrType_t *dstAddr, uint8 cmd,
                                              uint8 status, zclLocationData_t *locData,
                                              uint8 disableDefaultRsp, uint8 seqNum );
#endif // ZCL_LOCATION

#ifdef ZCL_ALARMS
/*
 * Add an alarm for a cluster
 */
extern ZStatus_t zclGeneral_AddAlarm( uint8 endpoint, zclGeneral_Alarm_t *alarm );

/*
 * Find an alarm with alarmCode and clusterID
 */
extern zclGeneral_Alarm_t *zclGeneral_FindAlarm( uint8 endpoint, uint8 alarmCode, uint16 clusterID );

/*
 * Find an alarm with the earliest timestamp
 */
extern zclGeneral_Alarm_t *zclGeneral_FindEarliestAlarm( uint8 endpoint );

/*
 * Remove a scene with endpoint and sceneID
 */
extern void zclGeneral_ResetAlarm( uint8 endpoint, uint8 alarmCode, uint16 clusterID );

/*
 * Remove all scenes with endpoint
 */
extern void zclGeneral_ResetAllAlarms( uint8 endpoint, uint8 notifyApp );
#endif // ZCL_ALARMS

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ZCL_GENERAL_H */
