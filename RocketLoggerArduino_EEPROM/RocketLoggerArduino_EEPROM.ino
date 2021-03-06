#include <Wire.h>
#include <SPI.h>
#include "Adafruit_BMP280.h"
#include "MPU9250.h"
#include <EEPROM.h>

#define INTERVAL 100 // ms
//#define LOG_SIZE 12 //float(4 bytes) x 3 fields -10
#define IND_PIN 13 // LED or indicator PIN
#define WAIT_UNTIL_START_RECORDING50000 // wait after LED is ON

struct LoggerObject { // 10 fields of float
  float acc[3]; // X,Y,Z
//  float gyro[3];
//  float mag[3];
//  float pressure;
};
int volatile eeAddress = 0;

MPU9250 mpu;

Adafruit_BMP280 bmp; // use I2C interface
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();

void setup() {
  Serial.begin(9600);
  Serial.println(F("BMP280 Sensor event test"));
  Wire.begin();
  delay(2000);

  pinMode(LED_BUILTIN, OUTPUT);
  
  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1) delay(10);
  }

  if (!mpu.setup(0x68)) {  // change to your own address
    while (1) {
      Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
      delay(5000);
    }
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  bmp_temp->printSensorDetails();
  
  // Empty byte first to indicate data begin
  eeAddress += 1;
  
  digitalWrite(IND_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(WAIT_UNTIL_START_RECORDING);

  // EEPROM Clear
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}

void loop() {
  if (mpu.update()) {
        static uint32_t prev_ms = millis();
        if (millis() > prev_ms + INTERVAL) {
          
            //print_roll_pitch_yaw();
            
//            sensors_event_t temp_event, pressure_event;
//            bmp_temp->getEvent(&temp_event);
//            bmp_pressure->getEvent(&pressure_event);
            
//            Serial.print(F("Temperature = "));
//            Serial.print(temp_event.temperature);
//            Serial.println(" *C");
          
//            Serial.print(F("Pressure = "));
//            Serial.print(pressure_event.pressure);
//            Serial.println(" hPa");

            LoggerObject data = {
              {mpu.getAcc(0), mpu.getAcc(1), mpu.getAcc(2)},
//              {mpu.getGyro(0), mpu.getGyro(1), mpu.getGyro(2)},
//              {mpu.getMag(0), mpu.getMag(1), mpu.getMag(2)},
//              (float) pressure_event.pressure
            };

            if(EEPROM.length() - eeAddress > sizeof(data)) {
              EEPROM.put(eeAddress, data);
//              Serial.println(data.pressure);
              eeAddress += sizeof(data);
            } else {
              digitalWrite(IND_PIN, LOW);    // turn the LED off by making the voltage LOW
              Serial.println("EEPROOM full. Please use Read Script.");
            }
            
//            Serial.println(eeAddress);
            
            prev_ms = millis();
        }
    }
}

//void print_roll_pitch_yaw() {
//    Serial.print("Yaw, Pitch, Roll: ");
//    Serial.print(mpu.getYaw(), 2);
//    Serial.print(", ");
//    Serial.print(mpu.getPitch(), 2);
//    Serial.print(", ");
//    Serial.println(mpu.getRoll(), 2);
//}
