#include <Wire.h> // Include I2C Communication Library
#include <IRremote.h> // Include IR Remote library 
#include <Adafruit_GFX.h>  // Include core graphics library
#include <Adafruit_ST7735.h>  // Include Adafruit_ST7735 library to drive the display
#include <virtuabotixRTC.h> // Include RTC library
//#include <Fonts/FreeSerif18pt7b.h> // Include special fonts for printing texts
#include <DHT.h> // Include DHT libraries
#include <DHT_U.h>



// Define slave address
#define SLAVE_ADDR 9

// Define pins for the IR Remote Receiver
#define IR_RECEIVE_PIN 2

// Define pins for the display
#define TFT_CS     10
#define TFT_RST    4  // You can also connect this to the Arduino reset in which case, set this #define pin to -1!
#define TFT_DC     9
// The rest of the pins are pre-selected as the default hardware SPI for Arduino Uno (SCK = 13 and SDA = 11)

// Define pins for the RTC
#define RTC_CLK     6
#define RTC_DAT     7
#define RTC_RST     8

// Define DHT pin and DHT Type
#define DHTPIN 5
#define DHTTYPE    DHT11

// Define main screen options
#define TEMPERATURE 1
#define HUMIDITY 2
#define PRESSURE 3
#define OVERRIDE 4

// Define isManual Transmission values
#define MANUAL_TRUE 5
#define MANUAL_FALSE 4

// Define other component setup messages
#define ARDUINO 6
#define BMP280 7 
#define SERVO 8
#define LDR 9


// Create DHT 
DHT_Unified dht(DHTPIN, DHTTYPE);
// Create display:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
// Create RTC 
virtuabotixRTC myRTC(RTC_CLK, RTC_DAT, RTC_RST);


// Create variables to memorize every state of screen 
int8_t lastScreen = -1; //the last selected screen according to the defines
int8_t selectedScreen = 0; //the selected screen according to the defines
int8_t currentScreen = -1; //show the selected screen according the defines
int8_t isFirstTime = 0; //boolean if it is the first time entering into a screen.
                      //Useful to not refresh screen every second
uint8_t isManual = 0;                      
uint8_t lastMinute = 0;//memorize last minute.
                             //Useful to not refresh screen every second
uint8_t servoDegree = 0; // int to memorize the servo degree
void setup(){
  //init I2C Communication 
  Wire.begin();
  
  //init Serial bus
  Serial.begin(9600);
  while (!Serial);
  // Display setup:

  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);  // Initialize a ST7735S chip, black tab

  tft.fillScreen(ST7735_BLACK);  // Fill screen with black

  tft.setRotation(0);  // Set orientation of the display. Values are from 0 to 3. If not declared, orientation would be 0,
                         // which is portrait mode.

  tft.setTextWrap(true);  // By default, long lines of text are set to automatically “wrap” back to the leftmost column.
                           // To override this behavior (so text will run off the right side of the display - useful for
                           // scrolling marquee effects), use setTextWrap(false). The normal wrapping behavior is restored
                           // with setTextWrap(true).

  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  //myRTC.setDS1302Time(00, 16, 14, 6, 21, 5, 2022);
  Bootload();
  
}
void Bootload(){
  String response = "";
  
  tft.setTextColor(ST7735_MAGENTA);
  tft.setCursor(10,10);
  tft.print("Starting bootload");
  tft.setCursor(10,10);
  tft.drawFastHLine(0,20,127, ST7735_MAGENTA);
  delay(300);
  tft.setCursor(110,10);
  tft.print(".");
  delay(350);
  tft.setCursor(115,10);
  tft.print(".");
  delay(350);
  tft.setCursor(120,10);
  tft.print(".");
  delay(300);
  
  tft.setCursor(5,25);
  tft.print("Loading Remote");
  
  //init IR Remote
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  
  tft.print(".");
  delay(350);
  tft.print(".");
  delay(350);
  tft.print(".");
  delay(350);
  tft.print(".");
  delay(100);
  tft.print("OK");
  
  delay(200);
  tft.setCursor(5,40);
  tft.print("Loading DHT11");
  delay(100);
  tft.print(".");
  delay(166);
  tft.print(".");  
  delay(166);
  tft.print(".");
  delay(166);
  tft.print(".");
  delay(166);
  tft.print(".");
  delay(100);
  tft.print("OK");
  delay(200);
  
  tft.setCursor(5,55);
  tft.print("Loading Arduino");
  tft.print(".");
  delay(200);
  tft.print(".");
  delay(200);
  tft.print(".");
  delay(250);
  tft.print("OK");
  delay(200);
  
  tft.setCursor(5,70);
  tft.print("Loading BMP280");
  tft.print(".");
  delay(250);
  tft.print(".");
  delay(250);
  tft.print(".");
  delay(200);
  tft.print(".");
  delay(250);
  tft.print("OK");
  delay(200);
  
  tft.setCursor(5,85);
  tft.print("Loading Servo");
  tft.print(".");
  delay(200);
  tft.print(".");
  delay(200);
  tft.print(".");
  delay(200);
  tft.print(".");
  delay(200);
  tft.print(".");
  delay(100);
  tft.print("OK");
  delay(200);
  
  tft.setCursor(5,100);
  tft.print("Loading LDR");
  tft.print(".");
  delay(150);
  tft.print(".");
  delay(150);
  tft.print(".");
  delay(150);
  tft.print(".");
  delay(150);
  tft.print(".");
  delay(150);
  tft.print(".");
  delay(150);
  tft.print(".");
  delay(200);
  tft.print("OK");
  delay(200);
}

