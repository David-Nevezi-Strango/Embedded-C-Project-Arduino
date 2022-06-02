/***************************************************************************
  This is a library for the BMP280 humidity, temperature & pressure sensor
  This example shows how to take Sensor Events instead of direct readings
  
  Designed specifically to work with the Adafruit BMP280 Breakout
  ----> http://www.adafruit.com/products/2651

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include "LDR.h" // Include Photoresistor library
#include <Servo.h> // Include Servo Library
//#include <DHT.h>
//#include <DHT_U.h>

// Define Slave I2C Address
#define SLAVE_ADDR 9

// Define pins for BMP
#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 10

// Define Photoresistor pins
#define LDR_1_PIN A1
#define LDR_2_PIN A2
#define LDR_3_PIN A3

// Define Servo pin
#define SERVOPIN 3

/*#define DHTPIN 5     // Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)
DHT_Unified dht(DHTPIN, DHTTYPE);*/


// Create BMP Sensor readers
Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();

// Create Servo
Servo myservo;

//uint32_t delayMS;
byte message = -1;
char response[7];
uint8_t degree = 0;

uint8_t isManual = 0;
uint16_t dayValue = 512;

// Global variables and defines
#define THRESHOLD_ldr_1   100
int ldr_1AverageLight;
#define THRESHOLD_ldr_2   100
int ldr_2AverageLight;
#define THRESHOLD_ldr_3   100
int ldr_3AverageLight;
// object initialization
LDR ldr_1(LDR_1_PIN);
LDR ldr_2(LDR_2_PIN);
LDR ldr_3(LDR_3_PIN);


void setup() {
  //Initialize I2C communication
  Wire.begin(SLAVE_ADDR);
  Wire.onRequest(requestEvent);
  Wire.onReceive(recieveEvent);

  Serial.begin(9600);
  while ( !Serial ) delay(100);   // wait for native usb
  Serial.println(F("BMP280 Sensor event test"));
  
  unsigned status;
  //status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
  status = bmp.begin();
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  bmp_temp->printSensorDetails();

  /*// Initialize device.
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.*/
  //delayMS = sensor.min_delay / 1000;
  
  //init Servo
  myservo.attach(SERVOPIN);

  // Setup Serial which is useful for debugging
    // Use the Serial Monitor to view printed messages
    Serial.println("start");

}


void recieveEvent(){
  while(Wire.available() > 0){
    message = Wire.read();
    if(message == 4){ //if message is mode auto
      isManual = 0;
    }else if(message == 5){ //if message is mode manual
      isManual = 1;
    }
    if((message % 10) == 0){// sets the servo position according to the scaled value, if corresponding signal has been sent
      degree = message;
    }
  }
  //Serial.print("Message recieved:");
}

void sendTemp(){
  //function to collect temperature data and insert it into response variable
  sensors_event_t temp_event;
  bmp_temp->getEvent(&temp_event);
  delay(100);
  
//    Serial.print(F("Temperature = "));
//    Serial.print(temp_event.temperature);
//    Serial.println(" *C");
  
  dtostrf(temp_event.temperature,4,2, response);
}
/*
void sendHumid(){
  sensors_event_t humidity_event;
  dht.humidity().getEvent(&humidity_event);
  delay(500);
//  
//      Serial.print(F("Humidity: "));
//      Serial.print(humidity_event.relative_humidity);
//      Serial.println(F("%"));

  dtostrf(humidity_event.relative_humidity,4,2, response);
}
*/

void sendPres(){
  //function to collect pressure data and insert it into response variable
  sensors_event_t pressure_event;
  bmp_pressure->getEvent(&pressure_event);
  delay(100);

//    Serial.print(F("Pressure = "));
//    Serial.print(pressure_event.pressure);
//    Serial.println(" hPa");
  dtostrf(pressure_event.pressure,4,2, response);
  
}
void requestEvent(){
  //Serial.println("Request handling");
  //sensors_event_t temp_event, pressure_event, humidity_event;

  if(message == 1) {
    sendTemp();
    Wire.write(response);
  }
  /*else if (message == 1) {
    sendHumid();
    Wire.write(response);
  }*/
  else if (message == 3 ) {
    sendPres();
    Wire.write(response);
  }
}

void loop() {
  Serial.println("\nBMP--------------------------------------\n");   
  sensors_event_t temp_event, pressure_event;
  bmp_temp->getEvent(&temp_event);
  bmp_pressure->getEvent(&pressure_event);
  
  Serial.print(F("Temperature = "));
  Serial.print(temp_event.temperature);
  Serial.println(" *C");

  Serial.print(F("Pressure = "));
  Serial.print(pressure_event.pressure);
  Serial.println(" hPa");

  //Serial.println();
  delay(500);
   Serial.println(message);
 /* Serial.println("\nDHT--------------------------------------\n"); 
  // Delay between measurements.
 // delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  //dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }
*/
  Serial.print(F("Message received: "));Serial.println(message);
  Serial.print(F("Message sent: "));Serial.println(response);
    if(!isManual){ 
      //if it is mode auto
          //read light sensor input
          int ldr_east = ldr_1.read();//east
          int ldr_south = ldr_2.read();//south
          int ldr_west = ldr_3.read();//west
      
          //verify if difference between 2 sensors is a small one
          int ldr_se = abs(ldr_south - ldr_east);
          int ldr_sw = abs(ldr_west - ldr_south);
          Serial.print(F("Light east: "));Serial.println(ldr_east);
          Serial.print(F("Light south: "));Serial.println(ldr_south);
          Serial.print(F("Light west: "));Serial.println(ldr_west);
          Serial.print(F("Light Diff12: ")); Serial.println(ldr_se);
          Serial.print(F("Light Diff23: ")); Serial.println(ldr_sw);
          
          //servo is off by ~5-15 degree, normal values are 15,45,90,135,165
          if(ldr_se < 200 && ldr_south > ldr_west && ldr_east > ldr_west) {// if light is between south and eest
            myservo.write(125);
            Serial.println("Light direction: southeast");
          } else if(ldr_sw < 200 && ldr_south > ldr_east && ldr_west > ldr_east) {// if light is between south and west
            myservo.write(50);
            Serial.println("Light direction: southwest");
          } else if(ldr_east > ldr_south && ldr_east > ldr_west){// if light is east
            myservo.write(155);
            Serial.println("Light direction: east");
          } else if(ldr_south > ldr_west && ldr_south > ldr_east){// if light is south
            myservo.write(85);
            Serial.println("Light direction: south");
          } else if(ldr_west > ldr_east && ldr_west > ldr_south){// if light is west
            myservo.write(30);
            Serial.println("Light direction: west");
          } else {
            myservo.write(90);
            Serial.println("Light direction: default");
          }
    }else{
      //if it is mode manual
          Serial.print("Mode Manual: ");
          Serial.println(degree);
          myservo.write(degree);
          delay(100);
    }
}
