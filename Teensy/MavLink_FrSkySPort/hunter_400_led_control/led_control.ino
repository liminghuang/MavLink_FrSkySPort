/*
 * led_control.ino part of MavLink_FrSkySPort
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
 * ====================================================================================================
 *
 * Lighting Controller for ArduCopter based on Teensy 3.1 and
 * APM2.5 Mavlink to FrSky X8R SPort interface using Teensy 3.1
 *
 * Teensy 3.1 PIN 6 is used for controlling WS2812B LEDs
 * Keep in mind teensy only provide 3.3V Output, to control WS2812B LEDs you need to shift this to 5V.
 * possible Solution:  *1: https://www.pjrc.com/teensy/td_libs_OctoWS2811.html
 *                     *2: http://forums.adafruit.com/viewtopic.php?f=47&t=48908&p=246902#p246902
 *
 * To Use this script in Simulation Mode (#define SIMULATION_MODE)
 * In SIMULATION_MODE script can run stand alone on your development environment, but no input is possible.
 */
#ifdef USE_TEENSY_LED_SUPPORT
//#####################################################################################################
//### MAIN CONFIG PART                                                                              ###
//#####################################################################################################
//### INCLUDES                                                                                      ###
//#####################################################################################################
#include <FastLED.h>

//#define SIMULATION_MODE // uncomment to enable simulation mode
//#define standalone      // DO NOT UNCOMMENT, just for testing purpose as standalone script without mavlink part.

//#####################################################################################################
//### DEBUG OPTIONS                                                                                 ###
//#####################################################################################################
//#define DEBUG_LANDUNG
//#define DEBUG_ROTOR_LIGHTS

//#####################################################################################################
//### DEFAULT VARIABLES                                                                             ###
//#####################################################################################################
#define RIGHT 0
#define LEFT  1

#define OFF   0
#define ON    1
#define AUTO  2
#define BLINK 3

#define PWM_MIN    982  // minimum pwm value of your RC-System
#define PWM_MID   1496  // middle  pwm value of your RC-System
#define PWM_MAX   2006  // maximum pwm value of your RC-System

#ifdef SIMULATION_MODE
  int LED_MODE_SWITCH;
  int LED_DIMM_CHAN;
#else
  #ifdef USE_RC_CHANNELS
    #define LED_MODE_SWITCH 10   // Channel Number of RC Channel used for manual Lightmode
    #define LED_DIMM_CHAN   11   // Channel Number of RC Channel used for dimming LEDs
  #else
    int LED_MODE_SWITCH;
    int LED_DIMM_CHAN;
    int ap_chan_raw[18];
  #endif
#endif


//#####################################################################################################
//### FASTLED CONFIG                                                                                ###
//#####################################################################################################
#define DATA_PIN  6
#define RGBORDER  GRB
#define LEDTYPE   WS2812B


//#####################################################################################################
//### LED STRIP CONFIG                                                                              ###
//#####################################################################################################
//### NUMBER OF LEDs                                                                                ###
//#####################################################################################################
#define NUM_ARMS           4  // Number of Arms (4 or 6)
#define NUM_LEDS_TOTAL    80  // Sum of all LEDs

#define NUM_LEDS_GPS          1 // Number of LEDs used for GPS-STATUS
#define NUM_LEDS_ARMED        1 // Number of LEDs used for ARMED-STATUS
#define NUM_LEDS_FRONTARM_F   6 // Number of LEDs on FRONTSIDE of each front arm
#define NUM_LEDS_FRONTARM_R   4 // Number of LEDs on BACKSIDE  of each front arm
#define NUM_LEDS_FRONTLIGHT   6 // Number of Frontlight LEDs
#define NUM_LEDS_BREAKLIGHT   3 // Number of Breaklight LEDs
#define NUM_LEDS_REARARMS     3 // Number of LEDs on each rear arm
#define NUM_LEDS_SIDEARMS     3 // Number of LEDs on each side/middle arm
#define NUM_LEDS_FLASH        3 // Number of FLASH_LIGHT_LEDs (on each Motor)
#define NUM_LEDS_LANDING1     3 // Number of LEDs used for LANDING-LIGHT 1
#define NUM_LEDS_LANDING2     4 // Number of LEDs used for LANDING-LIGHT 4

//#####################################################################################################
//### LED ORDER CONFIG                                                                              ###
//#####################################################################################################
int POS_LEDS_REARARMS[4];
int POS_LEDS_FLASH[4];
int POS_LEDS_SIDEARMS[2];
int POS_LEDS_FRONTARM_R[2];
int POS_LEDS_FRONTARM_F[2];
int POS_LEDS_LANDING[5];
int POS_LEDS_ARMED;
int POS_LEDS_GPS;
int POS_LEDS_BREAKLIGHT;
int POS_LEDS_FRONTLIGHT;


//#####################################################################################################
//### Define some variables                                                                         ###
//#####################################################################################################

CRGB leds[NUM_LEDS_TOTAL];
int LED_MODE = 999;
int FL_RUN = 0;

uint8_t ap_base_mode_last = 0;
int32_t ground_level = 0;

#ifdef SIMULATION_MODE
  int SIM_LOOP;
  unsigned long targetmillisfc_sim_mode = 0;
  #ifdef standalone
    int ap_base_mode;
    int ap_bar_altitude;
    int ap_sat_visible;
    int ap_fixtype;
    int ap_custom_mode;
    int ap_throttle;
    int ap_chancount;
    int ap_chan_raw[18];
    int ap_rssi;
  #endif
#endif

unsigned long currentmillis;      // current milliseconds
unsigned long lastmillis;         // milliseconds of last loop
unsigned long targetmillis_FLASH; // target milliseconds
unsigned long targetmillis_GPS;
unsigned long targetmillis_LS;
unsigned long targetmillis_BREAK;
unsigned long targetmillis_RL;

int state_GPS;
int state_FLASH;
int state_BREAK;
int pos_RL;

int pos;
int dir = RIGHT;

/*
 * LED_COLORS
 * CRGB::BLACK   <=> CHSV(0,0,0)
 * CRGB::WHITE   <=> CHSV(0,0,255)
 * CRGB::RED   <=> CHSV(0,255,255)
 * CRGB::GREEN   <=> CHSV(120,255,255)
 * CRGB::YELLOW  <=> CHSV(60,255,255)
 * CRGB::ORANGE  <=> CHSV(39,255,255)
 */
