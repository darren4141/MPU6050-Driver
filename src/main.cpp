#include <Arduino.h>
#include <Wire.h>

#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_ADDR 0x68

#define ACCEL_XOUT_H 0x3B //XOUT to ZOUT is 6 bytes from 0x3B to 0x40
float pitchOffset = 0;
float rollOffset = 0;

byte I2C_Scan();

class Accelerometer{

  public:
    Accelerometer();
    void wakeUp();
    void update_accel_data();
    int16_t ax;
    int16_t ay;
    int16_t az;

};

Accelerometer accel;

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);
  int scanned_addr = I2C_Scan();

  if(scanned_addr == 0){
    Serial.println("No I2C Devices found");
  }else{
    Serial.print("I2C Device found at: ");
    Serial.println(scanned_addr, HEX);
  }

  accel.wakeUp();

  calibrateOffsets();

}


void loop() {

  accel.update_accel_data();

  float pitch = atan2(accel.ax, sqrt(accel.ay * accel.ay + accel.az * accel.az)) * 180.0 / PI - pitchOffset;
  float roll = atan2(accel.ay, sqrt(accel.ax * accel.ax + accel.az * accel.az)) * 180.0 / PI - rollOffset;

  Serial.print(pitch);
  Serial.print(" ");
  Serial.print(roll);

  Serial.println();

  delay(100);
  
}

byte I2C_Scan(){
  byte error;
  byte address;

  for(address = 1; address < 127; address++){
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if(error == 0){
      break;
    }
  }

  if(error == 0){
    return address;
  }else{
    return 0x00;
  }
}

Accelerometer::Accelerometer() : ax(0), ay(0), az(0){

}

void Accelerometer::wakeUp(){
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0x6B);
  Wire.write(0x0);
  Wire.endTransmission();
}

void Accelerometer::update_accel_data(){
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(ACCEL_XOUT_H);
  Wire.endTransmission(false);

  Wire.requestFrom(I2C_ADDR, 6);

  if(Wire.available() == 6){
    ax = Wire.read() << 8 | Wire.read();
    ay = Wire.read() << 8 | Wire.read();
    az = Wire.read() << 8 | Wire.read();
  }

}

void calibrateOffsets() {
  int16_t ax, ay, az;
  float pitchSum = 0, rollSum = 0;

  for (int i = 0; i < 20; i++) {
    accel.update_accel_data();
    ax = accel.ax;
    ay = accel.ay;
    az = accel.az;
    float pitch = atan2(ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
    float roll = atan2(ay, sqrt(ax * ax + az * az)) * 180.0 / PI;
    pitchSum += pitch;
    rollSum += roll;
    delay(50);
  }

  pitchOffset = pitchSum / 20.0;
  rollOffset = rollSum / 20.0;
  Serial.println("Calibration done");
}