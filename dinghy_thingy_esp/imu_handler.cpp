#include "imu_handler.h"

MPU9255 imu;

void init_imu(){
    //Serial.begin(115200); //for debugging if needed.
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
