/*
 * FrSkySPortTelemetry.ino part of MavLink_FrSkySPort
 * https://github.com/Clooney82/MavLink_FrSkySPort
 *
 * Copyright (C) 2015 Jochen Kielkopf
 * https://github.com/Clooney82/MavLink_FrSkySPort
 *
 * Improved by:
 *    (2015) Michael Wolkstein
 *    https://github.com/wolkstein/MavLink_FrSkySPort
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
 * *******************************************************
 * *** Includes needed for FrSkySPortTelemetry         ***
 * *******************************************************
 */
#include "Time.h"
#include "FrSkySportSensor.h"
#include "FrSkySportSensorFcs.h"
#include "FrSkySportSensorAss.h"
#include "FrSkySportSensorFuel.h"
#include "FrSkySportSensorFlvss.h"
#include "FrSkySportSensorGps.h"
#include "FrSkySportSensorRpm.h"
#include "FrSkySportSensorAcc.h"
#include "FrSkySportSensorSp2uart.h"
#include "FrSkySportSensorVario.h"
#include "FrSkySportSingleWireSerial.h"
#include "FrSkySportTelemetry.h"
#ifdef SEND_STATUS_TEXT_MESSAGE
  #include "FrSkySportSensorStatus.h"     // Statustext Message
#endif

/*
 * *******************************************************
 * *** Define FrSkySPortTelemetry Sensors:             ***
 * *******************************************************
 */
#ifndef USE_FCS_SENSOR_INSTEAD_OF_APM_DATA
  FrSkySportSensorFcs fcs;                               // Create FCS sensor with default ID
#endif
FrSkySportSensorFuel fuel;                             // Create FUEL sensor with default ID
#ifdef SEND_STATUS_TEXT_MESSAGE
  FrSkySportSensorStatus txtmsg(FrSkySportSensor::ID9);                             // Create FUEL sensor with given ID
#endif
#if defined USE_SINGLE_CELL_MONITOR || defined USE_FLVSS_FAKE_SENSOR_DATA
  FrSkySportSensorFlvss flvss1;                          // Create FLVSS sensor with default ID
  #if (MAXCELLS > 6)
    FrSkySportSensorFlvss flvss2(FrSkySportSensor::ID15);  // Create FLVSS sensor with given ID
  #endif
#endif
FrSkySportSensorAss ass;                               // Create ASS sensor with default ID
FrSkySportSensorGps gps;                               // Create GPS sensor with default ID
FrSkySportSensorRpm rpm;                               // Create RPM sensor with default ID
FrSkySportSensorAcc acc;                               // Create ACC sensor with default ID
FrSkySportSensorSp2uart sp2uart;                       // Create SP2UART Type B sensor with default ID
FrSkySportSensorVario vario;                           // Create Variometer sensor with default ID
#ifdef POLLING_ENABLED
  FrSkySportTelemetry telemetry(true);                 // Initialise class (with polling)
#else
  FrSkySportTelemetry telemetry;                       // Initialise class (no polling)
#endif

/*
 * *******************************************************
 * *** Define some Variables:                          ***
 * *******************************************************
 */

// Status message ringbuffer vars
const int8_t statusRingsize = 30;
int32_t statusRingHead = 0;
int32_t statusRingTail = 0;
int32_t statusRing[statusRingsize];
uint32_t my_dequeue_status_value = 0;

float FCSCurrent = 0.0;
float FCSVoltage = 0.0;

unsigned long FCS_timer = 0;
int8_t transmit = 0;

// Scale factor for roll/pitch:
// We need to scale down 360 deg to fit when max value is 256, and 256 equals 362 deg
float scalefactor = 360.0/((362.0/360.0)*256.0);

uint32_t handle_A2_A3_value(uint32_t value)
{
  return (value *330-165)/0xFF;
}

#ifdef DEBUG_FrSkySportTelemetry_GPS
  unsigned long GPS_debug_time = 200;
#endif

#ifdef DEBUG_FrSkySportTelemetry_ASS
  unsigned long ASS_debug_time = 200;