/*
 * Currently used variables from MavLink_FrSkySPort.ino:
 * ap_base_mode  => ARMED=1, DISARMED=0
 * ap_bar_altitude => ALT from barometer, 100 = 1m
 * ap_sat_visible  => numbers of visible satelites
 * ap_fixtype    => 0 = No GPS, 1 = No Fix, 2 = 2D Fix, 3 = 3D Fix
 * ap_custom_mode  => see http://copter.ardupilot.com/wiki/configuration/arducopter-parameters/#flight_mode_1_arducopterfltmode1
 * ap_throttle   => 0..100
 * ap_chancount  => Number of RC_Channels used
 * ap_chan_raw[i]  => RC channel 1-18 value, in microseconds. A value of UINT16_MAX (65535U)
 * ap_rssi     => Receive signal strength indicator, 0: 0%, 100: 100%, 255: invalid/unknown.
 */

//#####################################################################################################
//### PROGAM BEGIN                                                                                  ###
//#####################################################################################################
//### PROGRAM SETUP                                                                                 ###
//#####################################################################################################
void Teensy_LED_Init()  {   // comment in SIMULATION_MODE
  // Add "Teensy_LED_Init();" to begin of MavLink_FrSkySPort.ino setup() part
/*  Cabling (new):
   1. POS_LEDS_ARMED
   2. NUM_LEDS_GPS
   3. NUM_LEDS_REARARM1[F]
   4. NUM_LEDS_FLASH[0]
   5. NUM_LEDS_REARARM1[R]
   6. NUM_LEDS_BREAKLIGHT
   7. NUM_LEDS_REARARM2[R]
   8. NUM_LEDS_FLASH[1]
   9. NUM_LEDS_REARARM2[F]
  10. NUM_LEDS_SIDEARMS[R]
  11. NUM_LEDS_FRONTARMR[R]
  12. NUM_LEDS_FLASH[2]
  13. NUM_LEDS_FRONTARMF[R]
  14. NUM_LEDS_FRONTLIGHT
  15. NUM_LEDS_FRONTARMF[L]
  16. NUM_LEDS_FLASH[3]
  17. NUM_LEDS_FRONTARMR[L]
  18. NUM_LEDS_LANDING[3]
  19. NUM_LEDS_LANDING[3]
  20. NUM_LEDS_SIDEARMS[L]
  21. NUM_LEDS_LANDING[4]
  22. NUM_LEDS_LANDING[3]
  23. NUM_LEDS_LANDING[3]
*/
  POS_LEDS_ARMED             = 0;
  POS_LEDS_GPS               = POS_LEDS_ARMED             + NUM_LEDS_ARMED;

  POS_LEDS_REARARMS[0]       = POS_LEDS_GPS               + NUM_LEDS_GPS;
  POS_LEDS_FLASH[0]          = POS_LEDS_REARARMS[0]       + NUM_LEDS_REARARMS;
  POS_LEDS_REARARMS[1]       = POS_LEDS_FLASH[0]          + NUM_LEDS_FLASH;

  POS_LEDS_BREAKLIGHT        = POS_LEDS_REARARMS[1]       + NUM_LEDS_REARARMS;

  POS_LEDS_REARARMS[2]       = POS_LEDS_BREAKLIGHT        + NUM_LEDS_BREAKLIGHT;
  POS_LEDS_FLASH[1]          = POS_LEDS_REARARMS[2]       + NUM_LEDS_REARARMS;
  POS_LEDS_REARARMS[3]       = POS_LEDS_FLASH[1]          + NUM_LEDS_FLASH;

  POS_LEDS_SIDEARMS[RIGHT]   = POS_LEDS_REARARMS[3]       + NUM_LEDS_REARARMS;

  POS_LEDS_FRONTARM_R[RIGHT] = POS_LEDS_SIDEARMS[RIGHT]   + NUM_LEDS_SIDEARMS;
  POS_LEDS_FLASH[2]          = POS_LEDS_FRONTARM_R[RIGHT] + NUM_LEDS_FRONTARM_R;
  POS_LEDS_FRONTARM_F[RIGHT] = POS_LEDS_FLASH[2]          + NUM_LEDS_FLASH;

  POS_LEDS_FRONTLIGHT        = POS_LEDS_FRONTARM_F[RIGHT] + NUM_LEDS_FRONTARM_F;

  POS_LEDS_FRONTARM_F[LEFT]  = POS_LEDS_FRONTLIGHT        + NUM_LEDS_FRONTLIGHT;
  POS_LEDS_FLASH[3]          = POS_LEDS_FRONTARM_F[LEFT]  + NUM_LEDS_FRONTARM_F;
  POS_LEDS_FRONTARM_R[LEFT]  = POS_LEDS_FLASH[3]          + NUM_LEDS_FLASH;

  POS_LEDS_LANDING[0]        = POS_LEDS_FRONTARM_R[LEFT]  + NUM_LEDS_FRONTARM_R;
  POS_LEDS_LANDING[1]        = POS_LEDS_LANDING[0]        + NUM_LEDS_LANDING1;

  POS_LEDS_SIDEARMS[LEFT]    = POS_LEDS_LANDING[1]        + NUM_LEDS_LANDING1;

  POS_LEDS_LANDING[2]        = POS_LEDS_SIDEARMS[LEFT]    + NUM_LEDS_SIDEARMS;
  POS_LEDS_LANDING[3]        = POS_LEDS_LANDING[2]        + NUM_LEDS_LANDING2;
  POS_LEDS_LANDING[4]        = POS_LEDS_LANDING[3]        + NUM_LEDS_LANDING1;

  FastLED.addLeds<LEDTYPE, DATA_PIN, RGBORDER>(leds, NUM_LEDS_TOTAL);
  #ifdef standalone
    Serial.begin(9600);
  #endif
}

