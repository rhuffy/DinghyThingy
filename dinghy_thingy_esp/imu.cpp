#include "gps.h"

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

IMU_READING_T read_imu(IMU_READING_T* data){
    imu.readAccelData(imu.accelCount);//read imu
    //the unit is in g.
    x_accel = imu.accelCount[0]*imu.aRes;
    y_accel = imu.accelCount[1]*imu.aRes;
    //there might be a offset for z; not sure if it's just my device
    z_accel =imu.accelCount[2]*imu.aRes;
    char output[40];
    //sprintf(output,"%4.2f,%4.2f,%4.2f",x,y,z); //render numbers with %4.2 float formatting
    //Serial.println(output); //print to serial for plotting
    //Serial.println(' ');
}