void updateDegree(){
  if(!isManual){
    return ;
  }
  tft.setTextColor(ST7735_MAGENTA);
  tft.fillRect(10,70,160,10,ST7735_BLACK);
  if(servoDegree > 99){
    tft.setCursor(30,70);
  }else if(servoDegree <10){
    tft.setCursor(40,70);
  }else{
    tft.setCursor(35,70);
  }
  tft.print(servoDegree);
  tft.print(" Degrees");
}

void servoIncrease(){ // send signal and degree to turn Servo to the right
  servoDegree += 10;
  if(servoDegree > 180){ //upper limit of 180 degrees
    servoDegree = 180;
  }
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(servoDegree);
  Wire.endTransmission();
  updateDegree();
}

void servoDecrease(){ // send signal and degree to turn Servo to the left
  servoDegree -= 10;
  if(servoDegree > 220){ //if it goes over 0, it would decrement from 255
    servoDegree = 0;
  }
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(servoDegree);
  Wire.endTransmission();
  updateDegree();
}

void updateOverride(){
  tft.fillRect(20,60,160,30,ST7735_BLACK);
  if(isManual){ 
    tft.setTextColor(ST7735_MAGENTA); 
    tft.setCursor(30,60);
    tft.println("MODE: MANUAL");
    updateDegree();
  }
  else{
    tft.setTextColor(ST7735_MAGENTA); 
    tft.setCursor(35,60);
    tft.println("MODE: AUTO");
    servoDegree = 0;
  }
}

void updateTime(){ //update time in header
  lastMinute = myRTC.minutes;
  String dateString = String(myRTC.dayofmonth)+"/"+String(myRTC.month)+"/"+String(myRTC.year);
  String myHour = String(myRTC.hours);
  if(myRTC.minutes<10){
    myHour = myHour+":0"+String(myRTC.minutes);
  }else{
    myHour = myHour+":"+String(myRTC.minutes);
  }
  tft.setTextColor(ST7735_MAGENTA);
  tft.fillRect(10,10,160,10,ST7735_BLACK);
  tft.setCursor(10,10);
  tft.print(myHour);
  tft.setCursor(65,10);
  tft.print(dateString);
}

void screenHeader(){ 
  //function to create the header of the screen
  tft.setTextColor(ST7735_MAGENTA); 
  tft.setCursor(10,10);
  tft.drawFastHLine(0,20,127, ST7735_MAGENTA);
  //print current time
  updateTime();
}

void backButton(){
  //function to create the back button for screens except main screen
  tft.fillRect(5,130,120,25,ST7735_RED);
  tft.setTextColor(ST7735_BLACK);
  tft.setCursor(55,140);
  tft.print("Back");
}

void tempScreen(){//temperature screen
  if(!isFirstTime){//if screen has already been selected, do not refresh it
    return;
  }
  isFirstTime = 0;
  String temperatureValue = "";
  tft.fillScreen(ST7735_BLACK);
  delay(500);
  
  screenHeader();

  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(TEMPERATURE);
  Wire.endTransmission();

  Wire.requestFrom(9,4);
  delay(300);
  while(Wire.available() > 0){
    char c = Wire.read();
    temperatureValue = temperatureValue + c;
  }
  
  tft.setTextColor(ST7735_MAGENTA); 
  tft.setCursor(30,60);
  tft.print("TEMPERATURE");
  if(!temperatureValue.equals("")){
    tft.setCursor(28,70);
    tft.print(temperatureValue);
    tft.print(" Celsius");
  }else {
    tft.setCursor(30,70);
    tft.print("NaN Celsius");
  }

  backButton();
}

