#include <SparkFun_u-blox_GNSS_v3.h>

// --- GNSS Definitions ---
SFE_UBLOX_GNSS myGNSS;

volatile float lat = 0;
volatile float longi = 0;
volatile float alt = 0;
volatile long speed_long = 0;
volatile int8_t fix_type = 0;
volatile uint8_t hour = 0;
volatile uint8_t minute = 0;
volatile uint8_t sec = 0;
volatile uint8_t save_enable = 0;

int PVTUpdate (uint8_t save_enable) {
  fix_type = myGNSS.getFixType();

  int32_t latitude = myGNSS.getLatitude();
  lat = (float) latitude;
  lat *= (0.0000001);

  int32_t longitude = myGNSS.getLongitude();
  longi = (float) longitude;
  longi *= (0.0000001);

  int32_t altitude = myGNSS.getAltitudeMSL(); // Altitude above Mean Sea Level
  alt = (float) altitude;
  alt /= 1000;

  speed_long = myGNSS.getGroundSpeed(); // Speed in mm/s (library default is often mm/s, check documentation)
  speed_long /= 1000;

  hour = myGNSS.getHour();
  minute = myGNSS.getMinute();
  sec = myGNSS.getSecond();

  if (save_enable == 1) {
    //Do the SD card saving stuff here
  }

  return 1;
}