#endif
/*
 * *******************************************************
 * *** Initialize FrSkySPortTelemetry                  ***
 * *******************************************************
 */
void FrSkySPort_Init()
{
  // Configure the telemetry serial port and sensors (remember to use & to specify a pointer to sensor)
  telemetry.begin(FrSkySportSingleWireSerial::SERIAL_1, &gps, &ass, &rpm, &sp2uart, &vario, &fuel, &acc);

  #ifndef USE_FCS_SENSOR_INSTEAD_OF_APM_DATA
    telemetry.addSensor( &fcs );
  #endif

  #if defined USE_SINGLE_CELL_MONITOR || defined USE_FLVSS_FAKE_SENSOR_DATA
    #if (MAXCELLS <= 6)
      telemetry.addSensor( &flvss1 );
    #else
      telemetry.addSensor( &flvss1 );
      telemetry.addSensor( &flvss2 );
    #endif
  #endif

  #ifdef SEND_STATUS_TEXT_MESSAGE
    telemetry.addSensor( &txtmsg );
  #endif

}

/*
 * *******************************************************
 * *** Process and Transmit FrSkySPortTelemetry Data   ***
 * *******************************************************
 */
void FrSkySPort_Process()
{
  /*
   * *****************************************************
   * *** Send the telemetry data                       ***
   * *****************************************************
   * Note that the data will only be sent for sensors
   * that are being polled at given moment
   */
  telemetry.send();
  /*
   * *****************************************************
   * *** Set current/voltage sensor (FCS) data         ***
   * *****************************************************
   */
  FrSkySportTelemetry_FCS();
/*
   * *****************************************************
   * *** Set airspeed sensor (ASS) data                ***
   * *****************************************************
   */
  FrSkySportTelemetry_ASS();

  /*
   * *****************************************************
   * *** Set current/voltage sensor (FCS) data         ***
   * *****************************************************
   */
  FrSkySportTelemetry_FLVSS();

  /*
   * *****************************************************
   * *** Set GPS data                                  ***
   * *****************************************************
   */
  FrSkySportTelemetry_GPS();

  /*
   * *****************************************************
   * *** Set RPM/Temperature sensor data               ***
   * *****************************************************
   */
  FrSkySportTelemetry_RPM();

  /*
   * *****************************************************
   * *** Set SP2UART sensor data ( A3 & A4 )           ***
   * *****************************************************
   */
  FrSkySportTelemetry_A3A4();

  /*
   * *****************************************************
   * *** Set variometer data                           ***
   * *****************************************************
   */
  FrSkySportTelemetry_VARIO();

  /*
   * *****************************************************
   * *** Set Accelerometer data                        ***
   * *****************************************************
   */
  FrSkySportTelemetry_ACC();

  /*
   * *****************************************************
   * *** Set Fuel sensor data (Flight Mode)            ***
   * *****************************************************
   */
  FrSkySportTelemetry_FUEL();
  /*
   * *****************************************************
   * *** Set TextMsg sensor data ()                    ***
   * *****************************************************
   */
  FrSkySportTelemetry_TXTMSG();
  /*
   * *****************************************************
   * *** Send the telemetry data                       ***
   * *****************************************************
   * Note that the data will only be sent for sensors
   * that are being polled at given moment
   */
//  telemetry.send();
}



/*
 * *******************************************************
 * *** Set current/voltage sensor (FCS) data           ***
 * *******************************************************
 * set Voltage source to FCS in menu to use this data for battery voltage,
 * set Current source to FCS in menu to use this data for current readings
 */
void FrSkySportTelemetry_FCS() {
  #ifndef USE_FCS_SENSOR_INSTEAD_OF_APM_DATA
    FCSVoltage = readAndResetAverageVoltage();
    if ( FCSVoltage > 0 ) {                                 // only progress if we have a Battery Voltage
      //FCSCurrent = readAndResetAverageCurrent();            // read Average Current
      #ifdef DEBUG_FrSkySportTelemetry_FCS
        debugSerial.print(millis());
        debugSerial.println("FrSkySportTelemetry_FCS:");
        debugSerial.print("\tVFCS (0x0210): ");
        debugSerial.print(FCSVoltage / 1000.0 );
        debugSerial.print("\tCurr (0x0200): ");
        //debugSerial.print(FCSCurrent);
        debugSerial.print(ap_current_battery / 100.0);
        debugSerial.println();
      #endif
      fcs.setData(ap_current_battery / 100.0,    // Current consumption in amps
                  FCSVoltage / 1000.0);           // Battery voltage in volts
    }
  #endif
}

