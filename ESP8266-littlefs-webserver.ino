#include <LiquidCrystal_I2C.h>

#include <Wire.h>

#include "LittleFS.h"

// set the LCD number of columns and rows

int lcdColumns = 16;

int lcdRows = 2;

// set LCD address, number of columns and rows

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 
/
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <FS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

// Replace with your network credentials
const char* ssid = "xxxxxx";
const char* password = "xxxxxx3";

// Set LED GPIO
const int ledPin = 4;
// Stores LED state
String ledState;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String getTemperature() {
  float temperature = bme.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float temperature = 1.8 * bme.readTemperature() + 32;
  Serial.println(temperature);
  return String(temperature);
}
  
String getHumidity() {
  float humidity = bme.readHumidity();
  Serial.println(humidity);
  return String(humidity);
}

String getPressure() {
  float pressure = bme.readPressure()/ 100.0F;
  Serial.println(pressure);
  return String(pressure);
}

// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if(digitalRead(ledPin)){
      ledState = "ON";
    }
    else{
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  else if (var == "TEMPERATURE"){
    return getTemperature();
  }
  else if (var == "HUMIDITY"){
    return getHumidity();
  }
  else if (var == "PRESSURE"){
    return getPressure();
  }  
}
 
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // Initialize the sensor
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // Initialize LittleFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/newindex.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/esp8266css.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/esp8266css.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, HIGH);    
    request->send(LittleFS, "/newindex.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, LOW);    
    request->send(LittleFS, "/newindex.html", String(), false, processor);
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getTemperature().c_str());
  });
  
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getHumidity().c_str());
  });
  
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getPressure().c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){
  
}
//function prototypes

void readData();

void writeData(String data);

void deleteData();

void setup() {

  //Start the serial monitor

  Serial.begin(115200);

  // initialize LCD

  lcd.init();

  // turn on LCD backlight                      

  lcd.backlight();

   // set cursor to first column, first row

  lcd.setCursor(0, 0);

  lcd.print("Little FS Demo");

  delay(1000);

  //Start LittleFS

  if(!LittleFS.begin()){

    Serial.println("An Error has occurred while mounting LittleFS");

    //Print the error on display

    lcd.clear();

    lcd.print("Mounting Error");

    delay(1000);

    return;

  }

  //Read the saved data

  readData(); 

}

void loop() {

    //Take input from user on serial monitor

    if(Serial.available())

    {

         String data = Serial.readString();

         Serial.println(data);

         if(data == "D") // To delete the file 

         {

            deleteData();

            Serial.println("File deleted!");

            return; 

         }

         else if(data == "R") // To read the file

         {

            readData();

            return;

         }        

         Serial.println("Writing Data...");

         writeData(data);

         Serial.println("done Writing Data!");

    }

}

void readData()

{

   //Open the file

   File file = LittleFS.open("/SavedFile.txt", "r");

  //Check if the file exists

   if(!file){

   //Read the file data and display it on LCD

    Serial.println("No Saved Data!");

    lcd.clear();

    lcd.print("No Saved Data!");   

    return;

   }

      lcd.clear();

      lcd.print("Saved Data :");

      // set cursor to first column, second row

      lcd.setCursor(0,1);

      //Display on the LCD

      while(file.available()){

      lcd.write(file.read());

      }   

      //reset cursor poisition

      lcd.setCursor(0,0);

   //Close the file

    file.close();

}

void writeData(String data)

{

  //Open the file 

  File file = LittleFS.open("/SavedFile.txt", "w");

  //Write to the file

  file.print(data);

  //Close the file

  file.close();

  delay(1); 

  Serial.println("Write successful");

  lcd.clear();

  lcd.print("Data Saved :"); 

  // set cursor to first column, second row

  lcd.setCursor(0,1);

  // print the data on the LCD

  lcd.print(data);

  // reset cursor position

  lcd.setCursor(0,0);

}

void deleteData()

{

   //Remove the file

   LittleFS.remove("/SavedFile.txt"); 

   lcd.clear();

   lcd.print("Data Deleted"); 

}
