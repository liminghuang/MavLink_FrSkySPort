/*
 * Mavlink.ino part of MavLink_FrSkySPort
 * https://github.com/Clooney82/MavLink_FrSkySPort
 *
 * Copyright (C) 2015 Jochen Kielkopf
 * https://github.com/Clooney82/MavLink_FrSkySPort
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

#ifdef DEBUG_APM_RC_CHANNELS
  unsigned long RC_DEBUG_TIMEOUT = 3000;
#endif

bool          MavLink_Connected_Last = 0;
unsigned long MavLink_Connected_timer = 0;
uint8_t       hb_count = 0;
uint8_t       hb_count_lost = 0;
unsigned long hb_timer = 0;

int           led = 13;

mavlink_message_t msg;
mavlink_system_t  mavlink_system = {MY_SYSID,MY_CMPID};
mavlink_status_t  status;
uint16_t          len = 0;
uint8_t           buf[MAVLINK_MAX_PACKET_LEN];

unsigned long send_mavlink_connection_config = 0;
/*
 * *******************************************************
 * *** Setup Mavlink:                                  ***
 * *******************************************************
 */
void Mavlink_setup() {
  _MavLinkSerial.begin(_MavLinkSerialBaud);
  debugSerial.begin(debugSerialBaud);

  pinMode(led,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(14,INPUT);

  MavLink_Connected_Last = 0;
  MavLink_Connected = 0;
}

/*
 * *******************************************************
 * *** Send Mavlink Heartbeat:                         ***
 * *******************************************************
 */
void Mavlink_send_heartbeat() {
  // Send a heartbeat over the mavlink
  #ifdef DEBUG_APM_CONNECTION
    debugSerial.print(millis());
    debugSerial.println("\tSending heartbeat message.");
  #endif

  //mavlink_msg_heartbeat_pack(123, 123, &msg, MAV_TYPE_ONBOARD_CONTROLLER, MAV_AUTOPILOT_INVALID, MAV_MODE_PREFLIGHT, <CUSTOM_MODE>, MAV_STATE_STANDBY);
  mavlink_msg_heartbeat_pack(mavlink_system.sysid, mavlink_system.compid, &msg, 18, 8, 0, 0, 4);
  len = mavlink_msg_to_send_buffer(buf, &msg);
  _MavLinkSerial.write(buf,len);
}

/*
 * *******************************************************
 * *** Config Mavlink connection:                      ***
 * *******************************************************
 */
void Mavlink_config_connection() {
  #ifdef DEBUG_APM_CONNECTION
    debugSerial.print(millis());
    debugSerial.println("\tSetting up Mavlink streams");
  #endif
  digitalWrite(led,HIGH);
  // mavlink_msg_request_data_stream_pack(0xFF,0xBE,&msg,1,1,MAV_DATA_STREAM_EXTENDED_STATUS, MSG_RATE, START);
  mavlink_msg_request_data_stream_pack(mavlink_system.sysid,mavlink_system.compid,&msg,AP_SYSID,AP_CMPID,MAV_DATA_STREAM_EXTENDED_STATUS, MSG_RATE, START);
  len = mavlink_msg_to_send_buffer(buf, &msg);
  _MavLinkSerial.write(buf,len);
  delay(10);
  mavlink_msg_request_data_stream_pack(mavlink_system.sysid,mavlink_system.compid,&msg,AP_SYSID,AP_CMPID,MAV_DATA_STREAM_EXTRA2, MSG_RATE, START);
  len = mavlink_msg_to_send_buffer(buf, &msg);
  _MavLinkSerial.write(buf,len);
  delay(10);
  mavlink_msg_request_data_stream_pack(mavlink_system.sysid,mavlink_system.compid,&msg,AP_SYSID,AP_CMPID,MAV_DATA_STREAM_RAW_SENSORS, MSG_RATE, START);
  len = mavlink_msg_to_send_buffer(buf, &msg);
  _MavLinkSerial.write(buf,len);
  #ifdef USE_RC_CHANNELS
    delay(10);
    mavlink_msg_request_data_stream_pack(mavlink_system.sysid,mavlink_system.compid,&msg,AP_SYSID,AP_CMPID,MAV_DATA_STREAM_RC_CHANNELS, MSG_RATE, START);
    len = mavlink_msg_to_send_buffer(buf, &msg);
    _MavLinkSerial.write(buf,len);
  #endif
  digitalWrite(led,LOW);
  send_mavlink_connection_config++;

}

/*
 * *******************************************************
 * *** Check Mavlink Connection:                       ***
 * *******************************************************
 */
void Mavlink_check_connection() {
  if(millis() > hb_timer) {
    hb_timer=millis() + 1500;
    if(MavLink_Connected == 1) {
      //Mavlink_send_heartbeat();
    }
    // Sending Mavlink configuration after 10sec, to give FC enough time to boot.
    if (send_mavlink_connection_config == 0) {
      if (millis() > 10000) {
        Mavlink_config_connection();
      }
    }
    if(millis() > MavLink_Connected_timer)  {   // if no HEARTBEAT from APM  in 1.5s then we are not connected
      hb_count_lost++;
      if ( hb_count_lost > 5 ) {
        MavLink_Connected_Last = MavLink_Connected;
        MavLink_Connected = 0;
        hb_count = 0;
        #ifdef DEBUG_APM_CONNECTION
          if (MavLink_Connected_Last == 1) {
            debugSerial.print(millis());
            debugSerial.println("\tLost MavLink connection...");
          } else {
            debugSerial.print(millis());
            debugSerial.println("\tMavLink not connected.");
          }
        #endif
      }
    } else if(hb_count >= 10) {        // If  received > 10 heartbeats from MavLink then we are connected
      MavLink_Connected_Last = MavLink_Connected;
      MavLink_Connected = 1;
      hb_count_lost = 0;
      #ifdef DEBUG_APM_CONNECTION
        if(hb_count == 10) {
          debugSerial.println("###############################################");
          debugSerial.print(millis());
          debugSerial.println("\tMavLink connection etablished.");
          debugSerial.print("\tMavLink Version: ");
          debugSerial.println(ap_mavlink_version);
          debugSerial.println("###############################################");
          send_mavlink_connection_config = 0;
        }
      #endif
      digitalWrite(led,HIGH);      // LED will be ON when connected to MavLink, else it will slowly blink
    }
  }
}

/*
 * *******************************************************
 * *** Receive Mavlink Messages:                       ***
 * *******************************************************
 */
void _MavLink_receive() {

  while(_MavLinkSerial.available())
  {
    uint8_t c = _MavLinkSerial.read();
    if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status) && (AP_SYSID == msg.sysid && AP_CMPID == msg.compid)) // only proceed with autopilot messages
    {
      /*
       * *******************************************************
       * *** Ardupilot MAVLINK Message                       ***
       * *******************************************************
       */
      #ifdef DEBUG_APM_MAVLINK_MSGS
        debugSerial.print(millis());
        debugSerial.print("\tMSG: ");
        debugSerial.print(msg.msgid);
        debugSerial.println();
      #endif
      switch(msg.msgid)
      {
        /*
         * *****************************************************
         * *** MAVLINK Message #0 - HEARTBEAT                ***
         * *****************************************************
         */
        case MAVLINK_MSG_ID_HEARTBEAT:
          ap_base_mode = (mavlink_msg_heartbeat_get_base_mode(&msg) & 0x80) > 7;
          ap_custom_mode = mavlink_msg_heartbeat_get_custom_mode(&msg);
          ap_mavlink_version = mavlink_msg_heartbeat_get_mavlink_version(&msg);
          #ifdef DEBUG_APM_HEARTBEAT
            debugSerial.print(millis());
            debugSerial.print("\tMAVLINK_MSG_ID_SYS_STATUS: base_mode: ");
            debugSerial.print((mavlink_msg_heartbeat_get_base_mode(&msg) & 0x80) > 7);
            debugSerial.print(", custom_mode: ");
            debugSerial.print(mavlink_msg_heartbeat_get_custom_mode(&msg));
            debugSerial.println();
          #endif

          hb_count++;
          MavLink_Connected_timer=millis() + 1500;
          #ifdef DEBUG_APM_CONNECTION
            debugSerial.print(millis());
            debugSerial.print("\t\theartbeat count:");
            debugSerial.print(hb_count);
            debugSerial.println();
          #endif
          break;
        /*
         * *****************************************************
         * *** MAVLINK Message #1 - System Status            ***
         * *****************************************************
         */
        case MAVLINK_MSG_ID_SYS_STATUS:
          #ifdef USE_AP_VOLTAGE_BATTERY_FROM_SINGLE_CELL_MONITOR
            ap_voltage_battery = lscm.getAllLipoCells();
            //no lipo to network connected use reported mavlink_msg voltage
            if(lscm.getCellsInUse() == 0) ap_voltage_battery = mavlink_msg_sys_status_get_voltage_battery(&msg);
          #else
            ap_voltage_battery = mavlink_msg_sys_status_get_voltage_battery(&msg);  // 1 = 1mV
          #endif
          ap_current_battery = mavlink_msg_sys_status_get_current_battery(&msg);     // 1=10mA
          ap_battery_remaining = mavlink_msg_sys_status_get_battery_remaining(&msg); //battery capacity reported in %
          storeVoltageReading(ap_voltage_battery);
          storeCurrentReading(ap_current_battery);

          #ifdef DEBUG_APM_BAT
            debugSerial.print(millis());
            debugSerial.print("\tMAVLINK_MSG_ID_SYS_STATUS: voltage_battery: ");
            debugSerial.print(mavlink_msg_sys_status_get_voltage_battery(&msg));
            debugSerial.print(", current_battery: ");
            debugSerial.print(mavlink_msg_sys_status_get_current_battery(&msg));
            debugSerial.println();
          #endif
          uint8_t temp_cell_count;
          #ifdef USE_SINGLE_CELL_MONITOR
            ap_cell_count =  lscm.getCellsInUse();
            if (lscm.getCellsInUse() == 0){
              if (ap_voltage_battery > 21000) temp_cell_count = 6;
              else if (ap_voltage_battery > 17500) temp_cell_count = 5;
              else if (ap_voltage_battery > 12750) temp_cell_count = 4;
              else if (ap_voltage_battery > 8500)  temp_cell_count = 3;
              else if (ap_voltage_battery > 4250)  temp_cell_count = 2;
              else temp_cell_count = 0;
              if (temp_cell_count > ap_cell_count)
                ap_cell_count = temp_cell_count;
            }
          #else
            if(ap_voltage_battery > 21000) temp_cell_count = 6;
            else if (ap_voltage_battery > 17500) temp_cell_count = 5;
            else if (ap_voltage_battery > 12750) temp_cell_count = 4;
            else if (ap_voltage_battery > 8500)  temp_cell_count = 3;
            else if (ap_voltage_battery > 4250)  temp_cell_count = 2;
            else temp_cell_count = 0;
            if(temp_cell_count > ap_cell_count)
              ap_cell_count = temp_cell_count;
          #endif
          break;
        /*
         * *****************************************************
         * *** MAVLINK Message #24 - GPS_RAW_INT             ***
         * *****************************************************
         */
        case MAVLINK_MSG_ID_GPS_RAW_INT:
          ap_fixtype = mavlink_msg_gps_raw_int_get_fix_type(&msg);                               // 0 = No GPS, 1 =No Fix, 2 = 2D Fix, 3 = 3D Fix, 4 = DGPS, 5 = RTK
          ap_sat_visible =  mavlink_msg_gps_raw_int_get_satellites_visible(&msg);          // numbers of visible satelites
          gps_status = (ap_sat_visible*10) + ap_fixtype;
          ap_gps_hdop = mavlink_msg_gps_raw_int_get_eph(&msg);
          // Max 8 bit
          if(ap_gps_hdop == 0 || ap_gps_hdop > 255)
            ap_gps_hdop = 255;
          if(ap_fixtype >= 3)  {
            ap_latitude = mavlink_msg_gps_raw_int_get_lat(&msg);
            ap_longitude = mavlink_msg_gps_raw_int_get_lon(&msg);
            ap_gps_altitude = mavlink_msg_gps_raw_int_get_alt(&msg);      // 1m = 1000
            ap_gps_speed = mavlink_msg_gps_raw_int_get_vel(&msg);         // 100 = 1m/s
            ap_cog = mavlink_msg_gps_raw_int_get_cog(&msg)/100;
          }
          else
          {
            ap_gps_speed = 0;
          }
          #ifdef DEBUG_APM_GPS_RAW
            debugSerial.print(millis());
            debugSerial.print("\tMAVLINK_MSG_ID_GPS_RAW_INT: fixtype: ");
            debugSerial.print(ap_fixtype);
            debugSerial.print(", visiblesats: ");
            debugSerial.print(ap_sat_visible);
            debugSerial.print(", status: ");
            debugSerial.print(gps_status);
            debugSerial.print(", gpsspeed: ");
            debugSerial.print(mavlink_msg_gps_raw_int_get_vel(&msg)/100.0);
            debugSerial.print(", hdop: ");
            debugSerial.print(mavlink_msg_gps_raw_int_get_eph(&msg)/100.0);
            debugSerial.print(", alt: ");
            debugSerial.print(mavlink_msg_gps_raw_int_get_alt(&msg));
            debugSerial.println();
          #endif
          break;
        /*
         * *****************************************************
         * *** MAVLINK Message #27 - RAW_IMU                 ***
         * *****************************************************
         */
        case MAVLINK_MSG_ID_RAW_IMU:
          storeAccX(mavlink_msg_raw_imu_get_xacc(&msg) / 10);
          storeAccY(mavlink_msg_raw_imu_get_yacc(&msg) / 10);
          storeAccZ(mavlink_msg_raw_imu_get_zacc(&msg) / 10);

          #ifdef DEBUG_APM_ACC
            debugSerial.print(millis());
            debugSerial.print("\tMAVLINK_MSG_ID_RAW_IMU: xacc: ");
            debugSerial.print(mavlink_msg_raw_imu_get_xacc(&msg));
            debugSerial.print(", yacc: ");
            debugSerial.print(mavlink_msg_raw_imu_get_yacc(&msg));
            debugSerial.print(", zacc: ");
            debugSerial.print(mavlink_msg_raw_imu_get_zacc(&msg));
            debugSerial.println();
          #endif
          break;
        /*
         * *****************************************************
         * *** MAVLINK Message #30 - Attitude                ***
         * *****************************************************
         */
        case MAVLINK_MSG_ID_ATTITUDE:
          ap_roll_angle = mavlink_msg_attitude_get_roll(&msg)*180/3.1416;  //value comes in rads, convert to deg
          // Not upside down
          if(abs(ap_roll_angle) <= 90)
          {
            ap_pitch_angle = mavlink_msg_attitude_get_pitch(&msg)*180/3.1416; //value comes in rads, convert to deg
          }
          // Upside down
          else
          {
            ap_pitch_angle = 180-mavlink_msg_attitude_get_pitch(&msg)*180/3.1416; //value comes in rads, convert to deg
          }
          ap_yaw_angle = (mavlink_msg_attitude_get_yaw(&msg)+3.1416)*162.9747; //value comes in rads, add pi and scale to 0 to 1024

          #ifdef DEBUG_APM_ATTITUDE
            debugSerial.print(millis());
            debugSerial.print("\tMAVLINK Roll Angle: ");
            debugSerial.print(mavlink_msg_attitude_get_roll(&msg));
            debugSerial.print(" (");
            debugSerial.print(ap_roll_angle);
            debugSerial.print("deg)");
            debugSerial.print("\tMAVLINK Pitch Angle: ");
            debugSerial.print(mavlink_msg_attitude_get_pitch(&msg));
            debugSerial.print(" (");
            debugSerial.print(ap_pitch_angle);
            debugSerial.print("deg)");
            debugSerial.print("\tMAVLINK Yaw Angle: ");
            debugSerial.print(mavlink_msg_attitude_get_yaw(&msg));
            debugSerial.println();
          #endif
          break;
        /*
         * *****************************************************
         * *** MAVLINK Message #33 - GLOBAL_POSITION_INT     ***
         * *****************************************************
         */
        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:   // 33
          ap_relative_alt = mavlink_msg_global_position_int_get_relative_alt(&msg)*0.1;
          #ifdef DEBUG_APM_GLOBAL_POSITION_INT
            debugSerial.print(millis());
            debugSerial.print("\tMAVLINK_MSG_ID_GLOBAL_POSITION_INT: relative_altitude: ");
            debugSerial.print(ap_relative_alt);
            debugSerial.println();
          #endif
          break;
        /*
         * *****************************************************
         * *** MAVLINK Message #63 - GLOBAL_POSITION_INT_COV ***
         * *****************************************************
         */
        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT_COV:   // 63
          ap_gps_time_unix_utc = mavlink_msg_global_position_int_cov_get_time_utc(&msg);
          #ifdef DEBUG_APM_GLOBAL_POSITION_INT_COV
            debugSerial.print(millis());
            debugSerial.print("\tap_gps_time_unix_utc: ");
            debugSerial.print(ap_gps_time_unix_utc);
            debugSerial.println();
          #endif
          break;
        /*
         * *****************************************************
         * *** MAVLINK Message #65 - RC_CHANNELS             ***
         * *****************************************************
         */
        #ifdef USE_RC_CHANNELS
        case MAVLINK_MSG_ID_RC_CHANNELS:
          ap_chancount = mavlink_msg_rc_channels_get_chancount(&msg);     // Number of RC Channels used
          ap_chan_raw[1] = mavlink_msg_rc_channels_get_chan1_raw(&msg);   // The PPM values of the RC channels received.
          ap_chan_raw[2] = mavlink_msg_rc_channels_get_chan2_raw(&msg);   // The standard PPM modulation is as follows:
          ap_chan_raw[3] = mavlink_msg_rc_channels_get_chan3_raw(&msg);   // 1000 microseconds: 0%, 2000 microseconds: 100%.
          ap_chan_raw[4] = mavlink_msg_rc_channels_get_chan4_raw(&msg);   // Individual receivers/transmitters might violate this specification.
          ap_chan_raw[5] = mavlink_msg_rc_channels_get_chan5_raw(&msg);
          ap_chan_raw[6] = mavlink_msg_rc_channels_get_chan6_raw(&msg);
          ap_chan_raw[7] = mavlink_msg_rc_channels_get_chan7_raw(&msg);
          ap_chan_raw[8] = mavlink_msg_rc_channels_get_chan8_raw(&msg);
          ap_chan_raw[9] = mavlink_msg_rc_channels_get_chan9_raw(&msg);
          ap_chan_raw[10] = mavlink_msg_rc_channels_get_chan10_raw(&msg);
          ap_chan_raw[11] = mavlink_msg_rc_channels_get_chan11_raw(&msg);
          ap_chan_raw[12] = mavlink_msg_rc_channels_get_chan12_raw(&msg);
          ap_chan_raw[13] = mavlink_msg_rc_channels_get_chan13_raw(&msg);
          ap_chan_raw[14] = mavlink_msg_rc_channels_get_chan14_raw(&msg);
          ap_chan_raw[15] = mavlink_msg_rc_channels_get_chan15_raw(&msg);
          ap_chan_raw[16] = mavlink_msg_rc_channels_get_chan16_raw(&msg);
          ap_chan_raw[17] = mavlink_msg_rc_channels_get_chan17_raw(&msg);
          ap_chan_raw[18] = mavlink_msg_rc_channels_get_chan18_raw(&msg);

          #ifdef DEBUG_APM_RC_CHANNELS
          if (millis() > RC_DEBUG_TIMEOUT) {
            RC_DEBUG_TIMEOUT = millis() + 3000;
            debugSerial.print(millis());
            debugSerial.print(" - ");
            debugSerial.print("\tMAVLINK_MSG_ID_RC_CHANNELS: chancount: ");
            debugSerial.print(ap_chancount);
            debugSerial.println();
            debugSerial.print("\tMAVLINK_MSG_ID_RC_CHANNELS: ap_chan1-4_raw: ");
            for (int i = 1; i <= 4; i++) {
              debugSerial.print(ap_chan_raw[i]);
              debugSerial.print(", ");
            }
            debugSerial.println();
            debugSerial.print("\tMAVLINK_MSG_ID_RC_CHANNELS: ap_chan5-8_raw: ");
            for (int i = 5; i <= 8; i++) {
              debugSerial.print(ap_chan_raw[i]);
              debugSerial.print(", ");
            }
            debugSerial.println();
            debugSerial.print("\tMAVLINK_MSG_ID_RC_CHANNELS: ap_chan9-12_raw: ");
            for (int i = 9; i <= 12; i++) {
              debugSerial.print(ap_chan_raw[i]);
              debugSerial.print(", ");
            }
            debugSerial.println();
            debugSerial.print("\tMAVLINK_MSG_ID_RC_CHANNELS: ap_chan13-16_raw: ");
            for (int i = 13; i <= 16; i++) {
              debugSerial.print(ap_chan_raw[i]);
              debugSerial.print(", ");
            }
            debugSerial.println();
            debugSerial.print("\tMAVLINK_MSG_ID_RC_CHANNELS: ap_chan17-18_raw: ");
            for (int i = 16; i < 18; i++) {
              debugSerial.print(ap_chan_raw[i]);
              debugSerial.print(", ");
            }
            debugSerial.println();
          }
          #endif
          break;
        #endif
        /*
         * *****************************************************
         * *** MAVLINK Message #74 - VFR_HUD                 ***
         * *****************************************************
         */
        case MAVLINK_MSG_ID_VFR_HUD:
          ap_groundspeed = mavlink_msg_vfr_hud_get_groundspeed(&msg);      // 100 = 1m/s
          ap_airspeed = mavlink_msg_vfr_hud_get_airspeed(&msg);            // 100 = 1m/s
          ap_heading = mavlink_msg_vfr_hud_get_heading(&msg);              // 100 = 100 deg
          ap_throttle = mavlink_msg_vfr_hud_get_throttle(&msg);            //  100 = 100%
          ap_bar_altitude = mavlink_msg_vfr_hud_get_alt(&msg) * 100;       //  m
          ap_climb_rate=mavlink_msg_vfr_hud_get_climb(&msg) * 100;         //  m/s
          #ifdef DEBUG_APM_VFR_HUD
            debugSerial.print(millis());
            debugSerial.print("\tMAVLINK_MSG_ID_VFR_HUD: groundspeed: ");
            debugSerial.print(ap_groundspeed);
            debugSerial.print(", heading: ");
            debugSerial.print(ap_heading);
            debugSerial.print(", throttle: ");
            debugSerial.print(ap_throttle);
            debugSerial.print(", alt: ");
            debugSerial.print(ap_bar_altitude);
            debugSerial.print(", climbrate: ");
            debugSerial.print(ap_climb_rate);
            debugSerial.println();
          #endif
          break;
        /*
         * *****************************************************
         * *** MAVLINK Message #253 - STATUSTEXT             ***
         * *****************************************************
         */
        case MAVLINK_MSG_ID_STATUSTEXT:     //253
          mavlink_msg_statustext_decode(&msg,&statustext);
          ap_status_severity = statustext.severity;
          ap_status_send_count = 1; // since messages are queued we only need one send_count
          if ( AC_VERSION == 3.3) {
            parseStatusText_v3_3(statustext.severity, statustext.text);
          } else {
            parseStatusText_v3_2(statustext.severity, statustext.text);
          }

          #ifdef DEBUG_APM_STATUSTEXT
            debugSerial.print(millis());
            debugSerial.print("\tAC_VERSION: ");
            debugSerial.print(AC_VERSION);
            debugSerial.print("\t textId: ");
            debugSerial.print(ap_status_text_id);
            debugSerial.print("\tMAVLINK_MSG_ID_STATUSTEXT - severity: ");
            debugSerial.print(statustext.severity);
            debugSerial.print(", text: ");
            debugSerial.print(statustext.text);
            debugSerial.println();
          #endif
          break;
        default:
          break;
      }
    } else if (GB_SYSID == msg.sysid && GB_CMPID == msg.compid) // only proceed with gimbal messages
    {
      /*
       * *******************************************************
       * *** GIMBAL MAVLINK Message                          ***
       * *******************************************************
       */
      switch(msg.msgid)
      {
        /*
         * *****************************************************
         * *** MAVLINK Message #0 - HEARTBEAT                ***
         * *****************************************************
         */
        case MAVLINK_MSG_ID_HEARTBEAT:  // 0
          #ifdef DEBUG_GIMBAL_HEARTBEAT
            debugSerial.print(millis());
            debugSerial.print("\tGIMBAL MESSAGE: ");
            debugSerial.print((mavlink_msg_heartbeat_get_base_mode(&msg) & 0x80) > 7);
            debugSerial.print(", custom_mode: ");
            debugSerial.print(mavlink_msg_heartbeat_get_custom_mode(&msg));
            debugSerial.println();
          #endif
          break;
      }
    } else
    {
      /*
       * *******************************************************
       * *** Other MAVLINK Message                           ***
       * *******************************************************
       */
      switch(msg.msgid)
      {
        /*
         * *****************************************************
         * *** MAVLINK Message #0 - HEARTBEAT                ***
         * *****************************************************
         */
        case MAVLINK_MSG_ID_HEARTBEAT:  // 0
          #ifdef DEBUG_OTHER_HEARTBEAT
            debugSerial.print(millis());
            debugSerial.print("\tOTHER MESSAGE: ");
            debugSerial.print("sysid: ");
            debugSerial.print(msg.sysid);
            debugSerial.print(", compid: ");
            debugSerial.print(msg.compid);
            debugSerial.print(", base_mode: ");
            debugSerial.print((mavlink_msg_heartbeat_get_base_mode(&msg) & 0x80) > 7);
            debugSerial.print(", custom_mode: ");
            debugSerial.print(mavlink_msg_heartbeat_get_custom_mode(&msg));
            debugSerial.println();
          #endif
          break;
      }
    }
  }
}