void humidScreen(){//humidity screen
  if(!isFirstTime){//if screen has already been selected, do not refresh it
    return;
  }
  isFirstTime = 0;
  String humidityValue = "";
  tft.fillScreen(ST7735_BLACK);
  delay(500);
    
  screenHeader();
  
  sensors_event_t humidity_event;
  dht.humidity().getEvent(&humidity_event);
  
  tft.setTextColor(ST7735_MAGENTA); 
  tft.setCursor(40,60);
  tft.print("HUMIDITY");
  tft.setCursor(40,70);
  //if(!humidityValue.equals("")){
  tft.print(String(humidity_event.relative_humidity));
  tft.print(" %");

  backButton();
}

void presScreen(){//pressure screen
  if(!isFirstTime){//if screen has already been selected, do not refresh it
    return;
  }
  isFirstTime = 0;
  String pressureValue = "";
  tft.fillScreen(ST7735_BLACK);
  delay(500);
    
  screenHeader();

  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(PRESSURE);
  Wire.endTransmission();

  Wire.requestFrom(9,7);
  delay(300);
  while(Wire.available() > 0){
    char c = Wire.read();
    pressureValue = pressureValue + c;
  }

  tft.setTextColor(ST7735_MAGENTA); 
  tft.setCursor(40,60);
  tft.print("PRESSURE");
  if(!pressureValue.equals("")){
    tft.setCursor(32,70);
    tft.print(pressureValue);
    tft.print(" hPa");
  }else {
    tft.setCursor(43,70);
    tft.print("NaN hPa");
  }

  backButton();
}

void overScreen(){//manual override screen
  if(!isFirstTime){//if screen has already been selected, do not refresh it
    return;
  }
  isFirstTime = 0;
  
  tft.fillScreen(ST7735_BLACK);
  delay(500);
    
  screenHeader();
  
  updateOverride();
  
  backButton();
}



void mainScreen(){
  if(selectedScreen == lastScreen){//if main screen has already been selected, do not refresh it
    return;
  }
  //if to not reset screen if we have reached top or bottom
  if(selectedScreen > OVERRIDE){
    selectedScreen = OVERRIDE;
  }
  if(selectedScreen < TEMPERATURE ){
    selectedScreen = TEMPERATURE;
  }
  
  lastScreen = selectedScreen;
  //clear the screen
  tft.fillScreen(ST7735_BLACK);
  delay(500);
  
  screenHeader();
  
  //body
  if(selectedScreen == TEMPERATURE){
    //TEMPERATURE SELECTED
    
    tft.fillRect(5,30,120,25,ST7735_RED);
    tft.setTextColor(ST7735_BLACK);
    tft.setCursor(31,40);
    tft.print("Temperature");
    tft.drawRect(5,60,120,25,ST7735_RED);
    tft.setTextColor(ST7735_RED);
    tft.setCursor(40,70);
    tft.print("Humidity");
    tft.drawRect(5,90,120,25,ST7735_RED);
    tft.setCursor(40,100);
    tft.print("Pressure");
    tft.drawRect(5,120,120,25,ST7735_RED);
    tft.setCursor(20,130);
    tft.print("Manual Override");
    
  } else if (selectedScreen == HUMIDITY){
    //HUMIDITY SELECTED
    
    tft.drawRect(5,30,120,25,ST7735_RED);
    tft.setTextColor(ST7735_RED);
    tft.setCursor(31,40);
    tft.print("Temperature");
    tft.fillRect(5,60,120,25,ST7735_RED);
    tft.setTextColor(ST7735_BLACK);
    tft.setCursor(40,70);
    tft.print("Humidity");
    tft.drawRect(5,90,120,25,ST7735_RED);
    tft.setTextColor(ST7735_RED);
    tft.setCursor(40,100);
    tft.print("Pressure");
    tft.drawRect(5,120,120,25,ST7735_RED);
    tft.setCursor(20,130);
    tft.print("Manual Override");
  } else if (selectedScreen == PRESSURE){
    //ATMOSPHERIC PRESSURE SELECTED
    
    tft.drawRect(5,30,120,25,ST7735_RED);
    tft.setTextColor(ST7735_RED);
    tft.setCursor(31,40);
    tft.print("Temperature");
    tft.drawRect(5,60,120,25,ST7735_RED);
    tft.setCursor(40,70);
    tft.print("Humidity");
    tft.fillRect(5,90,120,25,ST7735_RED);
    tft.setTextColor(ST7735_BLACK);
    tft.setCursor(40,100);
    tft.print("Pressure");
    tft.drawRect(5,120,120,25,ST7735_RED);
    tft.setTextColor(ST7735_RED);
    tft.setCursor(20,130);
    tft.print("Manual Override");
    
  } else if (selectedScreen == OVERRIDE){ 
    //MANUAL OVERRIDE SELECTED
    
    tft.drawRect(5,30,120,25,ST7735_RED);
    tft.setTextColor(ST7735_RED);
    tft.setCursor(31,40);
    tft.print("Temperature");
    tft.drawRect(5,60,120,25,ST7735_RED);
    tft.setCursor(40,70);
    tft.print("Humidity");
    tft.drawRect(5,90,120,25,ST7735_RED);
    tft.setCursor(40,100);
    tft.print("Pressure");
    tft.fillRect(5,120,120,25,ST7735_RED);
    tft.setTextColor(ST7735_BLACK);
    tft.setCursor(20,130);
    tft.print("Manual Override");
  }
  delay(500);
}

