/*
 * MavLink_FrSkySPort
 * https://github.com/Clooney82/MavLink_FrSkySPort
 *
 * Copyright (C) 2014 Rolf Blomgren
 *  http://diydrones.com/forum/topics/amp-to-frsky-x8r-sport-converter
 *  Inspired by https://code.google.com/p/telemetry-convert/
 *    (C) 2013 Jochen Tuchbreiter under (GPL3)
 *
 *  Improved by:
 *    (2014) Christian Swahn
 *    https://github.com/chsw/MavLink_FrSkySPort
 *
 *    (2014) Luis Vale
 *    https://github.com/lvale/MavLink_FrSkySPort
 *
 *    (2015) Michael Wolkstein
 *    https://github.com/wolkstein/MavLink_FrSkySPort
 *
 *    (2015) Jochen Kielkopf
 *    https://github.com/Clooney82/MavLink_FrSkySPort
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * Additional permission under GNU GPL version 3 section 7
 *
 * If you modify this Program, or any covered work, by linking or
 * combining it with FrSkySportTelemetry library (or a modified
 * version of that library), containing parts covered by the terms
 * of FrSkySportTelemetry library, the licensors of this Program
 * grant you additional permission to convey the resulting work.
 * {Corresponding Source for a non-source form of such a combination
 * shall include the source code for the parts of FrSkySportTelemetry
 * library used as well as that of the covered work.}
 *
 */
/*
 * ====================================================================================================
 *
 * Mavlink to FrSky X8R SPort Interface using Teensy 3.1
 *     http://www.pjrc.com/teensy/index.html
 *  based on ideas found here http://code.google.com/p/telemetry-convert/
 * ========================================================================
 *
 * Cut board on the backside to separate Vin from VUSB
 *
 * Connection on Teensy 3.1:
 * -----------------------------------
 *  SPort S --> TX1
 *  SPort + --> Vin
 *  SPort - --> GND
 *  APM Telemetry DF13-5 Pin 2 --> RX2
 *  APM Telemetry DF13-5 Pin 3 --> TX2
 *  APM Telemetry DF13-5 Pin 5 --> GND
 *
 * Note that when used with other telemetry device (3DR Radio 433 or 3DR Bluetooth tested) in parallel
 * on the same port the Teensy should only Receive, so please remove it's TX output (RX input on PixHawk or APM)
 *
 * Analog input  --> A0 (pin14) on Teensy 3.1 ( max 3.3 V ) - Not used
 *
 * This is the data we send to FrSky, you can change this to have your own set of data
 * ----------------------------------------------------------------------------------------------------
 * Data transmitted to FrSky Taranis:
 * Cell            ( Voltage of Cell=Cells/(Number of cells). [V])
 * Cells           ( Voltage from LiPo [V] )
 * A2              ( HDOP value * 25 - 8 bit resolution)
 * A3              ( Roll angle from -Pi to +Pi radians, converted to a value between 0 and 1024)
 * A4              ( Pitch angle from -Pi/2 to +Pi/2 radians, converted to a value between 0 and 1024)
 * Alt             ( Altitude from baro  [m] )
 * GAlt            ( Altitude from GPS   [m] )
 * hdg             ( Compass heading  [deg] )
 * Rpm             ( Throttle when ARMED [%] *100 + % battery remaining as reported by Mavlink)
 * VSpd            ( Vertical speed [m/s] )
 * Speed           ( Ground speed from GPS,  [km/h] )
 * T1              ( GPS status = ap_sat_visible*10) + ap_fixtype )
 * T2              ( Armed Status and Mavlink Messages :- 16 bit value: bit 1: armed - bit 2-5: severity +1 (0 means no message - bit 6-15: number representing a specific text)
 * Vfas            ( same as Cells )
 * Longitud        ( Longitud )
 * Latitud         ( Latitud )
 * Dist            ( Will be calculated by FrSky Taranis as the distance from first received lat/long = Home Position )
 * Fuel            ( Current Flight Mode reported by Mavlink )
 * AccX            ( X Axis average vibration m/s?)
 * AccY            ( Y Axis average vibration m/s?)
 * AccZ            ( Z Axis average vibration m/s?)
 * ====================================================================================================
 */
/*
 * *******************************************************
 * *** Basic Includes:                                 ***
 * *******************************************************
 */
#include "GCS_MAVLink.h"
//#include "mavlink.h"
#include "LSCM.h"
/*
 * *******************************************************
 * *** Basic Configuration:                            ***
 * *******************************************************
 */
