#include <SparkFun_u-blox_GNSS_v3.h>

// --- GNSS Definitions ---
SFE_UBLOX_GNSS myGNSS;

volatile float lat = 0;
volatile float longi = 0;
volatile float alt = 0;
volatile long speed_long = 0;
volatile uint8_t fix_type = 0;
volatile uint16_t year = 0;
volatile uint8_t month = 0;
volatile uint8_t day = 0;
volatile uint8_t hour = 0;
volatile uint8_t minute = 0;
volatile uint8_t sec = 0;

volatile uint8_t save_enable = 0;

void init_gps() {
  if (myGNSS.begin(Wire)) { // Use Wire for I2C
    Serial.println(F("u-blox GNSS initialized successfully!"));
    //display.setCursor(0, 9);
    //display.println("GNSS Ready");
    //display.display();
    myGNSS.setI2COutput(COM_TYPE_UBX); // Set I2C port to output UBX messages (less noisy than NMEA)
    myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); // Save settings
  } else {
    Serial.println(F("Failed to initialize u-blox GNSS"));
    //display.setCursor(0, 19);
    //display.println("GNSS Error!");
    //display.display();
    while(1);
  }
}

int PVTUpdate () {
  fix_type = myGNSS.getFixType();

  int32_t latitude = myGNSS.getLatitude();
  lat = (float)latitude;
  lat *= (0.0000001);

  int32_t longitude = myGNSS.getLongitude();
  longi = (float)longitude;
  longi *= (0.0000001);

  int32_t altitude = myGNSS.getAltitudeMSL();  // Altitude above Mean Sea Level
  alt = (float)altitude;
  alt /= 1000;

  speed_long = myGNSS.getGroundSpeed();  // Speed in mm/s (library default is often mm/s, check documentation)
  speed_long /= 1000;                    //km/s

  year = myGNSS.getYear();
  month = myGNSS.getMonth();
  day = myGNSS.getDay();
  hour = myGNSS.getHour();
  minute = myGNSS.getMinute();
  sec = myGNSS.getSecond();
  return 1;
}