#include <Arduino.h>
#include <Wire.h>

#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_ADDR 0x68

#define ACCEL_XOUT_H 0x3B //XOUT to ZOUT is 6 bytes from 0x3B to 0x40

byte I2C_Scan();
void get_accel_data(uint16_t (&xout)[3]);

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

  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0x6B);
  Wire.write(0x0);
  Wire.endTransmission();

}

void loop() {
  // //write raw data example:
  // Wire.beginTransmission(I2C_ADDR);
  // Wire.write(0x01); //register
  // Wire.write(0xAB);
  // Wire.endTransmission();

  // //read raw data example:
  // Wire.beginTransmission(I2C_ADDR);
  // Wire.write(0x01); //register
  // Wire.endTransmission(false); //send a restart

  // Wire.requestFrom(I2C_ADDR, 1); //request 1 byte

  // if(Wire.available()){
  //   uint8_t recieved = Wire.read();
  //   Serial.println(recieved, HEX);
  // }
  uint16_t xout_data[3] = {0x00, 0x00, 0x00};

  get_accel_data(xout_data);

  for(int i = 0; i < 3; i++){
    Serial.print(xout_data[i]);
  }

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

void get_accel_data(uint16_t (&accel)[3]){

  Wire.beginTransmission(I2C_ADDR);
  Wire.write(ACCEL_XOUT_H);
  Wire.endTransmission(false);

  Wire.requestFrom(I2C_ADDR, 6);
  if(Wire.available()){
    accel[0] = Wire.read() << 8 | Wire.read();
    accel[1] = Wire.read() << 8 | Wire.read();
    accel[2] = Wire.read() << 8 | Wire.read();
  }
}