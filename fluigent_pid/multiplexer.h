#include "Wire.h"
#define TCAADDR 0x70
const int ADDRESS = 0x08;
int active_sensors[8];
int ret;

void scan_i2c() {
  byte error, address;
  Serial.print("I2C device: \t");
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("Øx");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.print("\t");
    }
  }
  Serial.println("");
  delay(100);
}



void tcaselect(uint8_t i) {
  if (i > 7)
    return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void reset_mlpx(int flag_print) {

  Serial.begin(9600);  // initialize serial communication
  Wire.begin();        // join i2c bus (address optional for master)
  //Serial.print("reset:Sensor on TCA ports: \t");
  // Try to reset the sensor via each of the 8 TCA's
  // ports to check for sensor presence.
  for (int i = 0; i < 8; i++) {
    tcaselect(i);
    // Soft reset the sensor to check
    Wire.beginTransmission(0x00);
    Wire.write(0x06);
    int ret = Wire.endTransmission();
    if (ret != 0) {
      if (flag_print == 1) {
        Serial.print("No sensor detected on TCA port #");
        Serial.println(i);
        active_sensors[i] = 0;
      }
    } else {
      if (flag_print == 1) {
        Serial.print("#");
        Serial.print(i);
        Serial.print("\t");

        active_sensors[i] = 1;
      }
    }
  }
  delay(100);
}


void setup_mlpx() {
  Serial.print("Sensor on TCA port: \t");
  // Try to reset the sensor via each of the 8 TCA's
  // ports to check for sensor presence.
  for (int i = 0; i < 8; i++) {
    tcaselect(i);
    // Soft reset the sensor to check
    Wire.beginTransmission(0x00);
    Wire.write(0x06);
    ret = Wire.endTransmission();
    if (ret != 0) {
      // Serial.print("No sensor detected on TCA port #");
      // Serial.println(i);
      active_sensors[i] = 0;

    } else {
      //Serial.print("#");
      //Serial.print(i);
      //Serial.print("\t");
      active_sensors[i] = 1;
    }
  }
  delay(50);  // wait long enough for chip reset to complete

  for (int i = 0; i < 8; i++) {
    if (active_sensors[i] == 0) {
      continue;  // dismiss if this port is not active.
    }
    tcaselect(i);
    // SETUP FLUXMETERS
    Wire.beginTransmission(ADDRESS);
    Wire.write(0x36);
    Wire.write(0x08);
    ret = Wire.endTransmission();
  }
}

void scan_mlpx() {
  Serial.println("scanmlpx :Sensor on TCA port: \t");
  // Try to reset the sensor via each of the 8 TCA's
  // ports to check for sensor presence.
  for (int i = 0; i < 8; i++) {
    tcaselect(i);
    // Soft reset the sensor to check
    Wire.beginTransmission(0x00);
    Wire.write(0x06);
    ret = Wire.endTransmission();
    if (ret != 0) {
      Serial.print("No #");
      Serial.println(i);
      active_sensors[i] = 0;

    } else {
       Serial.print("yes #");
      Serial.println(i);
      active_sensors[i] = 1;
    }
  }
  Serial.println();
  delay(50);  // wait long enough for chip reset to complete

  for (int i = 0; i < 8; i++) {
    if (active_sensors[i] == 0) {
      continue;  // dismiss if this port is not active.
    }
    tcaselect(i);

    // Send 0x3608 to switch to continuous measurement mode (H20 calibration).
    // Check datasheet for available measurement commands.
    Wire.beginTransmission(ADDRESS);
    Wire.write(0x36);
    Wire.write(0x08);
    ret = Wire.endTransmission();
    if (ret != 0) {
      Serial.print("Error during write measurement mode command on TCA port #");
      Serial.print(i);
      Serial.println("!");
    } else {
      Serial.print("Measurement on TCA port #");
      Serial.print(i);
      Serial.println(" started!");
    }
  }
}
// ----------
//#include "pressione.h"
#include "Flux_Print.h"

