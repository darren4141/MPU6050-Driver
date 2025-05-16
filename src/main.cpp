#include <Arduino.h>
#include <Wire.h>

#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_ADDR 0x68

byte I2C_Scan();

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
}

void loop() {
  //write raw data example:
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0x01); //register
  Wire.write(0xAB);
  Wire.endTransmission();

  //read raw data example:
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0x01); //register
  Wire.endTransmission(false); //send a restart

  Wire.requestFrom(I2C_ADDR, 1); //request 1 byte

  if(Wire.available()){
    uint8_t recieved = Wire.read();
    Serial.println(recieved, HEX);
  }

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