void loop(){
  myRTC.updateTime();
//  //For Debug
//  Serial.println("Selected:" + String(selectedScreen));
//  Serial.println("Last:" + String(lastScreen));
//  Serial.println("Current:" + String(currentScreen));
//  delay(1000);
//  Serial.println("\nRTC--------------------------------------\n"); 
//  // This allows for the update of variables for time or accessing the individual elements.              //|
//                                                                                                         //| 
//  // Start printing elements as individuals                                                              //|   
//  Serial.print("Current Date / Time: ");                                                                 //| 
//  Serial.print(myRTC.dayofmonth);                                                                        //| 
//  Serial.print("/");                                                                                     //| 
//  Serial.print(myRTC.month);                                                                             //| 
//  Serial.print("/");                                                                                     //| 
//  Serial.print(myRTC.year);                                                                              //| 
//  Serial.print("  ");                                                                                    //| 
//  Serial.print(myRTC.hours);                                                                             //| 
//  Serial.print(":");                                                                                     //| 
//  Serial.print(myRTC.minutes);                                                                           //| 
//  Serial.print(":");                                                                                     //| 
//  Serial.println(myRTC.seconds);                                                                         //| 
//  delay(1000);                                                                                                       //| 


  
  
  if(currentScreen != -1){//if screen has been selected
    switch(currentScreen){
      case TEMPERATURE:
        tempScreen();
        break;
      case HUMIDITY:
        humidScreen();
        break;
      case PRESSURE:
        presScreen();
        break;
      case OVERRIDE:
        overScreen();
        break;
    }
  }else{
    mainScreen();
  }
  /*if(currentScreen == OVERRIDE){
    updateDegree();
  }*/
  //update time
  if(myRTC.minutes!=lastMinute){
    updateTime();
  }
  if (IrReceiver.decode()){
        unsigned long keycode = IrReceiver.decodedIRData.command;
        Serial.println(keycode);
        if (keycode == 0XFFFFFFFF)
          return;
        switch(keycode){
//          case 69:
//          Serial.println("CH-");
//          break;
//          case 70:
//          Serial.println("CH");
//          break;
//          case 71:
//          Serial.println("CH+");
//          break;
//          case 68:
//          Serial.println("|<<");
//          break;
//          case 64:
//          Serial.println(">>|");
//          break ;  
//          case 67:
//          Serial.println(">||");
//          break ;               
//          case 7:
//          Serial.println("-");
//          break ;  
//          case 21:
//          Serial.println("+");
//          break ;  
//          case 9:
//          Serial.println("EQ");
//          break ;  
//          case 22:
//          Serial.println("0");
//          break ;  
//          case 25:
//          Serial.println("100+");
//          break ;
//          case 13:
//          Serial.println("200+");
//          break ;
//          case 12:
//          Serial.println("1");
//          break ;
          case 24:
          Serial.println("2");
          if(currentScreen == -1){
            --selectedScreen;
          }else if(currentScreen == OVERRIDE){ //if screen is in override, switch override mode and send to slave
            if(isManual == 0){
              isManual = 1;
              Wire.beginTransmission(SLAVE_ADDR);
              Wire.write(MANUAL_TRUE);
              Wire.endTransmission();
            }else{
              isManual = 0;
              Wire.beginTransmission(SLAVE_ADDR);
              Wire.write(MANUAL_FALSE);
              Wire.endTransmission();
            }
            updateOverride();
          }
          break ;
//          case 94:
//          Serial.println("3");
//          break ;
          case 8:
          Serial.println("4");
          if(currentScreen == OVERRIDE){
            servoDecrease();
          }
          break ;
          case 28:
          Serial.println("5");
          if(currentScreen != selectedScreen){ 
            //if option is selected, 
            //memorize the screen that is going to be displayed
            currentScreen = selectedScreen;
            lastScreen = -1;
            isFirstTime = 1;
          } else { // if the back button is pressed
             currentScreen = -1;
          }
          break ;
          case 90:
          Serial.println("6");
          if(currentScreen == OVERRIDE){
            servoIncrease();
          }
          break ;
//          case 66:
//          Serial.println("7");
//          break ;
          case 82:
          Serial.println("8");
          if(currentScreen == -1){
            ++selectedScreen;
          }
          break ;
//          case 74:
//          Serial.println("9");
//          break ;      
        }
        delay(500);
        IrReceiver.resume(); 
  } 

}