/*
 * *******************************************************
 * *** Set current/voltage sensor (FCS) data           ***
 * *******************************************************
 * set LiPo voltage sensor (FLVSS) data (we use two sensors to simulate 8S battery)
 * set Voltage source to Cells in menu to use this data for battery voltage
 */
void FrSkySportTelemetry_FLVSS() {
  #ifdef USE_SINGLE_CELL_MONITOR
    #ifdef DEBUG_FrSkySportTelemetry_FLVSS
        debugSerial.print(millis());
        debugSerial.print("\tmaxCells: ");
        debugSerial.println(MAXCELLS);
      for (int i=0; i < MAXCELLS; i++) {
        debugSerial.print(millis());
        debugSerial.print("\tZelle[");
        debugSerial.print(i);
        debugSerial.print("]: ");
        debugSerial.print(lscm.getCellVoltageAsUint32_T(i) / 1000.0);
        debugSerial.println("Volt");

      }
    #endif

    switch(lscm.getCellsInUse()) {
      case 1:
        flvss1.setData(lscm.getCellVoltageAsUint32_T(0) / 1000.0);
        break;
      case 2:
        flvss1.setData(lscm.getCellVoltageAsUint32_T(0) / 1000.0, lscm.getCellVoltageAsUint32_T(1) / 1000.0);
        break;
      case 3:
        flvss1.setData(lscm.getCellVoltageAsUint32_T(0) / 1000.0, lscm.getCellVoltageAsUint32_T(1) / 1000.0, lscm.getCellVoltageAsUint32_T(2) / 1000.0);
        break;
      case 4:
        flvss1.setData(lscm.getCellVoltageAsUint32_T(0) / 1000.0, lscm.getCellVoltageAsUint32_T(1) / 1000.0, lscm.getCellVoltageAsUint32_T(2) / 1000.0, lscm.getCellVoltageAsUint32_T(3) / 1000.0);
        break;
      case 5:
        flvss1.setData(lscm.getCellVoltageAsUint32_T(0) / 1000.0, lscm.getCellVoltageAsUint32_T(1) / 1000.0, lscm.getCellVoltageAsUint32_T(2) / 1000.0, lscm.getCellVoltageAsUint32_T(3) / 1000.0, lscm.getCellVoltageAsUint32_T(4) / 1000.0);
        break;
      case 6:
        flvss1.setData(lscm.getCellVoltageAsUint32_T(0) / 1000.0, lscm.getCellVoltageAsUint32_T(1) / 1000.0, lscm.getCellVoltageAsUint32_T(2) / 1000.0, lscm.getCellVoltageAsUint32_T(3) / 1000.0, lscm.getCellVoltageAsUint32_T(4) / 1000.0, lscm.getCellVoltageAsUint32_T(5) / 1000.0);
        break;
      #if (MAXCELLS > 6)
      case 7:
        flvss1.setData(lscm.getCellVoltageAsUint32_T(0) / 1000.0, lscm.getCellVoltageAsUint32_T(1) / 1000.0, lscm.getCellVoltageAsUint32_T(2) / 1000.0, lscm.getCellVoltageAsUint32_T(3) / 1000.0, lscm.getCellVoltageAsUint32_T(4) / 1000.0, lscm.getCellVoltageAsUint32_T(5) / 1000.0);
        flvss2.setData(lscm.getCellVoltageAsUint32_T(6) / 1000.0);
        break;
      case 8:
        flvss1.setData(lscm.getCellVoltageAsUint32_T(0) / 1000.0, lscm.getCellVoltageAsUint32_T(1) / 1000.0, lscm.getCellVoltageAsUint32_T(2) / 1000.0, lscm.getCellVoltageAsUint32_T(3) / 1000.0, lscm.getCellVoltageAsUint32_T(4) / 1000.0, lscm.getCellVoltageAsUint32_T(5) / 1000.0);
        flvss2.setData(lscm.getCellVoltageAsUint32_T(6) / 1000.0, lscm.getCellVoltageAsUint32_T(7) / 1000.0);
        break;
      case 9:
        flvss1.setData(lscm.getCellVoltageAsUint32_T(0) / 1000.0, lscm.getCellVoltageAsUint32_T(1) / 1000.0, lscm.getCellVoltageAsUint32_T(2) / 1000.0, lscm.getCellVoltageAsUint32_T(3) / 1000.0, lscm.getCellVoltageAsUint32_T(4) / 1000.0, lscm.getCellVoltageAsUint32_T(5) / 1000.0);
        flvss2.setData(lscm.getCellVoltageAsUint32_T(6) / 1000.0, lscm.getCellVoltageAsUint32_T(7) / 1000.0, lscm.getCellVoltageAsUint32_T(8) / 1000.0);
        break;
      case 10:
        flvss1.setData(lscm.getCellVoltageAsUint32_T(0) / 1000.0, lscm.getCellVoltageAsUint32_T(1) / 1000.0, lscm.getCellVoltageAsUint32_T(2) / 1000.0, lscm.getCellVoltageAsUint32_T(3) / 1000.0, lscm.getCellVoltageAsUint32_T(4) / 1000.0, lscm.getCellVoltageAsUint32_T(5) / 1000.0);
        flvss2.setData(lscm.getCellVoltageAsUint32_T(6) / 1000.0, lscm.getCellVoltageAsUint32_T(7) / 1000.0, lscm.getCellVoltageAsUint32_T(8) / 1000.0, lscm.getCellVoltageAsUint32_T(9) / 1000.0);
        break;
      case 11:
        flvss1.setData(lscm.getCellVoltageAsUint32_T(0) / 1000.0, lscm.getCellVoltageAsUint32_T(1) / 1000.0, lscm.getCellVoltageAsUint32_T(2) / 1000.0, lscm.getCellVoltageAsUint32_T(3) / 1000.0, lscm.getCellVoltageAsUint32_T(4) / 1000.0, lscm.getCellVoltageAsUint32_T(5) / 1000.0);
        flvss2.setData(lscm.getCellVoltageAsUint32_T(6) / 1000.0, lscm.getCellVoltageAsUint32_T(7) / 1000.0, lscm.getCellVoltageAsUint32_T(8) / 1000.0, lscm.getCellVoltageAsUint32_T(9) / 1000.0, lscm.getCellVoltageAsUint32_T(10) / 1000.0);
        break;
      case 12:
        flvss1.setData(lscm.getCellVoltageAsUint32_T(0) / 1000.0, lscm.getCellVoltageAsUint32_T(1) / 1000.0, lscm.getCellVoltageAsUint32_T(2) / 1000.0, lscm.getCellVoltageAsUint32_T(3) / 1000.0, lscm.getCellVoltageAsUint32_T(4) / 1000.0, lscm.getCellVoltageAsUint32_T(5) / 1000.0);
        flvss2.setData(lscm.getCellVoltageAsUint32_T(6) / 1000.0, lscm.getCellVoltageAsUint32_T(7) / 1000.0, lscm.getCellVoltageAsUint32_T(8) / 1000.0, lscm.getCellVoltageAsUint32_T(9) / 1000.0, lscm.getCellVoltageAsUint32_T(10) / 1000.0, lscm.getCellVoltageAsUint32_T(11) / 1000.0);
        break;
      #endif
    }
  #elif defined USE_FLVSS_FAKE_SENSOR_DATA
    float fake_cell_voltage = (ap_voltage_battery / ap_cell_count) / 1000.0;
    #ifdef DEBUG_FrSkySportTelemetry_FLVSS
        debugSerial.print(millis());
        debugSerial.print("\tmaxCells: ");
        debugSerial.println(ap_cell_count);
      for (int i=0; i < ap_cell_count; i++) {
        debugSerial.print(millis());
        debugSerial.print("\tZelle[");
        debugSerial.print(i);
        debugSerial.print("]: ");
        debugSerial.print(fake_cell_voltage);
        debugSerial.println("Volt");

      }
    #endif
    switch (ap_cell_count) {
      case 1:
        flvss1.setData(fake_cell_voltage);
        break;
      case 2:
        flvss1.setData(fake_cell_voltage, fake_cell_voltage);
        break;
      case 3:
        flvss1.setData(fake_cell_voltage, fake_cell_voltage, fake_cell_voltage);
        break;
      case 4:
        flvss1.setData(fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage);
        break;
      case 5:
        flvss1.setData(fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage);
        break;
      case 6:
        flvss1.setData(fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage);
        break;
      #if (MAXCELLS > 6)
      case 7:
        flvss1.setData(fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage);
        flvss2.setData(fake_cell_voltage);
        break;
      case 8:
        flvss1.setData(fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage);
        flvss2.setData(fake_cell_voltage, fake_cell_voltage);
        break;
      case 9:
        flvss1.setData(fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage);
        flvss2.setData(fake_cell_voltage, fake_cell_voltage, fake_cell_voltage);
        break;
      case 10:
        flvss1.setData(fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage);
        flvss2.setData(fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage);
        break;
      case 11:
        flvss1.setData(fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage);
        flvss2.setData(fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage);
        break;
      case 12:
        flvss1.setData(fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage);
        flvss2.setData(fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage, fake_cell_voltage);
        break;
      #endif
    }
  #endif
}