//#####################################################################################################
//### PROGRAM LOOP                                                                                  ###
//#####################################################################################################
void Teensy_LED_process() {   // comment in SIMULATION_MODE
  float dim = 0.25;
  currentmillis = millis();
  ap_base_mode_last = ap_base_mode;

#ifdef SIMULATION_MODE
  fc_simulation_mode();
#endif
  if (LED_MODE_SWITCH == 0) {
    get_mode();
    if (LED_DIMM_CHAN != 0) {
      dim = rc_dimmer_proz();
    }
  } else {
#ifdef SIMULATION_MODE
#else
    /*
     * from MavLink_FrSkySPort.ino
     * ap_chan_raw[i]  => RC channel i input value, in microseconds. A value of UINT16_MAX (65535U)
     */
    if ( ap_chan_raw[LED_MODE_SWITCH] <= 1150 ) {
      get_mode();   // DEFAULT, LED Mode - auto set by FC
    } else if ( ap_chan_raw[LED_MODE_SWITCH] <= 1350 ) {
      LED_MODE = 101; // USER_DEFINED LED Mode 1
    } else if ( ap_chan_raw[LED_MODE_SWITCH] <= 1450 ) {
      LED_MODE = 102; // USER_DEFINED LED Mode 2
    } else if ( ap_chan_raw[LED_MODE_SWITCH] <= 1550 ) {
      LED_MODE = 103; // USER_DEFINED LED Mode 3
    } else if ( ap_chan_raw[LED_MODE_SWITCH] <= 1850 ) {
      LED_MODE = 104; // USER_DEFINED LED Mode 4
    } else if ( ap_chan_raw[LED_MODE_SWITCH] >  1850 ) {
      LED_MODE = 105; // USER_DEFINED LED Mode 5
    } else {
      get_mode();   // DEFAULT, LED Mode - auto set by FC
    }
    if (LED_DIMM_CHAN != 0) {
      dim = rc_dimmer_proz();
    }
#endif
  }
  dim = dim / 100;

  switch (LED_MODE) {
    case 1:   // NO_GPS Flight modes
      default_mode(ON, dim);
      rear_arms_F(OFF, dim);
      break;
    case 2:   // GPS    Flight modes
      default_mode(ON, dim);
      side_arms(ON, dim);
      rear_arms_F(OFF, dim);
      break;
    case 3:   // MANUAL Flight modes
      //default_mode(OFF, dim);
      front_arms(ON, dim);
      side_arms(OFF, dim);
      rear_arms(ON, dim);
      front_light(OFF, dim);
      break_light(BLINK, dim);    // maybe blink
      landing_light(OFF, dim);
      flash_pos_light(ON, dim);
      get_armed_status(ON, dim);
      get_gps_status(ON, dim);

      break;
    case 4:   // AUTO   Flight modes
      front_arms_R(ON, dim);
      side_arms(ON, dim);
      rear_arms(ON, dim);
      break_light(ON, dim);
      flash_pos_light(ON, dim);
      get_armed_status(ON, dim);
      get_gps_status(ON, dim);
      LarsonScanner(0, POS_LEDS_FRONTARM_F[RIGHT], POS_LEDS_FLASH[3], dim);
      landing_light(AUTO, 1);
      break;
    case 5:   // LAND   Flight mode
      default_mode(ON, dim);
      side_arms(ON, dim);
      landing_light(AUTO, dim);
      break;
    case 6:   // BRAKE Flight mode
      front_arms(ON, dim);
      side_arms(OFF, dim);
      rear_arms(ON, dim);
      front_light(OFF, dim);
      break_light(BLINK, dim);    // maybe blink
      landing_light(OFF, dim);
      flash_pos_light(ON, dim);
      get_armed_status(ON, dim);
      get_gps_status(ON, dim);
      break;
    case 7:   // FLIP  Flight mode
      default_mode(ON, dim);
      break;
    case 8:   // AUTO_TUNE, ...
      default_mode(ON, dim);
      break;
    case 101:   // USER Mode 1
      front_arms(OFF, dim);
      side_arms(OFF, dim);
      rear_arms(OFF, dim);
      landing_light(OFF, dim);
      front_light(ON, dim);
      break_light(ON, dim);
      get_armed_status(ON, dim);
      get_gps_status(ON, dim);
      flash_pos_light(ON, dim);
      break;
    case 102:   // USER Mode 2
      LarsonScanner(0, POS_LEDS_REARARMS[0], NUM_LEDS_TOTAL, dim);
      get_armed_status(ON, dim);
      get_gps_status(ON, dim);
      break;
    case 103:   // USER Mode 3
      front_arms(OFF, dim);
      front_arms_F(ON, dim);
      side_arms(OFF, dim);
      rear_arms(OFF, dim);
      front_light(ON, dim);
      break_light(BLINK, dim);
      landing_light(AUTO, 1);
      get_armed_status(OFF, dim);
      get_gps_status(OFF, dim);
      flash_pos_light(ON, dim);
      break;
    case 104:   // USER Mode 4
      /*front_arms(OFF, dim);
      side_arms(OFF, dim);
      rear_arms(OFF, dim);
      front_light(OFF, dim);
      break_light(OFF, dim);
      landing_light(ON, dim);*/
      /*rotor_lights(39, dim);
      get_armed_status(ON, dim);
      get_gps_status(ON, dim);
      flash_pos_light(ON, dim);*/
      cust_leds(ON, 0, NUM_LEDS_TOTAL, dim);
      break;
    case 105:   // USER Mode 5 - all off
      FastLED.clear();
      //cust_leds(ON, 0, NUM_LEDS_TOTAL, dim);
      break;
    case 999:   // disarmed
      front_arms(OFF, dim);
      side_arms(OFF, dim);
      rear_arms(OFF, dim);
      front_light(OFF, dim);
      break_light(OFF, dim);
      landing_light(OFF, dim);
      flash_pos_light(ON, dim);
      get_armed_status(ON, dim);
      get_gps_status(ON, dim);
      break;
  }
  FastLED.show();
  lastmillis = currentmillis;
}

//#####################################################################################################
//### DIMMER FUNCTIONs                                                                              ###
//#####################################################################################################
byte rc_dimmer() {          // ppm to byte (min 0 / max 255)
  double factor = (PWM_MAX - PWM_MIN) / 255;
  /*
   * double ppm_val = ap_chan_raw[LED_DIMM_CHAN] - PWM_MIN;
   * float dim = ppm_val / factor;
   */
  byte TARGET_DIM;
  if ( (ap_chan_raw[LED_DIMM_CHAN] - PWM_MIN ) / factor > 255 ) {
    TARGET_DIM = 255;
  } else if ( (ap_chan_raw[LED_DIMM_CHAN] - PWM_MIN ) / factor < 0 ) {
    TARGET_DIM = 0;
  } else {
    TARGET_DIM = (ap_chan_raw[LED_DIMM_CHAN] - PWM_MIN ) / factor;
  }
  return TARGET_DIM;
}

