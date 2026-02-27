#include <SparkFun_u-blox_GNSS_v3.h>
#include <DFRobot_BMM350.h>

#define MAG_I2C 0x14
#define VBATPIN A0
DFRobot_BMM350_I2C bmm350(&Wire, MAG_I2C);
SFE_UBLOX_GNSS myGNSS;

// --- SHARED VARIABLES ---
volatile float lat = 0, longi = 0, alt = 0;
volatile long speed_long = 0;
volatile uint8_t fix_type = 0;
volatile uint16_t year = 0;
volatile uint8_t month = 0, day = 0, hour = 0, minute = 0, sec = 0;
volatile float compassDegree = 0;

// Your proven calibration values
const float hard_iron[3] = { -3.3, -34.77, -19.67 };
const float soft_iron[3][3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };

float getCompassDegree() {
  sBmm350MagData_t magData = bmm350.getGeomagneticData();
  float mag_x = magData.float_x - hard_iron[0];
  float mag_y = magData.float_y - hard_iron[1];

  // Soft iron application
  float corrected_x = (soft_iron[0][0] * mag_x) + (soft_iron[0][1] * mag_y);
  float corrected_y = (soft_iron[1][0] * mag_x) + (soft_iron[1][1] * mag_y);

  float heading = atan2(corrected_x, corrected_y);
  if (heading < 0) heading += 2 * M_PI;
  return heading * 180.0 / M_PI;
}

void init_gps() {
  if (myGNSS.begin(Wire)) {
    myGNSS.setI2COutput(COM_TYPE_UBX);
    myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);
  }
}

int PVTUpdate() {
  fix_type = myGNSS.getFixType();
  lat = (float)myGNSS.getLatitude() * 0.0000001;
  longi = (float)myGNSS.getLongitude() * 0.0000001;
  
  // Altitude MSL
  alt = (float)myGNSS.getAltitudeMSL() / 1000.0;
  
  speed_long = (myGNSS.getGroundSpeed() / 1000.0) * 2.237; 
  
  // Update ALL time variables for the SD file naming
  year = myGNSS.getYear();
  month = myGNSS.getMonth();
  day = myGNSS.getDay();
  hour = myGNSS.getHour();
  minute = myGNSS.getMinute();
  sec = myGNSS.getSecond();
  
  return 1;
}

void init_mag() {
  if (bmm350.begin() == 0) {
    bmm350.setOperationMode(eBmm350NormalMode);
    bmm350.setPresetMode(BMM350_PRESETMODE_HIGHACCURACY, BMM350_DATA_RATE_25HZ);
    bmm350.setMeasurementXYZ();
  }
}

int checkBatteryLevel() {
  float v = (analogRead(VBATPIN) * 2.0 * 3.3) / 4095.0;
  return map(constrain(v * 100, 320, 420), 320, 420, 0, 100);
}
