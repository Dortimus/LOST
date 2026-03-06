#ifndef GPS_HEADER_H
#define GPS_HEADER_H

#include <SparkFun_u-blox_GNSS_v3.h>
#include <DFRobot_BMM350.h>

#define MAG_I2C 0x14
#define VBATPIN 35 
DFRobot_BMM350_I2C bmm350(&Wire, MAG_I2C);
SFE_UBLOX_GNSS myGNSS;

volatile double lat = 0, longi = 0;
volatile float alt = 0, compassDegree = 0;
volatile long speed_long = 0;
volatile int fix_type = 0, hour = 0, minute = 0, sec = 0;
volatile int year = 0, month = 0, day = 0;

//hard iron calibration parameters
const float hard_iron[3] = { -5.72, -30.74, -19.68 };
//soft iron calibration parameters
const float soft_iron[3][3] = {
  { 1.017, -0.023, -0.019 },
  { -0.023, 1.011, -0.023 },
  { -0.019, -0.023, 0.974 }
};

float getCompassDegree() {
  sBmm350MagData_t magData = bmm350.getGeomagneticData();
  float mag_data[3];
  // hard iron calibration
  mag_data[0] = magData.float_x - hard_iron[0];
  mag_data[1] = magData.float_y - hard_iron[1];
  mag_data[2] = magData.float_z - hard_iron[2];
  //soft iron calibration
  for (int i = 0; i < 3; i++) {
    mag_data[i] = (soft_iron[i][0] * mag_data[0]) + (soft_iron[i][1] * mag_data[1]) + (soft_iron[i][2] * mag_data[2]);
  }
  magData.x = mag_data[0];
  magData.y = mag_data[1];
  magData.z = mag_data[2];
  magData.float_x = mag_data[0];
  magData.float_y = mag_data[1];
  magData.float_z = mag_data[2];

  float compass = 0.0;
  compass = atan2(magData.x, magData.y);
  if (compass < 0) {
    compass += 2 * PI;
  }
  if (compass > 2 * PI) {
    compass -= 2 * PI;
  }
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
  lat = (double)myGNSS.getLatitude() * 0.0000001;
  longi = (double)myGNSS.getLongitude() * 0.0000001;
  alt = ((float)myGNSS.getAltitude() / 1000.0) * 3.28084;
  speed_long = myGNSS.getGroundSpeed() * 0.00223694;

  int pstHour = myGNSS.getHour() - 8;
  if (pstHour < 0) pstHour += 24;
  hour = (int)pstHour;
  minute = myGNSS.getMinute();
  sec = myGNSS.getSecond();
  year = myGNSS.getYear();
  month = myGNSS.getMonth();
  day = myGNSS.getDay();
  

  compassDegree = getCompassDegree();
  if (compassDegree <= 270){
    compassDegree += 90;
  } else {
    compassDegree -= 270;
  }
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

double haversine(double lat1, double lon1, double lat2, double lon2) {
  // Use 3958.8 for slightly better Miles accuracy
  const double R = 3958.8; 
  
  double dLat = radians(lat2 - lat1);
  double dLon = radians(lon2 - lon1);
  
  double rLat1 = radians(lat1);
  double rLat2 = radians(lat2);
  
  double a = sin(dLat / 2.0) * sin(dLat / 2.0) +
             cos(rLat1) * cos(rLat2) *
             sin(dLon / 2.0) * sin(dLon / 2.0);
  
  double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
  return R * c;
}

String getCardinalDirection(float heading) {
  // 1. Ensure heading is between 0 and 360
  while (heading < 0) heading += 360;
  while (heading >= 360) heading -= 360;

  // 2. Define the directions in an array
  const char* directions[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};

  // 3. Offset by 22.5 degrees so that North (0) sits in the middle of its slice.
  // Then divide by 45 to get an index (0-7).
  int index = (int)((heading + 22.5) / 45.0) % 8;

  return directions[index];
}

#endif
