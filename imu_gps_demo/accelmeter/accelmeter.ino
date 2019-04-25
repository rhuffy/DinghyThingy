#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.
#include <mpu9255_esp32.h>

TFT_eSPI tft = TFT_eSPI(); 
 
#define BACKGROUND TFT_GREEN
#define BALL_COLOR TFT_BLUE

const uint8_t BUTTON_PIN = 16; //CHANGE YOUR WIRING TO PIN 16!!! (FROM 19)


MPU9255 imu; //imu object called, appropriately, imu

void setup() {
  Serial.begin(115200); //for debugging if needed.
  pinMode(BUTTON_PIN,INPUT_PULLUP);
  //tft.init();
  //tft.setRotation(2);
  //tft.setTextSize(1);
  //tft.fillScreen(BACKGROUND);
  if (imu.setupIMU(1)){
    Serial.println("IMU Connected!");
  }else{
    Serial.println("IMU Not Connected :/");
    Serial.println("Restarting");
    ESP.restart(); // restart the ESP (proper way)
  }
 
}

void loop() {
  // put your main code here, to run repeatedly:
  imu.readAccelData(imu.accelCount);//read imu
  float x = imu.accelCount[0]*imu.aRes;
  float y = imu.accelCount[1]*imu.aRes;
  float z =imu.accelCount[2]*imu.aRes;
  char output[40];
  sprintf(output,"%4.2f,%4.2f,%4.2f",x,y,z); //render numbers with %4.2 float formatting
  Serial.println(output); //print to serial for plotting
  Serial.println(' ');
}