/*
 * *******************************************************
 * *** Set GPS data                                    ***
 * *******************************************************
 */
void FrSkySportTelemetry_GPS() {
    gps.setData(ap_latitude / 1E7, ap_longitude / 1E7,    // Latitude and longitude in degrees decimal (positive for N/E, negative for S/W)
        ap_gps_altitude / 10.0,                           // Altitude (AMSL, NOT WGS84), in meters * 1000 (positive for up). Note that virtually all GPS modules provide the AMSL altitude in addition to the WGS84 altitude.
        ap_groundspeed / 1.944,                           // Counteract Sport library which converts to kts (allows us to use default GPS settings on Taranis)
        ap_heading,                                       // Heading, in degrees * 100, 0.0..359.99 degrees. If unknown, set to: UINT16_MAX
        ap_gps_hdop);                                     // GPS HDOP horizontal dilution of position in cm (m*100)

    #ifdef DEBUG_FrSkySportTelemetry_GPS
      if (millis() > GPS_debug_time) {
        debugSerial.print(millis());
        debugSerial.print("\tFrSky Latitude: ");
        debugSerial.print(ap_latitude/1E7);
        debugSerial.print("\tFrSky Longitude: ");
        debugSerial.print(ap_longitude/1E7);
        debugSerial.print("\tGPSAlt: ");
        debugSerial.print(ap_gps_altitude / 10.0);
        debugSerial.print("cm");
        debugSerial.print("\tGroundSpeed: ");
        debugSerial.print(ap_groundspeed);
        debugSerial.print("m/s");
        debugSerial.print("\tCog: ");
        debugSerial.print(ap_cog);
        debugSerial.print("°");
        debugSerial.print("\tHeading: ");
        debugSerial.print(ap_heading);
        debugSerial.print("°");
        debugSerial.print("\tHDOP (A2): ");
        debugSerial.print(ap_gps_hdop);
        debugSerial.println();
        GPS_debug_time = millis() + 200;
      }
    #endif
}