#define debugSerial         Serial
#define debugSerialBaud     57600
#define _MavLinkSerial      Serial2
#define _MavLinkSerialBaud  57600
#define START               1
#define MSG_RATE            10      // Hertz
#define AP_SYSID            1       // autopilot system id
#define AP_CMPID            1       // autopilot component id
#define MY_SYSID            123     // teensy system id
#define MY_CMPID            123     // teensy component id
#define GB_SYSID            71      // gimbal system id
#define GB_CMPID            67      // gimbal component id

//#define AC_VERSION          3.2
#define AC_VERSION          3.3
/*
 * *******************************************************
 * *** Enable Addons:                                  ***
 * *******************************************************
 */
//#define USE_FAS_SENSOR_INSTEAD_OF_APM_DATA              // Enable  if you use a FrSky FAS   Sensor.
//#define USE_FLVSS_FAKE_SENSOR_DATA                      // Enable  if you want send fake cell info calculated from VFAS, please set MAXCELLs according your Number of LiPo Cells
//#define USE_SINGLE_CELL_MONITOR                         // Disable if you use a FrSky FLVSS Sensor. - Setup in LSCM Tab
//#define USE_AP_VOLTAGE_BATTERY_FROM_SINGLE_CELL_MONITOR // Use this only with enabled USE_SINGLE_CELL_MONITOR
//#define USE_RC_CHANNELS                                 // Use of RC_CHANNELS Informations ( RAW Input Valus of FC ) - enable if you use TEENSY_LED_SUPPORT.
//#define USE_TEENSY_LED_SUPPORT                          // Enable LED-Controller functionality

/*
 * *******************************************************
 * *** Debug Options:                                  ***
 * *******************************************************
 */
// *** DEBUG MAVLink Messages:
//#define DEBUG_APM_MAVLINK_MSGS              // Show all messages received from APM
//#define DEBUG_APM_CONNECTION
//#define DEBUG_APM_HEARTBEAT                 // MSG #0
//#define DEBUG_APM_SYS_STATUS                // MSG #1   - not used -> use: DEBUG_APM_BAT
//#define DEBUG_APM_BAT                       // Debug Voltage and Current received from APM
//#define DEBUG_APM_GPS_RAW                   // MSG #24
//#define DEBUG_APM_RAW_IMU                   // MSG #27  - not used -> use: DEBUG_APM_ACC
//#define DEBUG_APM_ACC                       // Debug Accelerometer
//#define DEBUG_APM_ATTITUDE                  // MSG #30
//#define DEBUG_APM_GLOBAL_POSITION_INT_COV   // MSG #63  - planned - currently not implemented - not supported by APM
//#define DEBUG_APM_RC_CHANNELS               // MSG #65
//#define DEBUG_APM_VFR_HUD                   // MSG #74
//#define DEBUG_APM_STATUSTEXT                // MSG #254 -
//#define DEBUG_APM_PARSE_STATUSTEXT
//#define DEBUG_GIMBAL_HEARTBEAT
//#define DEBUG_OTHER_HEARTBEAT


// *** DEBUG FrSkySPort Telemetry:
//#define DEBUG_FrSkySportTelemetry
//#define DEBUG_FrSkySportTelemetry_FAS
//#define DEBUG_FrSkySportTelemetry_FLVSS
//#define DEBUG_FrSkySportTelemetry_GPS
//#define DEBUG_FrSkySportTelemetry_RPM
//#define DEBUG_FrSkySportTelemetry_A3A4
//#define DEBUG_FrSkySportTelemetry_VARIO
//#define DEBUG_FrSkySportTelemetry_ACC
//#define DEBUG_FrSkySportTelemetry_FLIGHTMODE

// *** DEBUG other things:
//#define DEBUG_AVERAGE_VOLTAGE
//#define DEBUG_LIPO_SINGLE_CELL_MONITOR // Use this only with enabled USE_SINGLE_CELL_MONITOR


/*
 * *******************************************************
 * *** Variables Definitions:                          ***
 * *******************************************************
 */
// configure number maximum connected analog inputs(cells)
// if you build an six cell network then MAXCELLS is 6
#define MAXCELLS 3

/*
 * *******************************************************
 * *** Mavlink Definitions:                            ***
 * *******************************************************
 */
/*
 * *******************************************************
 * *** Message #0  HEARTHBEAT                          ***
 * *******************************************************
 */
//uint8_t     ap_type               =  0;
//uint8_t     ap_autopilot          =  0;
uint8_t     ap_base_mode          =  0;
int32_t     ap_custom_mode        = -1;
//uint8_t     ap_system_status      =  0;
uint8_t     ap_mavlink_version    =  0;

