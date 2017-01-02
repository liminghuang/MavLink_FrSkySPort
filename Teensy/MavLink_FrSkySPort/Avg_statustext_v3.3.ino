/*
 * statustext_v3.3.ino part of MavLink_FrSkySPort
 * https://github.com/Clooney82/MavLink_FrSkySPort
 *
 * Copyright (C) 2015 Jochen Kielkopf
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
void parseStatusText_v3_3(int32_t severity, String text)
{
  uint16_t textId = 0;

  // Texts with textId = 0 will be ignored

  if(text == "")                                                          textId = 0;

  //APMrover2/GCS_Mavlink.cpp
  else if(text == "Initialising APM...")                                                            textId = 1;
  else if(text == "Unsupported preflight calibration")                                                            textId = 2;
  else if(text == "command received:")                                                            textId = 3;

  //APMrover2/Log.cpp
  else if(text == "ERASING LOGS")                                                            textId = 4;
  else if(text == "No dataflash card inserted")                                                            textId = 5;

  //APMrover2/Steering.cpp
  else if(text == "AUTO triggered off")                                                            textId = 6;
  else if(text == "Triggered AUTO with pin")                                                            textId = 7;
  else if(text.startsWith("Triggered AUTO"))                                                   textId = 8;

  //APMrover2/commands.cpp
  else if(text == "Resetting prev_WP")                                                            textId = 9;
  else if(text == "init home")                                                            textId = 10;

  //APMrover2/commands_logic.cpp
  else if(text == "verify_conditon: Unsupported command")                                                            textId = 11;
  else if(text == "Reached Destination")                                                            textId = 12;
  else if(text.startsWith("Cruise speed:"))                                                   textId = 13;
  else if(text.startsWith("Cruise throttle:"))                                                   textId = 14;
  else if(text.startsWith("Executing command ID"))                                                   textId = 15;
  else if(text == "No commands. Can't set AUTO - setting HOLD")                                                            textId = 16;
  else if(text.startsWith("Passed Waypoint"))                                                   textId = 17;
  else if(text.startsWith("Reached Destination: Distance away"))                                                   textId = 18;
  else if(text.startsWith("Reached Waypoint"))                                                   textId = 19;

  //APMrover2/navigation.cpp
  else if(text == "<navigate> WP error - distance < 0")                                                            textId = 20;

  //APMrover2/sensors.cpp
  else if(text == "Calibrating barometer")                                                            textId = 21;
  else if(text == "barometer calibration complete")                                                            textId = 22;
  else if(text == "Obstacle passed")                                                            textId = 23;
  else if(text.startsWith("Sonar obstacle"))                                                   textId = 24;
  else if(text.startsWith("Sonar1 obstacle"))                                                   textId = 25;
  else if(text.startsWith("Sonar2 obstacle"))                                                   textId = 26;

  //APMrover2/system.cpp
  else if(text == "<startup_ground> GROUND START")                                                            textId = 27;
  else if(text == "<startup_ground> With Delay")                                                            textId = 28;
  else if(text.endsWith("Ready to drive."))                                                   textId = 29;
  else if(text == "Beginning INS calibration")                                                            textId = 30;
  else if(text == "Warming up ADC...")                                                            textId = 31;
  else if(text == "Failsafe ended")                                                            textId = 32;
  else if(text.startsWith("Failsafe trigger"))                                                   textId = 33;

  //AntennaTracker/GCS_Mavlink.cpp
  else if(text == "new HOME received")                                                            textId = 34;

  //AntennaTracker/system.cpp
  else if(text.endsWith("Ready to track."))                                                   textId = 35;

  //ArduCopter/GCS_Mavlink.cpp
  else if(text == "error setting rally point")                                                            textId = 36;
  else if(text == "bad rally point index")                                                            textId = 37;
  else if(text == "bad rally point message ID")                                                            textId = 38;
  else if(text == "bad rally point message count")                                                            textId = 39;
  else if(text == "failed to set rally point")                                                            textId = 40;

  //ArduCopter/Log.cpp
  else if(text.startsWith("Erasing logs"))                                                   textId = 41;
  else if(text.startsWith("Log erase complete"))                                                   textId = 42;
  else if(text == "No dataflash inserted")                                                            textId = 43;

  //ArduCopter/commands_logic.cpp
  else if(text.startsWith("Reached Command"))                                                   textId = 44;

  //ArduCopter/compassmot.cpp
  else if(text == "CURRENT")                                                            textId = 45;
  else if(text == "Calibration Successful!")                                                            textId = 46;
  else if(text == "Failed!")                                                            textId = 47;
  else if(text == "Not landed")                                                            textId = 48;
  else if(text == "RC not calibrated")                                                            textId = 49;
  else if(text == "STARTING CALIBRATION")                                                            textId = 50;
  else if(text == "THROTTLE")                                                            textId = 51;
  else if(text == "check compass")                                                            textId = 52;
  else if(text.startsWith("compass disabled"))                                                   textId = 53;
  else if(text == "thr not zero")                                                            textId = 54;

  //ArduCopter/control_autotune.cpp
  else if(text == "AutoTune: Failed")                                                            textId = 55;
  else if(text == "AutoTune: Saved Gains")                                                            textId = 56;
  else if(text == "AutoTune: Started")                                                            textId = 57;
  else if(text == "AutoTune: Stopped")                                                            textId = 58;
  else if(text == "AutoTune: Success")                                                            textId = 59;

  //ArduCopter/crash_check.cpp
  else if(text == "Crash: Disarming")                                                            textId = 60;
  else if(text == "Parachute: Landed")                                                            textId = 61;
  else if(text == "Parachute: Released!")                                                            textId = 62;
  else if(text == "Parachute: Too Low")                                                            textId = 63;

  //ArduCopter/ekf_check.cpp
  else if(text == "EKF variance")                                                            textId = 64;

  //ArduCopter/esc_calibration.cpp
  else if(text == "ESC Calibration: auto calibration")                                                            textId = 65;
  else if(text == "ESC Calibration: passing pilot throttle to ESCs")                                                            textId = 66;
  else if(text == "ESC Calibration: push safety switch")                                                            textId = 67;
  else if(text == "ESC Calibration: restart board")                                                            textId = 68;

  //ArduCopter/events.cpp
  else if(text == "Low Battery!")                                                            textId = 69;

  //ArduCopter/motor_test.cpp
  else if(text == "Motor Test: RC not calibrated")                                                            textId = 70;
  else if(text == "Motor Test: Safety Switch")                                                            textId = 71;
  else if(text == "Motor Test: vehicle not landed")                                                            textId = 72;

  //ArduCopter/motors.cpp
  else if(text == "ARMING MOTORS")                                                            textId = 73;
  else if(text == "Arm: Accelerometers not healthy")                                                            textId = 74;
  else if(text == "Arm: Altitude disparity")                                                            textId = 75;
  else if(text == "Arm: Barometer not healthy")                                                            textId = 76;
  else if(text == "Arm: Check Battery")                                                            textId = 77;
  else if(text == "Arm: Collective below Failsafe")                                                            textId = 78;
  else if(text == "Arm: Collective too high")                                                            textId = 79;
  else if(text == "Arm: Gyro calibration failed")                                                            textId = 80;
  else if(text == "Arm: Gyros not healthy")                                                            textId = 81;
  else if(text == "Arm: Leaning")                                                            textId = 82;
  else if(text == "Arm: Mode not armable")                                                            textId = 83;
  else if(text == "Arm: Motor Emergency Stopped")                                                            textId = 84;
  else if(text == "Arm: Motor Interlock Enabled")                                                            textId = 85;
  else if(text == "Arm: Rotor Control Engaged")                                                            textId = 86;
  else if(text == "Arm: Safety Switch")                                                            textId = 87;
  else if(text == "Arm: Throttle below Failsafe")                                                            textId = 88;
  else if(text == "Arm: Throttle too high")                                                            textId = 89;
  else if(text == "Arm: Waiting for Nav Checks")                                                            textId = 90;
  else if(text == "Arm: check fence")                                                            textId = 91;
  else if(text == "DISARMING MOTORS")                                                            textId = 92;
  else if(text == "Locate Copter Alarm!")                                                            textId = 93;
  else if(text == "PreArm: ACRO_BAL_ROLL/PITCH")                                                            textId = 94;
  else if(text == "PreArm: Accelerometers not healthy")                                                            textId = 95;
  else if(text == "PreArm: Accels not calibrated")                                                            textId = 96;
  else if(text == "PreArm: Altitude disparity")                                                            textId = 97;
  else if(text == "PreArm: Barometer not healthy")                                                            textId = 98;
  else if(text == "PreArm: Check ANGLE_MAX")                                                            textId = 99;
  else if(text == "PreArm: Check Battery")                                                            textId = 100;
  else if(text == "PreArm: Check Board Voltage")                                                            textId = 101;
  else if(text == "PreArm: Check FS_THR_VALUE")                                                            textId = 102;
  else if(text == "PreArm: Check Heli Parameters")                                                            textId = 103;
  else if(text == "PreArm: Check mag field")                                                            textId = 104;
  else if(text == "PreArm: Collective below Failsafe")                                                            textId = 105;
  else if(text == "PreArm: Compass not calibrated")                                                            textId = 106;
  else if(text == "PreArm: Compass not healthy")                                                            textId = 107;
  else if(text == "PreArm: Compass offsets too high")                                                            textId = 108;
  else if(text == "PreArm: Duplicate Aux Switch Options")                                                            textId = 109;
  else if(text == "PreArm: EKF compass variance")                                                            textId = 110;
  else if(text == "PreArm: EKF-home variance")                                                            textId = 111;
  else if(text == "PreArm: Gyros not healthy")                                                            textId = 112;
  else if(text == "PreArm: High GPS HDOP")                                                            textId = 113;
  else if(text == "PreArm: Interlock/E-Stop Conflict")                                                            textId = 114;
  else if(text == "PreArm: Motor Emergency Stopped")                                                            textId = 115;
  else if(text == "PreArm: Motor Interlock Enabled")                                                            textId = 116;
  else if(text == "PreArm: Need 3D Fix")                                                            textId = 117;
  else if(text == "PreArm: RC not calibrated")                                                            textId = 118;
  else if(text == "PreArm: Throttle below Failsafe")                                                            textId = 119;
  else if(text == "PreArm: Waiting for Nav Checks")                                                            textId = 120;
  else if(text == "PreArm: check fence")                                                            textId = 121;
  else if(text == "PreArm: check range finder")                                                            textId = 122;
  else if(text == "PreArm: inconsistent Accelerometers")                                                            textId = 123;
  else if(text == "PreArm: inconsistent Gyros")                                                            textId = 124;
  else if(text == "PreArm: inconsistent compasses")                                                            textId = 125;

  //ArduCopter/switches.cpp
  else if(text == "Trim saved")                                                            textId = 126;

  //ArduCopter/system.cpp
  else if(text == "GROUND START")                                                            textId = 127;
  else if(text == "Waiting for first HIL_STATE message")                                                            textId = 128;

  //ArduPlane/ArduPlane.cpp
  else if(text == "Disable fence failed (autodisable)")                                                            textId = 129;
  else if(text == "Disable fence floor failed (autodisable)")                                                            textId = 130;
  else if(text == "Fence disabled (autodisable)")                                                            textId = 131;
  else if(text == "Fence floor disabled (auto disable)")                                                            textId = 132;
  else if(text.startsWith("FBWA tdrag mode"))                                                   textId = 133;

  //ArduPlane/Attitude.cpp
  else if(text == "Demo Servos!")                                                            textId = 134;
  else if(text.startsWith("Throttle unsuppressed - altitude"))                                                   textId = 135;
  else if(text.startsWith("Throttle unsuppressed - speed"))                                                   textId = 136;

  //ArduPlane/GCS_Mavlink.cpp
  else if(text == "Fence floor disabled.")                                                            textId = 137;
  else if(text == "Go around command accepted.")                                                            textId = 138;
  else if(text == "Rejected go around command.")                                                            textId = 139;
  else if(text == "bad fence point")                                                            textId = 140;
  else if(text == "fencing must be disabled")                                                            textId = 141;
  else if(text.startsWith("set home to"))                                                   textId = 142;

  //ArduPlane/Log.cpp
  else if(text == "Erasing logs")                                                            textId = 143;
  else if(text == "Log erase complete")                                                            textId = 144;

  //ArduPlane/arming_checks.cpp
  else if(text == "PreArm: LIM_PITCH_MAX too small")                                                            textId = 145;
  else if(text == "PreArm: LIM_PITCH_MIN too large")                                                            textId = 146;
  else if(text == "PreArm: LIM_ROLL_CD too small")                                                            textId = 147;
  else if(text == "PreArm: invalid THR_FS_VALUE for rev throttle")                                                            textId = 148;

  //ArduPlane/commands.cpp
  else if(text.startsWith("gps alt:"))                                                   textId = 149;

  //ArduPlane/commands_logic.cpp
  else if(text == "Enable fence failed (cannot autoenable")                                                            textId = 150;
  else if(text == "Fence enabled. (autoenabled)")                                                            textId = 151;
  else if(text == "verify_conditon: Invalid or no current Condition cmd")                                                            textId = 152;
  else if(text == "verify_nav: Invalid or no current Nav cmd")                                                            textId = 153;
  else if(text == "Reached altitude")                                                            textId = 154;
  else if(text == "Reached home")                                                            textId = 155;
  else if(text == "verify_nav: LOITER orbits complete")                                                            textId = 156;
  else if(text == "verify_nav: LOITER time complete")                                                            textId = 157;
  else if(text.startsWith("Executing nav command ID"))                                                   textId = 158;
  else if(text.startsWith("Fence floor disabled."))                                                   textId = 159;
  else if(text.startsWith("Holding course"))                                                   textId = 160;
  else if(text.startsWith("Reached descent rate"))                                                   textId = 161;
  else if(text == "Returning to Home")                                                            textId = 162;
  else if(text.startsWith("Set airspeed"))                                                   textId = 163;
  else if(text.startsWith("Set fence enabled state to"))                                                   textId = 164;
  else if(text.startsWith("Set groundspeed"))                                                   textId = 165;
  else if(text.startsWith("Set inverted"))                                                   textId = 166;
  else if(text.startsWith("Set throttle"))                                                   textId = 167;
  else if(text.startsWith("Takeoff complete at"))                                                   textId = 168;
  else if(text.startsWith("Unable to set fence enabled state to"))                                                   textId = 169;
  else if(text.startsWith("Unabled to disable fence floor."))                                                   textId = 170;

  //ArduPlane/control_modes.cpp
  else if(text == "PX4IO Override disabled")                                                            textId = 171;
  else if(text == "PX4IO Override enable failed")                                                            textId = 172;
  else if(text == "PX4IO Override enabled")                                                            textId = 173;

  //ArduPlane/events.cpp
  else if(text == "No GCS heartbeat.")                                                            textId = 174;
  else if(text == "Failsafe - Long event on")                                                            textId = 175;
  else if(text == "Failsafe - Short event off")                                                            textId = 176;
  else if(text == "Failsafe - Short event on")                                                            textId = 177;
  else if(text.startsWith("Low Battery"))                                                   textId = 178;

  //ArduPlane/geofence.cpp
  else if(text == "geo-fence setup error")                                                            textId = 179;
  else if(text == "geo-fence OK")                                                            textId = 180;
  else if(text == "geo-fence loaded")                                                            textId = 181;
  else if(text == "geo-fence triggered")                                                            textId = 182;

  //ArduPlane/landing.cpp
  else if(text == "Unable to start landing sequence.")                                                            textId = 183;
  else if(text == "Auto-Disarmed")                                                            textId = 184;
  else if(text == "Landing sequence begun.")                                                            textId = 185;
  else if(text.startsWith("Distance from LAND point"))                                                   textId = 186;
  else if(text.startsWith("Flare crash detected:"))                                                   textId = 187;
  else if(text.startsWith("Flare"))                                                   textId = 188;

  //ArduPlane/radio.cpp
  else if(text.startsWith("MSG FS OFF"))                                                   textId = 189;
  else if(text.startsWith("MSG FS ON"))                                                   textId = 190;

  //ArduPlane/sensors.cpp
  else if(text == "zero airspeed calibrated")                                                            textId = 191;

  //ArduPlane/system.cpp
  else if(text == "NO airspeed")                                                            textId = 192;
  else if(text.endsWith("Ready to FLY."))                                                   textId = 193;
  else if(text == "Beginning INS calibration")                                                            textId = 194;

  //ArduPlane/takeoff.cpp
  else if(text == "FBWA tdrag off")                                                            textId = 195;
  else if(text.startsWith("Armed AUTO"))                                                   textId = 196;
  else if(text == "Bad Launch AUTO")                                                            textId = 197;
  else if(text == "Timeout AUTO")                                                            textId = 198;
  else if(text == "Timer Interrupted AUTO")                                                            textId = 199;

  //libraries/APM_OBC/APM_OBC.cpp
  else if(text == "Dual loss TERMINATE")                                                            textId = 200;
  else if(text == "Fence TERMINATE")                                                            textId = 201;
  else if(text == "GCS OK")                                                            textId = 202;
  else if(text == "GPS OK")                                                            textId = 203;
  else if(text == "RC failure terminate")                                                            textId = 204;
  else if(text == "Starting AFS_AUTO")                                                            textId = 205;
  else if(text == "State DATA_LINK_LOSS")                                                            textId = 206;
  else if(text == "State GPS_LOSS")                                                            textId = 207;

  //libraries/AP_Arming/AP_Arming.cpp
  else if(text == "PreArm: 3D accel cal needed")                                                            textId = 208;
  else if(text == "PreArm: AHRS not healthy!")                                                            textId = 209;
  else if(text == "PreArm: Bad GPS Position")                                                            textId = 210;
  else if(text == "PreArm: Barometer not healthy!")                                                            textId = 211;
  else if(text == "PreArm: Battery failsafe on.")                                                            textId = 212;
  else if(text == "PreArm: Compass not healthy!")                                                            textId = 213;
  else if(text == "PreArm: Hardware Safety Switch")                                                            textId = 214;
  else if(text == "PreArm: Radio failsafe on")                                                            textId = 215;
  else if(text == "PreArm: accels not healthy!")                                                            textId = 216;
  else if(text == "PreArm: airspeed not healthy")                                                            textId = 217;
  else if(text == "PreArm: gyros not calibrated!")                                                            textId = 218;
  else if(text == "PreArm: gyros not healthy!")                                                            textId = 219;
  else if(text == "PreArm: inconsistent gyros")                                                            textId = 220;
  else if(text == "PreArm: logging not available")                                                            textId = 221;
  else if(text == "Throttle armed!")                                                            textId = 222;
  else if(text == "Throttle disarmed!")                                                            textId = 223;

  //libraries/GCS_MAVLink/GCS_Common.cpp
  else if(text == "flight plan received")                                                            textId = 224;
  else if(text == "flight plan update rejected")                                                            textId = 225;

  //modules/PX4Firmware/src/modules/mavlink/mavlink_ftp.cpp
  else if(text == "FTP: can't open path (file system corrupted?)")                                                            textId = 226;
  else if(text == "FTP: list readdir_r failure")                                                            textId = 227;

  //modules/PX4Firmware/src/modules/mavlink/mavlink_main.cpp
  else if(text == "Save params and reboot to change COMPID")                                                            textId = 228;
  else if(text == "Save params and reboot to change SYSID")                                                            textId = 229;

  //modules/PX4Firmware/src/modules/mavlink/mavlink_mission.cpp
  else if(text == "ERROR: Waypoint index exceeds list capacity")                                                            textId = 230;
  else if(text == "ERROR: can't save mission state")                                                            textId = 231;
  else if(text == "ERROR: wp index out of bounds")                                                            textId = 232;
  else if(text == "IGN MISSION_ITEM: Busy")                                                            textId = 233;
  else if(text == "IGN MISSION_ITEM: No transfer")                                                            textId = 234;
  else if(text == "IGN MISSION_ITEM_REQUEST: No active transfer")                                                            textId = 235;
  else if(text == "IGN REQUEST LIST: Busy")                                                            textId = 236;
  else if(text == "Operation timeout")                                                            textId = 237;
  else if(text == "REJ. WP CMD: partner id mismatch")                                                            textId = 238;
  else if(text == "Unable to read from micro SD")                                                            textId = 239;
  else if(text == "Unable to write on micro SD")                                                            textId = 240;
  else if(text == "WPM: ERR: not all items sent -> IDLE")                                                            textId = 241;
  else if(text == "WPM: IGN CLEAR CMD: Busy")                                                            textId = 242;
  else if(text == "WPM: IGN MISSION_COUNT: Busy")                                                            textId = 243;
  else if(text == "WPM: IGN WP CURR CMD: Busy")                                                            textId = 244;
  else if(text == "WPM: REJ. CMD: Busy")                                                            textId = 245;
  else if(text == "WPM: REJ. CMD: Req. WP was unexpected")                                                            textId = 246;
  else if(text == "WPM: REJ. CMD: partner id mismatch")                                                            textId = 247;
  else if(text == "WPM: WP CURR CMD: Error setting ID")                                                            textId = 248;
  else if(text == "WPM: WP CURR CMD: Not in list")                                                            textId = 249;
  else if(text == "WP CMD OK TRY AGAIN")                                                            textId = 250;
  else if(text == "WPM: COUNT 0: CLEAR MISSION")                                                            textId = 251;
  else if(text == "WPM: Transfer complete.")                                                            textId = 252;
  else if(text == "WPM: mission is empty")                                                            textId = 253;

  //ArduCopter/motors.cpp
  else if(text.startsWith("PreArm:"))                                                   textId = 254;

  // Unknown text (textId = 1023)
  else                                                                    textId = 1023;

  ap_status_text_id = textId;

  #ifdef DEBUG_APM_PARSE_STATUSTEXT
    debugSerial.print(millis());
    debugSerial.print("\tparseStatusText. severity: ");
    debugSerial.print(severity);
    debugSerial.print(", text: \"");
    debugSerial.print(text);
    debugSerial.print("\" textId: ");
    debugSerial.print(textId);
    debugSerial.println();
  #endif
}