byte rc_dimmer_proz() {         // ppm to % ( min 0 % / max 100 % )
  double factor = (PWM_MAX - PWM_MIN) / 100;
  byte TARGET_DIM;
  if (ap_chan_raw[LED_DIMM_CHAN] > 900 ) {
    TARGET_DIM = (ap_chan_raw[LED_DIMM_CHAN] - PWM_MIN ) / factor;
    if ( TARGET_DIM > 100 ) {
      TARGET_DIM = 100;
    } else if ( TARGET_DIM < 0 ) {
      TARGET_DIM = 0;
    }
  } else {
    TARGET_DIM = 15;
  }
  return TARGET_DIM;
}

float dimmer(int DIM_VAL) {
  double factor = 2.55;
  byte TARGET_DIM;
  if (DIM_VAL > 100) DIM_VAL = 100;
  TARGET_DIM = DIM_VAL * factor;
  return TARGET_DIM;
}

//#####################################################################################################
//### SIMULATION FUNCTION                                                                           ###
//#####################################################################################################
#ifdef SIMULATION_MODE
void fc_simulation_mode() {
  int max_loops = 60;
  if (lastmillis >= targetmillisfc_sim_mode) {
    //LED_MODE_SWITCH = 0;
    if (SIM_LOOP <= 3) {
      ap_fixtype = 0;
      ap_sat_visible = random(0, SIM_LOOP);
      //SIM_LOOP = 20;
    } else if (SIM_LOOP <= 5) {
      ap_fixtype = 1;
      ap_sat_visible = random(SIM_LOOP, 3);
    } else if (SIM_LOOP > 5) {
      ap_fixtype = 2;
      ap_sat_visible = random(SIM_LOOP, 6);
    }

    if (SIM_LOOP > 8) {
      ap_fixtype = 3;
      ap_sat_visible = random(6, 20);
    }

    if (SIM_LOOP > 10) {
      ap_base_mode = 1;
    } else {
      ap_base_mode = 0;
      ap_bar_altitude = 0;
    }

    if (SIM_LOOP > 13) {
      ap_custom_mode = random(0, 17);
      ap_throttle = random(50, 100);
      ap_bar_altitude = random(0, 1000);
    } else {
      ap_custom_mode = 0;
      ap_throttle = 0;
    }

    if (SIM_LOOP > 20 && SIM_LOOP < 40) {
      LED_MODE = random(101, 105);
      LED_MODE_SWITCH = 13;
    } else {
      LED_MODE_SWITCH = 0;
    }

    if (SIM_LOOP > max_loops - 10) {
      ap_throttle = 0;
    }

    if (SIM_LOOP > max_loops - 5) {
      ap_throttle = 0;
      ap_base_mode = 0;
      ap_custom_mode = 0;
      LED_MODE = 0;
    }

    if (SIM_LOOP > max_loops) {
      SIM_LOOP = 0;
    } else {
      SIM_LOOP++;
    }
    /*
    if (SIM_LOOP < 9) {
      LED_MODE = SIM_LOOP;
      LED_MODE_SWITCH = 13;
    }
    */
    /*
    LED_MODE = 102;
    LED_MODE = 4;
    LED_MODE_SWITCH = 13;
    */
    Serial.println();
    Serial.print("LED_MODE: ");
    Serial.print(LED_MODE);
    Serial.print(" LastMillis: ");
    Serial.print(lastmillis);
    Serial.print(" CurMillis: ");
    Serial.print(currentmillis);
    Serial.print(", targetMillis: ");
    Serial.print(targetmillisfc_sim_mode);
    Serial.print(", Loop: ");
    Serial.print(SIM_LOOP);
    Serial.print(":");
    Serial.println();
    Serial.print("fixtype: ");
    Serial.print(ap_fixtype);
    Serial.print(" #Sats: ");
    Serial.print(ap_sat_visible);
    Serial.print(" Armed?: ");
    Serial.print(ap_base_mode);
    Serial.print(" FlightModes: ");
    Serial.print(ap_custom_mode);
    Serial.print(" Throttle: ");
    Serial.print(ap_throttle);
    Serial.print(" ALT: ");
    Serial.print(ap_bar_altitude);
    Serial.println();

    targetmillisfc_sim_mode = currentmillis + 3000;
  }
}
#endif

//#####################################################################################################
//### GET ArduCopter Flightmode                                                                     ###
//#####################################################################################################
void get_mode() {
  /* from MavLink_FrSkySPort.ino
   *  ap_custom_mode  => see http://copter.ardupilot.com/wiki/configuration/arducopter-parameters/#flight_mode_1_arducopterfltmode1
   *  LED_MODE  TYPE
   *  1         NO_GPS Flight modes
   *  2         GPS    Flight modes
   *  3         MANUAL Flight modes
   *  4         AUTO   Flight modes
   *  5         LAND
   *  6         BRAKE
   *  7         FLIP
   *  8         AUTO_TUNE, ...
   *  999       DISARMED
   *
   *  VALUE  MEANING    LED_MODE
   *  0      Stabilize  1
   *  1      Acro       3
   *  2      AltHold    1
   *  3      Auto       4
   *  4      Guided     4
   *  5      Loiter     2
   *  6      RTL        4
   *  7      Circle     4
   *  9      Land       5
   *  11     Drift      3
   *  13     Sport      1
   *  14     Flip       7
   *  15     AutoTune   8
   *  16     PosHold    2
   *  17     Brake      6
   *  ...    ...        ...
   *
   *  Mapping of AC Flight modes to LED lighting modes.
   */
  if (ap_base_mode == 1) {
    switch (ap_custom_mode) {
      case 0:  LED_MODE = 1; break;
      case 1:  LED_MODE = 3; break;
      case 2:  LED_MODE = 1; break;
      case 3:  LED_MODE = 4; break;
      case 4:  LED_MODE = 5; break;
      case 5:  LED_MODE = 2; break;
      case 6:  LED_MODE = 4; break;
      case 7:  LED_MODE = 4; break;
      case 9:  LED_MODE = 5; break;
      case 11: LED_MODE = 3; break;
      case 13: LED_MODE = 1; break;
      case 14: LED_MODE = 7; break;
      case 15: LED_MODE = 8; break;
      case 16: LED_MODE = 2; break;
      case 17: LED_MODE = 6; break;
      default: LED_MODE = 1; break;
    }
  } else {
    LED_MODE = 999;
  }
}