/*
 * *******************************************************
 * *** Message #1  SYS_STATUS                          ***
 * *******************************************************
 */
uint16_t    ap_voltage_battery    = 0;    // 1000 = 1V | Battery voltage, in millivolts (1 = 1 millivolt)
int16_t     ap_current_battery    = 0;    //   10 = 1A | Battery current, in 10*milliamperes (1 = 10 milliampere),
                                          //              -1: autopilot does not measure the current.
int8_t      ap_battery_remaining  = 0;    // Remaining battery energy: (0%: 0, 100%: 100),
                                          //              -1: autopilot estimate the remaining battery

/*
 * *******************************************************
 * *** Message #24  GPS_RAW_INT                        ***
 * *******************************************************
 */
uint8_t     ap_fixtype            =   0;    // 0 = No GPS, 1 = No Fix, 2 = 2D Fix, 3 = 3D Fix, 4 = DGPS, 5 = RTK.
                                            // Some applications will not use the value of this field unless it is at least two,
                                            // so always correctly fill in the fix.
uint8_t     ap_sat_visible        =   0;    // Number of visible Satelites
// FrSky Taranis uses the first recieved lat/long as homeposition.
int32_t     ap_latitude           =   0;    // Latitude (WGS84), in degrees * 1E7
int32_t     ap_longitude          =   0;    // Longitude (WGS84), in degrees * 1E7
int32_t     ap_gps_altitude       =   0;    // Altitude (AMSL, NOT WGS84), in meters * 1000 (positive for up).
                                            // Note that virtually all GPS modules provide the AMSL altitude in addition to the WGS84 altitude.
//int32_t latitude  = 0;
//int32_t longitude = 0;
int32_t     ap_gps_speed          =   0;    // GPS ground speed (m/s * 100)
uint16_t    ap_gps_hdop           = 255;    // GPS HDOP horizontal dilution of position in cm (m*100). If unknown, set to: 65535
//uint16_t    ap_gps_vdop           =   0;    // GPS VDOP horizontal dilution of position in cm (m*100). If unknown, set to: 65535
uint32_t    ap_cog                =   0;    // Course over ground (NOT heading, but direction of movement) in degrees * 100, 0.0..359.99 degrees. If unknown, set to: 65535

/*
 * *******************************************************
 * *** Message #30  ATTITUDE                           ***
 * *** needed to use current Angles and axis speeds    ***
 * *******************************************************
 */
int32_t     ap_roll_angle         = 0;      // Roll angle (deg -180/180)
int32_t     ap_pitch_angle        = 0;      // Pitch angle (deg -180/180)
uint32_t    ap_yaw_angle          = 0;      // Yaw angle (rad)
uint32_t    ap_roll_speed         = 0;      // Roll angular speed (rad/s)
uint32_t    ap_pitch_speed        = 0;      // Pitch angular speed (rad/s)
uint32_t    ap_yaw_speed          = 0;      // Yaw angular speed (rad/s)

/*
 * *******************************************************
 * *** Message #63  GLOBAL_POSITION_INT_COV            ***
 * *** Needed for Date/Time submission to RC           ***
 * *******************************************************
 */
time_t      ap_gps_time_unix_utc  = 0;      // Timestamp (microseconds since UNIX epoch) in UTC.
                                            // 0 for unknown.
                                            // Commonly filled by the precision time source of a GPS receiver.

// Message #65 RC_CHANNELS
#ifdef USE_RC_CHANNELS
uint8_t     ap_chancount          = 0;      // Total number of RC channels being received.
                                            // This can be larger than 18, indicating that more channels are available but
                                            // not given in this message. This value should be 0 when no RC channels are available.
uint16_t    ap_chan_raw[18]       = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  // RC channel x input value, in microseconds.
                                                                            // A value of UINT16_MAX (65535U) implies the channel is unused.
#endif

/*
 * *******************************************************
 * *** Message #74  VFR_HUD                            ***
 * *******************************************************
 */
//int32_t     ap_airspeed           = 0;    // Current airspeed in m/s
uint32_t    ap_groundspeed        = 0;    // Current ground speed in m/s
uint32_t    ap_heading            = 0;    // Current heading in degrees, in compass units (0..360, 0=north)
uint16_t    ap_throttle           = 0;    // Current throttle setting in integer percent, 0 to 100
// FrSky Taranis uses the first recieved value after 'PowerOn' or  'Telemetry Reset'  as zero altitude
int32_t     ap_bar_altitude       = 0;    // 100 = 1m
int32_t     ap_climb_rate         = 0;    // 100 = 1m/s


