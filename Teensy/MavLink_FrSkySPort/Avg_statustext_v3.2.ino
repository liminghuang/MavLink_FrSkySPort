/*
 * statustext_v3.2.ino part of MavLink_FrSkySPort
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
void parseStatusText_v3_2(int32_t severity, String text)
{
  uint16_t textId = 0;

  // Texts with textId = 0 will be ignored

  // motors.pde
       if(text == "ARMING MOTORS")                                         textId = 1;
  else if(text == "PreArm: RC not calibrated")                             textId = 2;
  else if(text.startsWith("PreArm: Baro not healthy"))                     textId = 3;
  else if(text == "PreArm: Alt disparity")                                 textId = 4;
  else if(text.startsWith("PreArm: Compass not healthy"))                  textId = 5;
  else if(text == "PreArm: Compass not calibrated")                        textId = 6;
  else if(text == "PreArm: Compass offsets too high")                      textId = 7;
  else if(text == "PreArm: Check mag field")                               textId = 8;
  else if(text == "PreArm: INS not calibrated")                            textId = 9;
  else if(text == "PreArm: INS not healthy")                               textId = 10;
  else if(text == "PreArm: Check Board Voltage")                           textId = 11;
  else if(text == "PreArm: Ch7&Ch8 Opt cannot be same")                    textId = 12;
  else if(text == "PreArm: Check FS_THR_VALUE")                            textId = 13;
  else if(text == "PreArm: Check ANGLE_MAX")                               textId = 14;
  else if(text == "PreArm: ACRO_BAL_ROLL/PITCH")                           textId = 15;
  else if(text == "PreArm: GPS Glitch")                                    textId = 16;
  else if(text == "PreArm: Need 3D Fix")                                   textId = 17;
  else if(text == "PreArm: Bad Velocity")                                  textId = 18;
  else if(text == "PreArm: High GPS HDOP")                                 textId = 19;
  else if(text == "PreArm: Accels inconsistent")                           textId = 111;
  else if(text == "PreArm: Accels not healthy")                            textId = 112;
  else if(text == "PreArm: Bad GPS Pos")                                   textId = 113;
  else if(text == "PreArm: Battery failsafe on.")                          textId = 114;
  else if(text == "PreArm: compasses inconsistent")                        textId = 115;
  else if(text == "PreArm: Gyro cal failed")                               textId = 116;
  else if(text == "PreArm: Gyros inconsistent")                            textId = 117;
  else if(text == "PreArm: Gyros not healthy")                             textId = 118;
  else if(text == "PreArm: Radio failsafe on.")                            textId = 119;
  else if(text == "Arm: Alt disparity")                                    textId = 20;
  else if(text == "Arm: Thr below FS")                                     textId = 21;
  else if(text == "Arm: Leaning")                                          textId = 22;
  else if(text == "Arm: Safety Switch")                                    textId = 23;
  else if(text == "Arm: Mode not armable")                                 textId = 100;
  else if(text == "Arm: Rotor not spinning")                               textId = 101;
  else if(text == "Arm: Thr too high")                                     textId = 102;
  else if(text == "DISARMING MOTORS")                                      textId = 89;
  else if(text == "Throttle armed!")                                       textId = 120;
  else if(text == "Throttle disarmed!")                                    textId = 121;

  // plane/copter sensors.pde
  else if(text == "Calibrating barometer")                                 textId = 90;
  else if(text == "barometer calibration complete")                        textId = 91;
  else if(text == "zero airspeed calibrated")                              textId = 92;

  // control_autotune.pde
  else if(text == "AutoTune: Started")                                     textId = 24;
  else if(text == "AutoTune: Stopped")                                     textId = 25;
  else if(text == "AutoTune: Success")                                     textId = 26;
  else if(text == "AutoTune: Failed")                                      textId = 27;

  // crash_check.pde
  else if(text == "Crash: Disarming")                                      textId = 28;
  else if(text == "Parachute: Released!")                                  textId = 29;
  else if(text == "Parachute: Too Low")                                    textId = 30;

  // efk_check.pde
  else if(text == "EKF variance")                                          textId = 31;
  else if(text == "DCM bad heading")                                       textId = 125;

  // events.pde
  else if(text == "Low Battery!")                                          textId = 32;
  else if(text == "Lost GPS!")                                             textId = 33;

  // switches.pde
  else if(text == "Trim saved")                                            textId = 34;

  // Compassmot.pde
  else if(text == "compass disabled\n")                                    textId = 35;
  else if(text == "check compass")                                         textId = 36;
  else if(text == "RC not calibrated")                                     textId = 37;
  else if(text == "thr not zero")                                          textId = 38;
  else if(text == "Not landed")                                            textId = 39;
  else if(text == "STARTING CALIBRATION")                                  textId = 40;
  else if(text == "CURRENT")                                               textId = 41;
  else if(text == "THROTTLE")                                              textId = 42;
  else if(text == "Calibration Successful!")                               textId = 43;
  else if(text == "Failed!")                                               textId = 44;

  // copter/plane GCS_Mavlink.pde
  else if(text == "bad rally point message ID")                            textId = 45;
  else if(text == "bad rally point message count")                         textId = 46;
  else if(text == "error setting rally point")                             textId = 47;
  else if(text == "bad rally point index")                                 textId = 48;
  else if(text == "failed to set rally point")                             textId = 49;
  else if(text == "Initialising APM...")                                   textId = 93;

  // copter/plane Log.pde
  else if(text.startsWith("Erasing logs"))                                 textId = 50;
  else if(text.startsWith("Log erase complete"))                           textId = 51;

  // motor_test.pde
  else if(text == "Motor Test: RC not calibrated")                         textId = 52;
  else if(text == "Motor Test: vehicle not landed")                        textId = 53;
  else if(text == "Motor Test: Safety Switch")                             textId = 54;

  // plane/copter system.pde
  else if(text == "No dataflash inserted")                                 textId = 55;
  else if(text == "No dataflash card inserted")                            textId = 55; // Duplicate
  else if(text == "ERASING LOGS")                                          textId = 56;
  else if(text == "Waiting for first HIL_STATE message")                   textId = 57;
  else if(text == "GROUND START")                                          textId = 94;
  else if(text == "<startup_ground> GROUND START")                         textId = 95;
  else if(text == "<startup_ground> With Delay")                           textId = 96;
  else if(text.endsWith("Ready to FLY."))                                  textId = 61;
  else if(text == "Beginning INS calibration; do not move plane")          textId = 97;
  else if(text == "NO airspeed")                                           textId = 62;

  // AntennaTracker GCS_Mavnlink.pde
  else if(text == "command received: ")                                    textId = 59;
  else if(text == "new HOME received")                                     textId = 60;

  // AntennaTracker system.pde
  else if(text.endsWith("Ready to track.  "))                              textId = 98;
  else if(text == "Beginning INS calibration; do not move tracker")        textId = 99;

  // Arduplane.pde
  else if(text == "Disable fence failed (autodisable)")                    textId = 63;
  else if(text == "Fence disabled (autodisable)")                          textId = 64;
  else if(text.startsWith("FBWA tdrag mode"))                              textId = 110;

  // Arduplane attitude.pde
  else if(text == "Demo Servos!")                                          textId = 65;

  // Arduplane commands.pde
  else if(text == "Resetting prev_WP")                                     textId = 66;
  else if(text == "init home")                                             textId = 67;
  else if(text == "Fence enabled. (autoenabled)")                          textId = 68;
  else if(text == "verify_nav: LOITER time complete")                      textId = 69;
  else if(text == "verify_nav: LOITER orbits complete")                    textId = 70;
  else if(text == "Reached home")                                          textId = 71;

  // Arduplane events.pde
  else if(text == "Failsafe - Short event on, ")                           textId = 72;
  else if(text == "Failsafe - Long event on, ")                            textId = 73;
  else if(text == "No GCS heartbeat.")                                     textId = 74;
  else if(text == "Failsafe - Short event off")                            textId = 75;

  // Arduplane GCS_Mavlink.pde
  else if(text == "command received: ")                                    textId = 76;
  else if(text == "fencing must be disabled")                              textId = 77;
  else if(text == "bad fence point")                                       textId = 78;

  // Arduplane commands_logic.pde
  else if(text == "verify_nav: Invalid or no current Nav cmd")             textId = 79;
  else if(text == "verify_conditon: Invalid or no current Condition cmd")  textId = 80;
  else if(text == "Enable fence failed (cannot autoenable")                textId = 81;
  else if(text == "verify_conditon: Unsupported command")                  textId = 124;

  // Arduplane geofence.pde
  else if(text == "geo-fence loaded")                                      textId = 82;
  else if(text == "geo-fence setup error")                                 textId = 83;
  else if(text == "geo-fence OK")                                          textId = 84;
  else if(text == "geo-fence triggered")                                   textId = 85;

  // Rover steering.pde
  else if(text == "AUTO triggered off")                                    textId = 103;
  else if(text == "Triggered AUTO with pin")                               textId = 122;

  // Rover system.pde
  else if(text == "Beginning INS calibration; do not move vehicle")        textId = 104;
  else if(text == "Warming up ADC...")                                     textId = 123;

  // Arducopter esc_calibraton.pde
  else if(text == "ESC Cal: auto calibration")                             textId = 105;
  else if(text == "ESC Cal: passing pilot thr to ESCs")                    textId = 106;
  else if(text == "ESC Cal: push safety switch")                           textId = 107;
  else if(text == "ESC Cal: restart board")                                textId = 108;

  // Arduplane takeoff.pde
  else if(text == "FBWA tdrag off")                                        textId = 109;

  // Arducopter commands_logic.pde
  else if(text.startsWith("Reached Command #"))                            textId = 88;

  // Libraries GCS_Common.cpp
  else if(text == "flight plan update rejected")                           textId = 86;
  else if(text == "flight plan received")                                  textId = 87;

  // Gcs_Mavlink.pde
  else if(text.startsWith("Frame: "))                                      textId = 0;

  // System version (received when connecting Mission planner)
  else if(text.startsWith("ArduCopter V"))                                 textId = 0;
  else if(text.startsWith("ArduPlane V"))                                  textId = 0;
  else if(text.startsWith("PX4: "))                                        textId = 0;

  // Last used id: 125
  // Unknown text (textId = 1023)
  else                                                                     textId = 1023;
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