//#####################################################################################################
//### SETUP DEFAULT MODE FOR LED LIGHTNING - SIDEARMS, FRONT-, BREAK-, LANDING-LIGHT always off     ###
//#####################################################################################################
void default_mode(int STATUS, float dim) {
  front_arms(STATUS, dim);
  side_arms(!STATUS, dim);
  rear_arms(STATUS, dim);
  front_light(!STATUS, dim);
  break_light(!STATUS, dim);
  flash_pos_light(STATUS, dim);
  landing_light(AUTO, dim);
  get_armed_status(ON, dim);
  get_gps_status(ON, dim);
}

//#####################################################################################################
//### GET ARMED STATUS FROM ARDUPILOT AND PRINT CORESPONDING LED(s)                                 ###
//#####################################################################################################
void get_armed_status(int STATUS, float dim) {
  /*
   * from MavLink_FrSkySPort.ino
   * ap_base_mode  => ARMED=1, DISARMED=0
  */
  /*
  debugSerial.print(millis());
  debugSerial.print("Armed?: ");
  debugSerial.print(ap_base_mode);
  debugSerial.println();
  */
  if (NUM_LEDS_ARMED > 0) {
    for (int i = 0; i < NUM_LEDS_ARMED; i++) {
      if (STATUS == 1) {
        switch (ap_base_mode) {
          case 0:
            leds[i+POS_LEDS_ARMED] = CHSV(39,255,255*dim);
            break;
          case 1:
            leds[i+POS_LEDS_ARMED] = CHSV(96,255,255*dim);
            break;
          default:
            leds[i+POS_LEDS_ARMED] = CHSV(0, 0, 0);
            break;
        }
      } else {
        leds[i+POS_LEDS_ARMED] = CHSV(0, 0, 0);
      }
    }
  }
}

//#####################################################################################################
//### GET GPS-STATUS FROM ARDUPILOT AND PRINT CORESPONDING LED(s)                                   ###
//#####################################################################################################
void get_gps_status(int STATUS, float dim) {
  /* from MavLink_FrSkySPort.ino
   *  ap_sat_visible  => numbers of visible satellites
   *  ap_fixtype    => 0 = No GPS, 1 = No Fix, 2 = 2D Fix, 3 = 3D Fix
   */
  CRGB COLOR;
  int FREQ;
  if (NUM_LEDS_GPS > 0) {
    if (STATUS == 1) {
      //Serial.print(ap_fixtype);
      switch (ap_fixtype) {
        case 0:  //blink red
          COLOR = CHSV(0,255,255*dim);
          FREQ = 750;
          break;
        case 1: //blink orange
          COLOR = CHSV(39,255,255*dim);
          FREQ = 750;
          break;
        case 2: // green
          COLOR = CHSV(96,255,128*dim);
          FREQ = 750;
          break;
        case 3: // const green
          COLOR = CHSV(96,255,255*dim);
          FREQ = 0;
          state_GPS = 0;
          break;
        default: // off
          COLOR = CHSV(0, 0, 0);
          FREQ = 0;
      }
      if (currentmillis >= targetmillis_GPS) {
        if (state_GPS == 0) {
          for (int i = 0; i < NUM_LEDS_GPS; i++) {
            leds[i+POS_LEDS_GPS] = COLOR;
          }
          state_GPS = 1;
        } else {
          for (int i = 0; i < NUM_LEDS_GPS; i++) {
            leds[i+POS_LEDS_GPS] = CHSV(0, 0, 0);
          }
          state_GPS = 0;
        }
        targetmillis_GPS = lastmillis + FREQ;
      }
    } else {
      for (int i = 0; i < NUM_LEDS_GPS; i++) {
        leds[i+POS_LEDS_GPS] = CHSV(0, 0, 0);
      }
    }
  }
}

//#####################################################################################################
//### CONFIG LED_MODES                                                                              ###
//#####################################################################################################
//### DEFAULT FRONTLIGHT SETTING (WHITE)                                                            ###
//#####################################################################################################
void front_light(int STATUS, float dim) {
  if (NUM_LEDS_FRONTLIGHT > 0) {
    for (int i = 0; i < NUM_LEDS_FRONTLIGHT; i++) {
      if (STATUS == 1) {
        leds[i+POS_LEDS_FRONTLIGHT] = CHSV(0,0,255*dim);
      } else {
        leds[i+POS_LEDS_FRONTLIGHT] = CHSV(0, 0, 0);
      }
    }
  }
}

//#####################################################################################################
//### DEFAULT REAR/BREAK-LIGHT SETTING (RED)                                                        ###
//#####################################################################################################
void break_light(int STATUS, float dim) {
  int DELAY;
  if (STATUS == BLINK) {
    DELAY = 500;
  } else {
    DELAY = 0;
    state_BREAK = 0;
  }

  byte hue = 0;
  byte saturation = 255;
  byte value = 255;
  byte value_dimmed = value * dim;
  if (NUM_LEDS_BREAKLIGHT > 0) {
    for (int i = 0; i < NUM_LEDS_BREAKLIGHT; i++) {
      if (STATUS >= 1) {
        if (currentmillis >= targetmillis_BREAK) {
          if (state_BREAK == 0) {
            for (int i = 0; i < NUM_LEDS_BREAKLIGHT; i++) {
              leds[POS_LEDS_BREAKLIGHT + i] = CHSV(hue, saturation, value_dimmed);
            }
            state_BREAK = 1;
          } else {
            for (int i = 0; i < NUM_LEDS_BREAKLIGHT; i++) {
              leds[POS_LEDS_BREAKLIGHT + i] = CHSV(0, 0, 0);
            }
            state_BREAK = 0;
          }
          targetmillis_BREAK = lastmillis + DELAY;
        }
      } else {
        leds[i + POS_LEDS_BREAKLIGHT] = CHSV(0, 0, 0);    // BLACK
      }
    }
  }


}

