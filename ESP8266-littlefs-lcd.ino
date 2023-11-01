#include <LiquidCrystal_I2C.h>

#include <Wire.h>

#include "LittleFS.h"

// set the LCD number of columns and rows

int lcdColumns = 16;

int lcdRows = 2;

// set LCD address, number of columns and rows

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

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