/*
 * *******************************************************
 * *** Message #253  MAVLINK_MSG_ID_STATUSTEXT         ***
 * *******************************************************
 */
uint16_t    ap_status_severity    = 255;
uint16_t    ap_status_send_count  =   0;
uint16_t    ap_status_text_id     =   0;
mavlink_statustext_t statustext;
/*
  MAV_SEVERITY_EMERGENCY=0,     System is unusable. This is a "panic" condition.
  MAV_SEVERITY_ALERT=1,         Action should be taken immediately. Indicates error in non-critical systems.
  MAV_SEVERITY_CRITICAL=2,      Action must be taken immediately. Indicates failure in a primary system.
  MAV_SEVERITY_ERROR=3,         Indicates an error in secondary/redundant systems.
  MAV_SEVERITY_WARNING=4,       Indicates about a possible future error if this is not resolved within a given timeframe. Example would be a low battery warning.
  MAV_SEVERITY_NOTICE=5,        An unusual event has occured, though not an error condition. This should be investigated for the root cause.
  MAV_SEVERITY_INFO=6,          Normal operational messages. Useful for logging. No action is required for these messages.
  MAV_SEVERITY_DEBUG=7,         Useful non-operational messages that can assist in debugging. These should not occur during normal operation.
  MAV_SEVERITY_ENUM_END=8,
*/


/*
 * *******************************************************
 * *** These are special for FrSky:                    ***
 * *******************************************************
 */

int32_t     gps_status            = 0;    // (ap_sat_visible * 10) + ap_fixtype
                                          // ex. 83 = 8 sattelites visible, 3D lock
uint8_t     ap_cell_count         = 0;

/*
 * *******************************************************
 * *** Variables needed for Mavlink Connection Status  ***
 * *** and starting FrSkySPort Telemetry               ***
 * *******************************************************
 */
bool          MavLink_Connected     =     0;  // Connected or Not
unsigned long start_telemetry       = 30000;  // Start Telemetry after 30s (or 5s after init)
bool          telemetry_initialized =     0;  // Is FrSkySPort Telemetry initialized

/*
 * *******************************************************
 * *** Wolke´s Single-Lipo-Cell-Monitor Definitions:   ***
 * *******************************************************
 */
#ifdef USE_SINGLE_CELL_MONITOR
  // construct and init LSCM (LipoSingleCellMonitor)
  LSCM lscm(MAXCELLS, 13, 0.99);
#endif


/*
 * *******************************************************
 * *** End of Variables definition                     ***
 * *******************************************************
 */

/*
 * *******************************************************
 * *** Setup:                                          ***
 * *******************************************************
 */
void setup()  {

 // delay(100000);

  #ifdef USE_SINGLE_CELL_MONITOR
    // Set your custom values (double) for LSCM software divider here.
    // Dependent to your resistor network you can call this function with 1-12 parameter.

    //lscm.setCustomCellDivider(1905.331599479, 929.011553273, 615.667808219); // This is an example for three cells
  #endif


  #ifdef DEBUG_LIPO_SINGLE_CELL_MONITOR
    lscm.setDebug(true);                      // Enable LSCM debug
  #endif


  #ifdef USE_TEENSY_LED_SUPPORT
    Teensy_LED_Init();                      // Init LED Controller
  #endif

  Mavlink_setup();                          // Init Mavlink

}

/*
 * *******************************************************
 * *** Main Loop:                                      ***
 * *******************************************************
 */
void loop()  {

  #ifdef USE_SINGLE_CELL_MONITOR
    lscm.process();                         // Read Lipo Cell Informations from Network
  #endif

  _MavLink_receive();                       // receive MavLink communication

  Mavlink_check_connection();               // Check MavLink communication

  if ( MavLink_Connected == 1 ) {           // If we have a valid MavLink Connection
    if (telemetry_initialized == 0 ) {
      #ifdef DEBUG_FrSkySportTelemetry
        debugSerial.println("###############################################");
        debugSerial.print(millis());
        debugSerial.println("\tInitialise FrSky S.Port Telemetry");
        debugSerial.println("###############################################");
      #endif
      FrSkySPort_Init();                    // initialize FrSkySPortTelemetry
      telemetry_initialized = 1;
    }
    if (telemetry_initialized == 1 ) {
      #ifdef DEBUG_FrSkySportTelemetry
        debugSerial.print(millis());
        debugSerial.println("\tProcessing FrSky S.Port Telemetry");
      #endif
      FrSkySPort_Process();               // transmit FrSkySPortTelemetry
    }
    #ifdef USE_TEENSY_LED_SUPPORT
      Teensy_LED_process();                 // Process LED-Controller
    #endif
  }

}