//#####################################################################################################
//### DEFAULT FRONT ARM LED MODE (CONST-RED)                                                        ###
//#####################################################################################################
void front_arms(int STATUS, float dim) {
  /*
   * #define NUM_LEDS_FRONTARM_F     6 // Number of LEDs on FRONTSIDE of front arms
   * #define NUM_LEDS_FRONTARM_R     3 // Number of LEDs on BACKSIDE  of front arms
   */
  if (NUM_LEDS_FRONTARM_F > 0) {
    for (int i = 0; i < NUM_LEDS_FRONTARM_F; i++) {
      if (STATUS == 1) {
        leds[POS_LEDS_FRONTARM_F[RIGHT] + i] = CHSV(0,255,255*dim);
        leds[POS_LEDS_FRONTARM_F[LEFT]  + i] = CHSV(0,255,255*dim);
      } else {
        leds[POS_LEDS_FRONTARM_F[RIGHT] + i] = CHSV(0, 0, 0);
        leds[POS_LEDS_FRONTARM_F[LEFT]  + i] = CHSV(0, 0, 0);
      }
    }
  }
  // Second if loop needed due to different Number of LEDs
  if (NUM_LEDS_FRONTARM_R > 0) {
    for (int i = 0; i < NUM_LEDS_FRONTARM_R; i++) {
      if (STATUS == 1) {
        leds[POS_LEDS_FRONTARM_R[RIGHT] + i] = CHSV(0,255,255*dim);
        leds[POS_LEDS_FRONTARM_R[LEFT]  + i] = CHSV(0,255,255*dim);
      } else {
        leds[POS_LEDS_FRONTARM_R[RIGHT] + i] = CHSV(0, 0, 0);
        leds[POS_LEDS_FRONTARM_R[LEFT]  + i] = CHSV(0, 0, 0);
      }
    }
  }
}

void front_arms_F(int STATUS, float dim) {
  if (NUM_LEDS_FRONTARM_F > 0) {
    for (int i = 0; i < NUM_LEDS_FRONTARM_F; i++) {
      if (STATUS == 1) {
        leds[POS_LEDS_FRONTARM_F[RIGHT] + i] = CHSV(0,255,255*dim);
        leds[POS_LEDS_FRONTARM_F[LEFT]  + i] = CHSV(0,255,255*dim);
      } else {
        leds[POS_LEDS_FRONTARM_F[RIGHT] + i] = CHSV(0, 0, 0);
        leds[POS_LEDS_FRONTARM_F[LEFT]  + i] = CHSV(0, 0, 0);
      }
    }
  }
}

void front_arms_R(int STATUS, float dim) {
  if (NUM_LEDS_FRONTARM_R > 0) {
    for (int i = 0; i < NUM_LEDS_FRONTARM_R; i++) {
      if (STATUS == 1) {
        leds[POS_LEDS_FRONTARM_R[RIGHT] + i] = CHSV(0,255,255*dim);
        leds[POS_LEDS_FRONTARM_R[LEFT]  + i] = CHSV(0,255,255*dim);
      } else {
        leds[POS_LEDS_FRONTARM_R[RIGHT] + i] = CHSV(0, 0, 0);
        leds[POS_LEDS_FRONTARM_R[LEFT]  + i] = CHSV(0, 0, 0);
      }
    }
  }
}

//#####################################################################################################
//### DEFAULT SIDE ARM LED MODE (CONST-YELLOW)                                                      ###
//#####################################################################################################
void side_arms(int STATUS, float dim) {
  if (NUM_LEDS_SIDEARMS > 0) {
    for (int i = 0; i < NUM_LEDS_SIDEARMS; i++) {
      if (STATUS == 1) {
        leds[POS_LEDS_SIDEARMS[RIGHT] + i] = CHSV(60,255,255*dim);
        leds[POS_LEDS_SIDEARMS[LEFT]  + i] = CHSV(60,255,255*dim);
      } else {
        leds[POS_LEDS_SIDEARMS[RIGHT] + i] = CHSV(0, 0, 0);
        leds[POS_LEDS_SIDEARMS[LEFT]  + i] = CHSV(0, 0, 0);
      }
    }
  }
}

//#####################################################################################################
//### DEFAULT REAR ARM LED MODE (CONST-GREEN)                                                       ###
//#####################################################################################################
void rear_arms(int STATUS, float dim) {
  if (NUM_LEDS_REARARMS > 0) {
    for (int i = 0; i < NUM_LEDS_REARARMS; i++) {
      if (STATUS == 1) {
        leds[POS_LEDS_REARARMS[0] + i] = CHSV(96,255,255*dim);
        leds[POS_LEDS_REARARMS[1] + i] = CHSV(96,255,255*dim);
        leds[POS_LEDS_REARARMS[2] + i] = CHSV(96,255,255*dim);
        leds[POS_LEDS_REARARMS[3] + i] = CHSV(96,255,255*dim);
      } else {
        leds[POS_LEDS_REARARMS[0] + i] = CHSV(0, 0, 0);
        leds[POS_LEDS_REARARMS[1] + i] = CHSV(0, 0, 0);
        leds[POS_LEDS_REARARMS[2] + i] = CHSV(0, 0, 0);
        leds[POS_LEDS_REARARMS[3] + i] = CHSV(0, 0, 0);
      }
    }
  }
}

void rear_arms_F(int STATUS, float dim) {
  if (NUM_LEDS_REARARMS > 0) {
    for (int i = 0; i < NUM_LEDS_REARARMS; i++) {
      if (STATUS == 1) {
        leds[POS_LEDS_REARARMS[0] + i] = CHSV(96,255,255*dim);
        //leds[POS_LEDS_REARARMS[1] + i] = CHSV(96,255,255*dim);
        //leds[POS_LEDS_REARARMS[2] + i] = CHSV(96,255,255*dim);
        leds[POS_LEDS_REARARMS[3] + i] = CHSV(96,255,255*dim);
      } else {
        leds[POS_LEDS_REARARMS[0] + i] = CHSV(0, 0, 0);
        //leds[POS_LEDS_REARARMS[1] + i] = CHSV(0, 0, 0);
        //leds[POS_LEDS_REARARMS[2] + i] = CHSV(0, 0, 0);
        leds[POS_LEDS_REARARMS[3] + i] = CHSV(0, 0, 0);
      }
    }
  }
}