/*
 * *******************************************************
 * *** Set RPM/Temperature sensor data                 ***
 * *******************************************************
 *
 * 16 bit value:
 * * bit 1     : armed
 * * bit 2 -  5: severity +1 (0 means no message)
 * * bit 6 - 15: number representing a specific text
 */
void FrSkySportTelemetry_RPM() {
  uint32_t ap_status_value = ap_base_mode&0x01;
  // If we have a message-text to report (we send it multiple times to make sure it arrives even on telemetry glitches)
  if(ap_status_send_count > 0 && ap_status_text_id > 0)
  {
    // Add bits 2-15
    ap_status_value |= (((ap_status_severity+1)&0x0F)<<1) |((ap_status_text_id&0x3FF)<<5);

    // put message into status queue if queue is smaller than 15 items
    // ap_status_send_count is set to 1. this happens only ones.
    if(statusQueuelevel()<30)
      statusEqueue(ap_status_value);

    ap_status_send_count--;
    if(ap_status_send_count == 0)
    {
       // Reset severity and text-message after we have sent the message
       ap_status_severity = 0;
       ap_status_text_id = 0;
    }
  }

  #ifdef DEBUG_FrSkySportTelemetry_RPM
  if(ap_status_value>0){
    debugSerial.print(millis());
    debugSerial.print("\tRPM (Throttle/battery_remain): ");
    debugSerial.print(ap_throttle * 200+ap_battery_remaining*2);
    debugSerial.print("\tT1 ((ap_sat_visible * 10) + ap_fixtype): ");
    debugSerial.print(gps_status);
    debugSerial.print("\tT2 (Armed Status + Severity + Statustext): ");
    debugSerial.print(ap_status_value);
    debugSerial.println();
  }
  #endif

  // we dequeue status every 1400ms T2 sensor sends all 1000ms.

  static long dequeue_request = 0;
  long actualtime = millis();
  if(actualtime > dequeue_request + 1400){

    //returns 0 if statusDequeue is empty
    my_dequeue_status_value = statusDequeue();
    // if statusDequeue() returns 0 we set armed status from ap_status_value
    if(my_dequeue_status_value == 0)
      my_dequeue_status_value = ap_status_value;
    dequeue_request = actualtime;
  }

  rpm.setData(ap_throttle * 200+ap_battery_remaining*2,    // * 2 if number of blades on Taranis is set to 2 + First 4 digits reserved for battery remaining in %
              gps_status,         // (ap_sat_visible * 10) + ap_fixtype eg. 83 = 8 sattelites visible, 3D lock
              my_dequeue_status_value);   // Armed Status + Severity + Statustext


}

