/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Libraries for SD card
#include "FS.h"
#include "SD.h"
#include <SPI.h>


// Define CS pin for the SD card module
#define SD_CS 5

String dataMessage;

void setup() {
  // Start serial communication for debugging purposes
  Serial.begin(115200);

  // Initialize SD card
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }

  // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file = SD.open("/data.txt");
  if(!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/data.txt", "Reading ID, Date, Hour, Temperature \r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();

//  // Enable Timer wake_up
//  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  // Start the DallasTemperature library
//  sensors.begin(); 

//  getReadings();
//  getTimeStamp();
  logSDCard();
  
  // Increment readingID on every new reading
//  readingID++;
  
  // Start deep sleep
//  Serial.println("DONE! Going to sleep now.");
//  esp_deep_sleep_start(); 
}

void loop() {
  // The ESP32 will be in deep sleep
  // it never reaches the loop()
  //logSDCard();
}

//// Function to get temperature
//void getReadings(){
//  sensors.requestTemperatures(); 
//  temperature = sensors.getTempCByIndex(0); // Temperature in Celsius
//  //temperature = sensors.getTempFByIndex(0); // Temperature in Fahrenheit
//  Serial.print("Temperature: ");
//  Serial.println(temperature);
//}

// Function to get date and time from NTPClient
//void getTimeStamp() {
//  while(!timeClient.update()) {
//    timeClient.forceUpdate();
//  }
//  // The formattedDate comes with the following format:
//  // 2018-05-28T16:00:13Z
//  // We need to extract date and time
//  formattedDate = timeClient.getFormattedDate();
//  Serial.println(formattedDate);
//
//  // Extract date
//  int splitT = formattedDate.indexOf("T");
//  dayStamp = formattedDate.substring(0, splitT);
//  Serial.println(dayStamp);
//  // Extract time
//  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
//  Serial.println(timeStamp);
//}

// Write the sensor readings on the SD card
void logSDCard() {
  dataMessage = "hello world!";
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/data.txt", dataMessage.c_str());
}

// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