void rear_arms_R(int STATUS, float dim) {
  if (NUM_LEDS_REARARMS > 0) {
    for (int i = 0; i < NUM_LEDS_REARARMS; i++) {
      if (STATUS == 1) {
        //leds[POS_LEDS_REARARMS[0] + i] = CHSV(96,255,255*dim);
        leds[POS_LEDS_REARARMS[1] + i] = CHSV(96,255,255*dim);
        leds[POS_LEDS_REARARMS[2] + i] = CHSV(96,255,255*dim);
        //leds[POS_LEDS_REARARMS[3] + i] = CHSV(96,255,255*dim);
      } else {
        //leds[POS_LEDS_REARARMS[0] + i] = CHSV(0, 0, 0);
        leds[POS_LEDS_REARARMS[1] + i] = CHSV(0, 0, 0);
        leds[POS_LEDS_REARARMS[2] + i] = CHSV(0, 0, 0);
        //leds[POS_LEDS_REARARMS[3] + i] = CHSV(0, 0, 0);
      }
    }
  }
}

//#####################################################################################################
//### DEFAULT LANDING LIGHT                                                                         ###
//#####################################################################################################
void landing_light(int STATUS, float dim) {
  int32_t rel_alt = 0;
  int alt_land = 3;
  if (ap_base_mode == 1 && ap_base_mode_last == 0) {
    ground_level = ap_bar_altitude / 100;
  } else if (ap_base_mode == 1) {
    rel_alt = ap_bar_altitude / 100;
#ifdef DEBUG_LANDUNG
  } else {
    ground_level = 0;
    rel_alt = ap_bar_altitude / 100;
    ap_base_mode = 1;
#endif
  }
  if (STATUS == 2) {
    if ( rel_alt - alt_land < ground_level && ap_base_mode == 1 ) {
      STATUS = 1;
    } else if ( rel_alt - alt_land > ground_level && ap_base_mode == 1 ) {
      STATUS = 0;
    }
  }
#ifdef DEBUG_LANDUNG
  debugSerial.print(millis());
  debugSerial.println();
  debugSerial.print("Groundlevel: ");
  debugSerial.print(ground_level);
  debugSerial.println();
  debugSerial.print("AP_ALT: ");
  debugSerial.print(ap_bar_altitude);
  debugSerial.println();
  debugSerial.print("REL_ALT: ");
  debugSerial.print(rel_alt);
  debugSerial.println();
  debugSerial.print("ALT_LAND: ");
  debugSerial.print(alt_land);
  debugSerial.println();
  debugSerial.print("Status: ");
  debugSerial.print(STATUS);
  debugSerial.println();
  debugSerial.println();
#endif
  if (NUM_LEDS_LANDING1 > 0) {
    for (int i = 0; i < NUM_LEDS_LANDING1; i++) {
      if (STATUS == 1) {
        leds[POS_LEDS_LANDING[0] + i] = CHSV(0,0,255*dim);
        leds[POS_LEDS_LANDING[1] + i] = CHSV(0,0,255*dim);
        leds[POS_LEDS_LANDING[2] + i] = CHSV(0,0,255*dim);
        leds[POS_LEDS_LANDING[3] + i] = CHSV(0,0,255*dim);
        leds[POS_LEDS_LANDING[4] + i] = CHSV(0,0,255*dim);
      } else {
        leds[POS_LEDS_LANDING[0] + i] = CHSV(0, 0, 0);
        leds[POS_LEDS_LANDING[1] + i] = CHSV(0, 0, 0);
        leds[POS_LEDS_LANDING[2] + i] = CHSV(0, 0, 0);
        leds[POS_LEDS_LANDING[3] + i] = CHSV(0, 0, 0);
        leds[POS_LEDS_LANDING[4] + i] = CHSV(0, 0, 0);
      }
    }
    // needed because Landinglight of REAR has 1 LED more
    if (STATUS == 1) {
      leds[POS_LEDS_LANDING[2] + NUM_LEDS_LANDING2] = CHSV(0,0,255*dim);
    } else {
      leds[POS_LEDS_LANDING[2] + NUM_LEDS_LANDING2] = CHSV(0, 0, 0);
    }
  }
}


//#####################################################################################################
//### FLASH POSITION LIGHTS (around motors)                                                         ###
//#####################################################################################################
void flash_pos_light(int STATUS, float dim) {
  int DELAY = 1000;
  if (STATUS == OFF) {
  state_FLASH = 1;
  }
  //if (dim < 0.5) dim = 0.5;
  //dim = 0.05;
  if (currentmillis >= targetmillis_FLASH) {
    if (state_FLASH == 0) {
      for (int i = 0; i < NUM_ARMS; i++) {
        leds[POS_LEDS_FLASH[i] + 0] = CHSV(60,255,255*dim);
        leds[POS_LEDS_FLASH[i] + 1] = CHSV(0,0,255*dim);
        leds[POS_LEDS_FLASH[i] + 2] = CHSV(60,255,255*dim);
      }
      state_FLASH = 1;
      DELAY = DELAY / 7;
    } else {
      for (int i = 0; i < NUM_ARMS; i++) {
        leds[POS_LEDS_FLASH[i] + 0] = CHSV(0, 0, 0);
        leds[POS_LEDS_FLASH[i] + 1] = CHSV(0, 0, 0);
        leds[POS_LEDS_FLASH[i] + 2] = CHSV(0, 0, 0);
      }
      state_FLASH = 0;
      if (FL_RUN == 3) {
        DELAY = DELAY / 9;
      } else if (FL_RUN == 4) {
        FL_RUN = 0;
      }
      FL_RUN++;
    }
    targetmillis_FLASH = lastmillis + DELAY;
  }
}