/*
 * *****************************************************
 * *** Set SP2UART sensor data ( A3 & A4 )           ***
 * *****************************************************
 */
void FrSkySportTelemetry_A3A4() {
  #ifdef USE_MAV_RSSI    //we will assume we require Mavlink RSSI output on A3 as likely using with ULRS
    #ifdef DEBUG_FrSkySportTelemetry_A3A4
      debugSerial.print(millis());
      debugSerial.print("\tRSSI (A3): ");
      debugSerial.print(ap_rssi / 2.55);
      debugSerial.println();
    #endif
    sp2uart.setData(ap_rssi / 2.55,   // Mavlink RSSI
                              0);     // Assign zero to A4 - used in LUA to replace regular RSSI with RSSI on A3
  #else                               //if no polling then we will output roll/pitch on A3/A4
    #ifdef DEBUG_FrSkySportTelemetry_A3A4
      debugSerial.print(millis());
      debugSerial.print("\tRoll Angle (A3): ");
      debugSerial.print(handle_A2_A3_value((ap_roll_angle+180)/scalefactor));
      debugSerial.print("\tPitch Angle (A4): ");
      debugSerial.print(handle_A2_A3_value((ap_pitch_angle+180)/scalefactor));
      debugSerial.println();
    #endif
    sp2uart.setData(handle_A2_A3_value((ap_roll_angle+180)/scalefactor),     // Roll Angle
                  handle_A2_A3_value((ap_pitch_angle+180)/scalefactor));   // Pitch Angle
  #endif
}

/*
 * *******************************************************
 * *** Set variometer data                             ***
 * *******************************************************
 * set Variometer source to VSpd in menu to use the vertical speed data from this sensor for variometer.
 */
