
unsigned long startTime;
unsigned long duration;

float voltage, amp;

char vol_display_bf[32];
char amp_display_bf[32];
char rssi_display_bf[32];
char statustext_bf[256];
char imu_display_xacc_bf[32], imu_display_yacc_bf[32], imu_display_zacc_bf[32];
char bar_alt_bf[32], gps_type_bf[32], gps_satellites_bf[32], gps_lat_bf[32], gps_longt_bf[32], gps_hdop_bf[32];
char gps_home_bf[32];
float dstlon, dstlat;

#define delay_lcd_Show_time 200

void getHomeVars()
{
  float dstlon, dstlat;

  osd_alt_to_home = (osd_alt - osd_home_alt);
  if(osd_got_home == 0 && gps_type == 3){
    osd_home_lat = gps_latitude;
    osd_home_lon = gps_longitude;
    
    osd_got_home = 1;
  }
  else if(osd_got_home == 1) {
    double scaleLongDown = cos(abs(osd_home_lat) * 0.0174532925);
    
    dstlat = fabs(osd_home_lat - gps_latitude) * 111319.5;
    dstlon = fabs(osd_home_lon - gps_longitude) * 111319.5 * scaleLongDown;
    osd_home_distance = sqrt(sq(dstlat) + sq(dstlon));
  }
}

void _LCD_init() {
  lcdSerial.println("CLS(0)");
  lcdSerial.print("BOX(0,0,200,120,33);");
  lcdSerial.print("BOX(200,0,399,120,33);");
  lcdSerial.print("BOX(0,120,200,239,33);");
  lcdSerial.print("BOX(200,120,399,239,33);");
  lcdSerial.print("PL(100,0,100,119,33);");
  lcdSerial.println("PIC(10,8,2);");
  delay(200);
  lcdSerial.print("DS24(10,60,' 0.0 V',13);"); //battery voltage
  lcdSerial.print("DS24(10,90,' 0.0 A',13);"); //battery amp
  lcdSerial.println("PIC(110,8,3);");
  delay(200);
  lcdSerial.print("DS24(110,55,'0D/S:0',13);");
  lcdSerial.print("DS16(110,85,'0.0000000',13);");
  lcdSerial.print("DS16(110,100,'0.0000000',13);");
  //lcdSerial.println("PIC(240,10,4);");
  delay(200);
  lcdSerial.print("BS16(205,10,390,4,'Here will show status messages.',13);");
  //lcdSerial.print("DS16(320,10,'AccZ',13);");
  //lcdSerial.print("DS16(210,60,'AccX',13);");
  //lcdSerial.print("DS16(340,60,'AccY',13);");
  //lcdSerial.println("PIC(210,125,5);");
  //delay(200);
  lcdSerial.print("DS24(210,125,'Alt:0m',13);");
  lcdSerial.println("DS24(210,150,'Home:0m',13);");
  lcdSerial.println("DS24(210,175,'HDOP:',13);");
  delay(200);
  lcdSerial.println("DQX(2,125,5,10,35,11);");
}

void _Mavlink_lcd_setup() {
  lcdSerial.begin(lcdSerialBaud);
  startTime = 0;
  _LCD_init();
}

void _Mavlink_lcd_show() {
  duration = millis() - startTime;
  if(duration >= delay_lcd_Show_time)
  {
    startTime = millis();
    
    memset(vol_display_bf, 0x0, sizeof(vol_display_bf));
    sprintf(vol_display_bf,"DS24(8,60,'%.3fV',13);", (float)ap_voltage_battery/1000);
    lcdSerial.print(vol_display_bf); //battery voltage

    memset(amp_display_bf, 0x0, sizeof(amp_display_bf));
    sprintf(amp_display_bf,"DS24(10,90,'%.2fA',13);", (float)ap_current_battery/100);
    lcdSerial.print(amp_display_bf); //battery voltage
    
    //IMU
    /*memset(imu_display_xacc_bf, 0x0, sizeof(imu_display_xacc_bf));
    memset(imu_display_yacc_bf, 0x0, sizeof(imu_display_yacc_bf));
    memset(imu_display_zacc_bf, 0x0, sizeof(imu_display_zacc_bf));
    
    sprintf(imu_display_zacc_bf, "DS24(320,10,'%4d  ',13);", imu_zacc);
    sprintf(imu_display_xacc_bf, "DS24(210,60,'%4d  ',13);", imu_xacc);
    sprintf(imu_display_yacc_bf, "DS24(330,60,'%4d  ',13);", imu_yacc);
    lcdSerial.print(imu_display_xacc_bf);
    lcdSerial.print(imu_display_yacc_bf);
    lcdSerial.print(imu_display_zacc_bf);
    #ifdef DEBUG_LCD_MSGS
      debugSerial.println(imu_display_xacc_bf);
      debugSerial.println(imu_display_yacc_bf);
      debugSerial.println(imu_display_zacc_bf);
    #endif
    */

    //Status text
    memset(statustext_bf, 0x0, sizeof(statustext_bf));
    sprintf(statustext_bf, "BS16(205,10,390,4,'%s  ',13);", statustext.text);
    lcdSerial.print(statustext_bf);
    
    //GPS
    if(gps_alt>=0) {
      memset(bar_alt_bf, 0x0, sizeof(bar_alt_bf));
      sprintf(bar_alt_bf, "DS24(210,125,'Alt:%d m',13);",(int)ap_bar_altitude/100);
      lcdSerial.print(bar_alt_bf);
    }
    memset(gps_type_bf, 0x0, sizeof(gps_type_bf));
    sprintf(gps_type_bf, "DS24(110,55,'%dD/S:%2d ',13);",gps_type, gps_satellites);
    lcdSerial.print(gps_type_bf);
    
    //GPS latitude/longitude
    memset(gps_lat_bf, 0x0, sizeof(gps_lat_bf));
    memset(gps_longt_bf, 0x0, sizeof(gps_longt_bf));
    sprintf(gps_lat_bf, "DS16(110,85,'%f',13);",gps_latitude);
    sprintf(gps_longt_bf, "DS16(110,100,'%f',13);", gps_longitude);
    #ifdef DEBUG_LCD_MSGS
      debugSerial.println(gps_lat_bf);
      debugSerial.println(gps_longt_bf);
    #endif
    lcdSerial.print(gps_lat_bf);
    lcdSerial.print(gps_longt_bf);

    //Home distance
    getHomeVars();
    memset(gps_home_bf, 0x0, sizeof(gps_home_bf));
    sprintf(gps_home_bf, "DS24(210,150,'Home:%ldm',13);", osd_home_distance);
    lcdSerial.print(gps_home_bf);

    //HDOP
    memset(gps_hdop_bf, 0x0, sizeof(gps_hdop_bf));
    sprintf(gps_hdop_bf, "DS24(210,175,'HDOP:%.2f',13);",(float)ap_gps_hdop/100);
    lcdSerial.print(gps_hdop_bf);
    
    //rssi
    memset(rssi_display_bf, 0x0, sizeof(rssi_display_bf));
    sprintf(rssi_display_bf,"S%d;", ap_rssi);
    lcdSerial.println(rssi_display_bf);

  }

}
