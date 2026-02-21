#include <SparkFun_u-blox_GNSS_v3.h>
#include <DFRobot_BMM350.h>

#define MAG_I2C 0x14
DFRobot_BMM350_I2C bmm350(&Wire, MAG_I2C);

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
volatile float compassDegree = 0;
volatile uint8_t save_enable = 0;

//hard iron calibration parameters
const float hard_iron[3] = { -3.3, -34.77, -19.67 };
//soft iron calibration parameters
const float soft_iron[3][3] = {
  { 1, 0, 0 },
  { 0, 1, 0 },
  { 0, 0, 1 }
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
  speed_long /= 1000*2.237;                    //mph

  year = myGNSS.getYear();
  month = myGNSS.getMonth();
  day = myGNSS.getDay();
  hour = myGNSS.getHour();
  minute = myGNSS.getMinute();
  sec = myGNSS.getSecond();

  compassDegree = getCompassDegree();

  return 1;
}

void init_mag() {
  while (bmm350.begin()) {
    Serial.println("bmm350 init failed, Please try again!");
    delay(1000);
  }
  Serial.println("bmm350 init success!");
  bmm350.setOperationMode(eBmm350NormalMode);
  bmm350.setPresetMode(BMM350_PRESETMODE_HIGHACCURACY,BMM350_DATA_RATE_25HZ);
  bmm350.setMeasurementXYZ(); //Probably don't need Z enabled but whatever, do later.
}



