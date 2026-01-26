#include <DFRobot_BMM350.h>

#define MAG_I2C 0x14
DFRobot_BMM350_I2C bmm350(&Wire, MAG_I2C);

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

