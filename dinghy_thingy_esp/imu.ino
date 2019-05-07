//TODO method headers and constant definitions here.
#include <string.h>  //used for some string handling and processing.
#include <mpu9255_esp32.h>
//#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display

//TFT_eSPI tft = TFT_eSPI();
MPU9255 imu;

struct IMU_READING_T{
  float x_accel;
  float y_accel;
  float z_accel;
  // change this or add more fields for gyroscope
};


// void init_imu();
// //IMU_READING_T data;
// IMU_READING_T read_imu();

void init_imu(){
    Serial.begin(115200); //for debugging if needed.
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

IMU_READING_T read_imu(){
    imu.readAccelData(imu.accelCount);//read imu
    //the unit is in g.

    IMU_READING_T current_reading = {
      .x_accel = imu.accelCount[0]*imu.aRes,
      .y_accel = imu.accelCount[1]*imu.aRes,
      //there might be a offset for z; not sure if it's just my device
      .z_accel =imu.accelCount[2]*imu.aRes
    };

    return current_reading;
}
