#ifndef GPS_HEADER_H
#define GPS_HEADER_H

#include <SparkFun_u-blox_GNSS_v3.h>
#include <DFRobot_BMM350.h>

#define MAG_I2C 0x14
#define VBATPIN 35 
DFRobot_BMM350_I2C bmm350(&Wire, MAG_I2C);
SFE_UBLOX_GNSS myGNSS;

volatile float lat = 0, longi = 0, alt = 0, compassDegree = 0;
volatile long speed_long = 0;
volatile uint8_t fix_type = 0, hour = 0, minute = 0, sec = 0;
volatile uint16_t year = 0, month = 0, day = 0;

const float hard_iron[3] = { -3.3, -34.77, -19.67 };

float getCompassDegree() {
  sBmm350MagData_t magData = bmm350.getGeomagneticData();
  float compass = atan2(magData.float_x - hard_iron[0], magData.float_y - hard_iron[1]);
  if (compass < 0) compass += 2 * PI;
  return compass * 180 / M_PI;
}

void init_gps() {
  if (myGNSS.begin(Wire)) {
    myGNSS.setI2COutput(COM_TYPE_UBX); 
    myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); 
  }
}

int PVTUpdate () {
  fix_type = myGNSS.getFixType();
  lat = (float)myGNSS.getLatitude() * 0.0000001;
  longi = (float)myGNSS.getLongitude() * 0.0000001;
  alt = ((float)myGNSS.getAltitudeMSL() / 1000.0) * 3.28084;
  speed_long = (myGNSS.getGroundSpeed() / 1000.0) * 2.237;

  int pstHour = myGNSS.getHour() - 8;
  if (pstHour < 0) pstHour += 24;
  hour = (uint8_t)pstHour;
  minute = myGNSS.getMinute();
  sec = myGNSS.getSecond();
  year = myGNSS.getYear();
  month = myGNSS.getMonth();
  day = myGNSS.getDay();
  
  compassDegree = getCompassDegree();
  return 1;
}

int checkBatteryLevel() {
  float v = (analogRead(VBATPIN) / 4095.0) * 3.3 * 2.0;
  return map(constrain(v * 100, 320, 420), 320, 420, 0, 100);
}

void init_mag() {
  if(bmm350.begin() == 0) {
    bmm350.setOperationMode(eBmm350NormalMode);
    bmm350.setPresetMode(BMM350_PRESETMODE_HIGHACCURACY, BMM350_DATA_RATE_25HZ);
  }
}
#endif