//#####################################################################################################
//### LARSON SCANNER (CIRCLING ON ALL LEDs)                                                         ###
//#####################################################################################################
void LarsonScanner(byte LS_hue, int START_POS, int END_POS, float dim) {
  int DELAY = 50;
  if (pos == 0 ) {
    pos = START_POS;
  } else if (pos < START_POS) {
    pos = START_POS;
  } else if (pos > END_POS) {
    pos = START_POS;
  }

  if (currentmillis >= targetmillis_LS) {
    switch (dir) {
      case RIGHT:
        pos++;
        if (pos == END_POS - 1)
          dir = LEFT;
        break;
      case LEFT:
        pos--;
        if (pos == START_POS)
          dir = RIGHT;
        break;
    }
    for (int i = START_POS; i < END_POS; i++) {
      leds[i] = CHSV(0,0,0);
    }
    if (dir == RIGHT) {
      if (pos <= END_POS - 1 && pos >= START_POS)
        leds[pos] = CHSV(LS_hue, 255, 255*dim);
      if (pos <= END_POS + 1 && pos >= START_POS + 1)
        leds[pos - 1] = CHSV(LS_hue, 255, 180*dim);
      if (pos <= END_POS + 2 && pos >= START_POS + 2)
        leds[pos - 2] = CHSV(LS_hue, 255, 100*dim);
      if (pos == -2)
        leds[pos + 2] = CHSV(LS_hue, 255, 100*dim);
    }
    if (dir == LEFT) {
      if (pos <= END_POS - 1 && pos >= START_POS)
        leds[pos] = CHSV(LS_hue, 255, 255*dim);
      if (pos <= END_POS - 2 && pos >= START_POS - 1)
        leds[pos + 1] = CHSV(LS_hue, 255, 180*dim);
      if (pos <= END_POS - 3 && pos >= START_POS - 2)
        leds[pos + 2] = CHSV(LS_hue, 255, 100*dim);
      if (pos == END_POS + 1)
        leds[pos - 2] = CHSV(LS_hue, 255, 100*dim);
    }
    targetmillis_LS = lastmillis + DELAY;
  }
}

//#####################################################################################################
//### Rotor Lights (Cycling Light on Arms)                                                          ###
//#####################################################################################################
void rotor_lights(byte hue, float dim) {
  int DELAY = 500;

  if (currentmillis >= targetmillis_RL) {
    #ifdef DEBUG_ROTOR_LIGHTS
      debugSerial.print(millis());
      debugSerial.print("\tRotor Lights Position: ");
      debugSerial.println(pos_RL);
    #endif
    FastLED.clear();
    switch(pos_RL) {
      case 0:
        if (NUM_LEDS_FRONTARM_R > 0) {
          for (int i = 0; i < NUM_LEDS_FRONTARM_R; i++) {
            leds[i + POS_LEDS_FRONTARM_R[RIGHT]] = CHSV(0, 0, 0);
          }
        }
        if (NUM_LEDS_FRONTARM_F > 0) {
          for (int i = 0; i < NUM_LEDS_FRONTARM_F; i++) {
            leds[i + POS_LEDS_FRONTARM_F[RIGHT]] = CHSV(hue, 255, 255*dim);
          }
        }
        pos_RL++;
        break;
      case 1:
        if (NUM_LEDS_FRONTARM_F > 0) {
          for (int i = 0; i < NUM_LEDS_FRONTARM_F; i++) {
            leds[i + POS_LEDS_FRONTARM_F[RIGHT]] = CHSV(0, 0, 0);
            leds[i + POS_LEDS_FRONTARM_F[LEFT]] = CHSV(hue, 255, 255*dim);
          }
        }
        pos_RL++;
        break;
      case 2:
        if (NUM_LEDS_FRONTARM_F > 0) {
          for (int i = 0; i < NUM_LEDS_FRONTARM_F; i++) {
            leds[i + POS_LEDS_FRONTARM_F[LEFT]] = CHSV(0, 0, 0);
          }
        }
        if (NUM_LEDS_FRONTARM_R > 0) {
          for (int i = 0; i < NUM_LEDS_FRONTARM_R; i++) {
            leds[i + POS_LEDS_FRONTARM_R[LEFT]] = CHSV(hue, 255, 255*dim);
          }
        }
        pos_RL++;
        break;
      case 3:
        if (NUM_LEDS_FRONTARM_R > 0) {
          for (int i = 0; i < NUM_LEDS_FRONTARM_R; i++) {
            leds[i + POS_LEDS_FRONTARM_R[LEFT]] = CHSV(0, 0, 0);
          }
        }
        if (NUM_LEDS_REARARMS > 0) {
          for (int i = 0; i < NUM_LEDS_REARARMS; i++) {
            leds[i + POS_LEDS_REARARMS[0]] = CHSV(39, 255, 255*dim);
          }
        }
        pos_RL++;
        break;
      case 4:
        if (NUM_LEDS_REARARMS > 0) {
          for (int i = 0; i < NUM_LEDS_REARARMS; i++) {
            leds[i + POS_LEDS_REARARMS[0]] = CHSV(0, 0, 0);
            leds[i + POS_LEDS_REARARMS[1]] = CHSV(39, 255, 255*dim);
          }
        }
        pos_RL++;
        break;
      case 5:
        if (NUM_LEDS_REARARMS > 0) {
          for (int i = 0; i < NUM_LEDS_REARARMS; i++) {
            leds[i + POS_LEDS_REARARMS[1]] = CHSV(0, 0, 0);
            leds[i + POS_LEDS_REARARMS[2]] = CHSV(39, 255, 255*dim);
          }
        }
        pos_RL++;
        break;
      case 6:
        if (NUM_LEDS_REARARMS > 0) {
          for (int i = 0; i < NUM_LEDS_REARARMS; i++) {
            leds[i + POS_LEDS_REARARMS[2]] = CHSV(0, 0, 0);
            leds[i + POS_LEDS_REARARMS[3]] = CHSV(39, 255, 255*dim);
          }
        }
        pos_RL++;
        break;
      case 7:
        if (NUM_LEDS_REARARMS > 0) {
          for (int i = 0; i < NUM_LEDS_REARARMS; i++) {
            leds[i + POS_LEDS_REARARMS[3]] = CHSV(0, 0, 0);
          }
        }
        if (NUM_LEDS_FRONTARM_R > 0) {
          for (int i = 0; i < NUM_LEDS_FRONTARM_R; i++) {
            leds[i + POS_LEDS_FRONTARM_R[RIGHT]] = CHSV(39, 255, 255*dim);
          }
        }
        pos_RL = 0;
        break;
    }
    targetmillis_RL = currentmillis + DELAY;
  }
}

//#####################################################################################################
//### Activate defined LEDS                                                                         ###
//#####################################################################################################
void cust_leds(int STATUS, int START_POS, int END_POS, float dim) {
  for (int i = START_POS; i < END_POS; i++) {
    if (STATUS == 1) {
      leds[i] = CHSV(0, 0, 255*dim);
    }
  }
}

//#####################################################################################################
//### PROGRAM END                                                                                   ###
//#####################################################################################################
/*
  http://forum.arduino.cc/index.php?topic=198987.0
  http://forum.arduino.cc/index.php?topic=299023.0
*/
#endif