void FrSkySportTelemetry_VARIO() {
  #ifdef DEBUG_FrSkySportTelemetry_VARIO
    debugSerial.print(millis());
//    debugSerial.print("\tCurrent altitude: ");
//    debugSerial.print(ap_bar_altitude / 100.0);
    debugSerial.print("\tCurrent home altitude: ");
    debugSerial.print(ap_relative_alt / 100.0);
    debugSerial.print("m\tCurrent climb rate in meters/second: ");
    debugSerial.print(ap_climb_rate /100.0);
    debugSerial.print("m/s");
    debugSerial.println();
  #endif
    //vario.setData(ap_bar_altitude,  // Current altitude (MSL), in meters
    vario.setData(ap_relative_alt /100,    // Current home location altitude, in centimeters
                ap_climb_rate /100.0);   // Current climb rate in meters/second
}

/*
 * *******************************************************
 * *** Set Accelerometer data                          ***
 * *******************************************************
 */
void FrSkySportTelemetry_ACC() {
  #ifdef DEBUG_FrSkySportTelemetry_ACC
    debugSerial.print(millis());
    debugSerial.print("\tX acceleration (raw): ");
    debugSerial.print(fetchAccX());
    debugSerial.print("\tY acceleration (raw): ");
    debugSerial.print(fetchAccY());
    debugSerial.print("\tZ acceleration (raw): ");
    debugSerial.print(fetchAccZ());
    debugSerial.println();
  #endif
  acc.setData(fetchAccX(),        // X acceleration (raw)
              fetchAccY(),        // Y acceleration (raw)
              fetchAccZ());       // Z acceleration (raw)
}

/*
 * *****************************************************
 * *** Set Fuel sensor data                          ***
 * *****************************************************
 * Used for Flight Mode
 */
void FrSkySportTelemetry_FUEL() {
  #ifdef DEBUG_FrSkySportTelemetry_FLIGHTMODE
    debugSerial.print(millis());
    debugSerial.print("\tFlightmode: ");
    debugSerial.print(ap_custom_mode);
    debugSerial.println();
  #endif
  if(ap_custom_mode >= 0) {
    fuel.setData(ap_custom_mode);
  }
}

/*
 * *****************************************************
 * *** Set Airspeed sensor data                      ***
 * *****************************************************
 */
void FrSkySportTelemetry_ASS() {
  #ifdef DEBUG_FrSkySportTelemetry_ASS
    if (millis() > ASS_debug_time) {
      debugSerial.print(millis());
      debugSerial.print("\tFrSky Airspeed (m/s): ");
      debugSerial.print(ap_airspeed);
      debugSerial.println();
      ASS_debug_time = millis() + 200;
    }
  #endif
  ass.setData(ap_airspeed);
}

/*
 * *****************************************************
 * *** Set TextMsg sensor data ()                    ***
 * *****************************************************
 */
void FrSkySportTelemetry_TXTMSG() {
  #ifdef SEND_STATUS_TEXT_MESSAGE
    #ifdef DEBUG_FrSkySportTelemetry_TXTMSG
      txtmsg.setDebug(true);
    #endif
    if(status_text_buffer_id != 0) {
      txtmsg.send_text_message(status_text_buffer);
      status_text_buffer_id = 0;
    }

  #endif
}

/*
 * *****************************************************
 * *** Helper function "queue buffer" to queue       ***
 * *** Armed Status, Severity - Statustext messages  ***
 * *****************************************************
 * Used for Flight Mode
 */

  // Put something into the buffer. Returns 0 when the buffer was full,
  // 1 when the stuff was put sucessfully into the buffer
int statusEqueue(int32_t val) {
  int32_t newtail = (statusRingTail + 1) % statusRingsize;
  if (newtail == statusRingHead) {
     // Buffer is full, do nothing
     return 0;
  }
  else {
     statusRing[statusRingTail] = val;
     statusRingTail = newtail;
     return 1;
  }
}

// Return number of elements in the queue.
int statusQueuelevel () {
   return statusRingTail - statusRingHead + (statusRingHead > statusRingTail ? statusRingsize : 0);
}

// Get something from the queue. 0 will be returned if the queue is empty
int32_t statusDequeue () {
  if (statusRingHead == statusRingTail) {
     return 0;
  }
  else {
     uint32_t val = statusRing[statusRingHead];
     statusRingHead  = (statusRingHead + 1) % statusRingsize;
     return val;
  }
}
