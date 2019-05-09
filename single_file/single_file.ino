#include <Wire.h>
#include <string.h>

#include <SPI.h>
#include <math.h>

#include <mpu9255_esp32.h>

#include <cstdlib>
#include <TFT_eSPI.h>
#include <WiFi.h>

#include "FS.h"
#include "SD.h"

char network[] = "6s08"; //"6s08";  //SSID for 6.08 Lab
char password[] = "iesc6s08"; //"iesc6s08"; //Password for 6.08 Lab
char host[] = "608dev.net";


#define BOAT_NUMBER 1
#define MAX_READINGS 50

#define RECORD_BUTTON_PIN 16
#define UPLOAD_BUTTON_PIN 17

#define LOG_RATE 1000

#define SD_CS 5


typedef enum {
  STATE_ROOT,
  STATE_READY,
  STATE_SENSE,
  STATE_WRITEFLASH,
  STATE_UPLOAD
} STATE_T;

struct GPS_READING_T{
    float latitude; //in degrees
    float longitude; //in degrees
    int year; //year
    int month; //month
    int day; //day of month
    int hour; //hour (24 clock GMT)
    int minute; //minute
    int second; //second
};

struct IMU_READING_T{
  float x_accel;
  float y_accel;
  float z_accel;
  // change this or add more fields for gyroscope
};

typedef struct {
  GPS_READING_T gps;
  IMU_READING_T imu;
  int boat_id;
} SENSOR_READING_T;

STATE_T current_state;

SENSOR_READING_T data_buffer[MAX_READINGS+1] = {0};
int data_buffer_index, time_in_ready;


HardwareSerial gps(2); //instantiate approporiate Serial object for GPS

const int BUFFER_LENGTH = 200;  //size of char array we'll use for
char buffer[BUFFER_LENGTH] = {0}; //dump chars into the

bool valid; //is the data valid
int lat_deg; //degrees portion of lattitude
float lat_dm; //latitude decimal minutes
char lat_dir; //latitude direction
int lon_deg; //longitude in degrees
float lon_dm; //longitude decimal minutes
char lon_dir; //longitude direction



MPU9255 imu;



char request_buffer[1000];
char data_message[1000];

uint8_t char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len>buff_size) return false;
  buff[len] = c;
  buff[len+1] = '\0';
  return true;
}

void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial){
  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);
    client.print(request);
    memset(response, 0, response_size);
    uint32_t count = millis();
    while (client.connected()) {
      client.readBytesUntil('\n',response,response_size);
      if (serial) Serial.println(response);
      if (strcmp(response,"\r")==0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis()-count>response_timeout) break;
    }
    memset(response, 0, response_size);
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response,client.read(),1000);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");
  }else{
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}

void wifi_connect(){
  WiFi.begin(network, password); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//Main function for the file to make Post requests to the server with the information
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@



void send_info(char* info, char* response_buffer){
  char body[5000];
  sprintf(body,"categories=int boatnum, datetime time, float lat, float lon, float x_accel, float y_accel, float z_accel&data=%s",info);
  int body_len = strlen(body);
  sprintf(request_buffer,"POST http://608dev.net/sandbox/sc/mayigrin/final_project/parse_data.py HTTP/1.1\r\n");
   strcat(request_buffer,"Host: 608dev.net\r\n");
   strcat(request_buffer,"Content-Type: application/x-www-form-urlencoded\r\n");
   sprintf(request_buffer+strlen(request_buffer),"Content-Length: %d\r\n", body_len);
   strcat(request_buffer,"\r\n");
   strcat(request_buffer,body);
   strcat(request_buffer,"\r\n");
   do_http_request("608dev.net", request_buffer, response_buffer, 1000, 6000,true);
   return;
}


// int boatnum, datetime time, float lat, float lon, float x_accel, float y_accel, float z_accel
void sd_write(SENSOR_READING_T *data_buffer, int size){
 for(int i = 0; i < size; i++){
   sprintf(data_message, "%d,%04d-%02d-%02dT%02d:%02d:%02d,%f,%f,%f,%f,%f;",
   data_buffer[i].boat_id,
   data_buffer[i].gps.year,
   data_buffer[i].gps.month,
   data_buffer[i].gps.day,
   data_buffer[i].gps.hour,
   data_buffer[i].gps.minute,
   data_buffer[i].gps.second,
   data_buffer[i].gps.latitude,  // write data
   data_buffer[i].gps.longitude,
   data_buffer[i].imu.x_accel,
   data_buffer[i].imu.y_accel,
   data_buffer[i].imu.z_accel);
   Serial.print("Save data: ");
   Serial.println(data_message);
   appendFile(SD, "/data.txt", data_message);
 }
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

// Read data from the SD card (DON'T MODIFY THIS FUNCTION)
void readFile(fs::FS &fs, const char * path, char* output){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void init_sd(){
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
    writeFile(SD, "/data.txt", "");
  }
  else {
    Serial.println("File already exists");
  }
  file.close();
}


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


void init_gps(){
  gps.begin(9600,SERIAL_8N1,32,33);
}


void get_number_gps(char* data_array,int s, float* j){
    float sum1=0,sum2=0;
    int len=0,flag = 0;
    while(data_array[s]!=','){
        if(data_array[s]!='.' && flag==0) sum1 = sum1*10+data_array[s]-48;
        else if(data_array[s]=='.') flag=1;
        else if (data_array[s]!='.' && flag==1){sum2=sum2*10+data_array[s]-48;len++;}
        s++;
    }
    sum1+=sum2*pow(10,-1*len);
    j[0]=sum1;
    j[1]=s;
}

void extract_gps_data(char* data_array,GPS_READING_T* data){
    if(data_array[18]=='A'){
        valid=1;
        //hour needs to -4 ton convert it to local time
        data->hour = (data_array[7]-48)*10+data_array[8]-48-4;
        data->minute = (data_array[9]-48)*10+data_array[10]-48;
        data->second = (data_array[11]-48)*10+data_array[12]-48;
        lat_deg = (data_array[20]-48)*10+data_array[21]-48;
        int i=22;
        float j[2];
        get_number_gps(data_array,i,j);
        lat_dm = j[0];
        i = j[1];
        lat_dir = data_array[++i];
        i+=2;
        lon_deg = (data_array[i]-48)*100+(data_array[++i]-48)*10+(data_array[++i]-48);
        get_number_gps(data_array,++i,j);
        lon_dm = j[0];
        i = j[1];
        lon_dir = data_array[++i];
        i+=12;
        data->day = (data_array[i]-48)*10+data_array[++i]-48;
        data->month = (data_array[++i]-48)*10+data_array[++i]-48;
        data->year = (data_array[++i]-48)*10+data_array[++i]-48;
    }
    else valid = 0;
}

void extractGNRMC_gps(GPS_READING_T* data){
    while (gps.available()) {     // If anything comes in Serial1 (pins 0 & 1)
        gps.readBytesUntil('\n', buffer, BUFFER_LENGTH); // read it and send it out Serial (USB)
        char* info = strstr(buffer,"GNRMC");
        if (info!=NULL){
            //Serial.println(buffer); for debugging;
            extract_gps_data(buffer,data);
        }
    }
}

GPS_READING_T read_gps(){
    GPS_READING_T data;
    extractGNRMC_gps(&data);
    //convert latitude and longitude into degrees
    data.latitude = lat_deg+lat_dm/60;
    if(lat_dir != 'N')data.latitude*=-1;
    data.longitude = lon_deg+lon_dm/60;
    if(lon_dir != 'E')data.longitude*=-1;

    return data;
}


/**
 * Calls the correct update function based on current state.
 * Should be called once every cycle.
 */
void advance_state(){
  switch(current_state){
    case STATE_ROOT:
      update_state_root();
      break;
    case STATE_READY:
      update_state_ready();
      break;
    case STATE_SENSE:
      update_state_sense();
      break;
    case STATE_WRITEFLASH:
      update_state_writeflash();
      break;
    case STATE_UPLOAD:
      update_state_upload();
      break;
  }
}

/**
 * Calls the correct enter_state function for the desired new_state.
 * Re-assignes current_state to the new_state.
 * @param new_state the desired state to enter.
 */
void set_state(STATE_T new_state){
  switch(new_state){
    case STATE_ROOT:
      enter_state_root();
      break;
    case STATE_READY:
      enter_state_ready();
      break;
    case STATE_SENSE:
      enter_state_sense();
      break;
    case STATE_WRITEFLASH:
      enter_state_writeflash();
      break;
    case STATE_UPLOAD:
      enter_state_upload();
      break;
  }

  current_state = new_state;
}


/**
 * Initializes FSM. Should be called once in setup.
 */
void init_state(){
  pinMode(RECORD_BUTTON_PIN, INPUT_PULLUP);
  pinMode(UPLOAD_BUTTON_PIN, INPUT_PULLUP);
  data_buffer_index = 0;
  set_state(STATE_ROOT);
}



// The following functions should not be called directly.
// Use set_state() and update_state() instead.


// enter_state_XXX() is called once when the state is entered.
// update_state_XXX() is called repeatedly while in the associated state.

void enter_state_root(){
  Serial.println("[ROOT]");
}

void update_state_root(){


  if(!digitalRead(RECORD_BUTTON_PIN)){
    set_state(STATE_SENSE);
  }
}


void enter_state_ready(){
    Serial.println("[READY]");
    time_in_ready = millis();
}

void update_state_ready(){

  if(!digitalRead(RECORD_BUTTON_PIN)){
    set_state(STATE_ROOT);
  }
  else if(!digitalRead(UPLOAD_BUTTON_PIN)){
    set_state(STATE_UPLOAD);
  }

  else if(data_buffer_index >= MAX_READINGS-1){
    set_state(STATE_WRITEFLASH);
  }

  else if(millis() - time_in_ready > LOG_RATE){
    set_state(STATE_SENSE);
  }
}


void enter_state_sense(){
  Serial.println("[SENSE]");
}

void update_state_sense(){
  // get sensor readings

  SENSOR_READING_T current_reading = {
    .gps = read_gps(),
    .imu = read_imu(),
    .boat_id = BOAT_NUMBER
  };

  // add data to buffer
  Serial.print("Save data at: ");
  Serial.println(data_buffer_index);
  data_buffer[data_buffer_index++] = current_reading;

  set_state(STATE_READY);
}


void enter_state_writeflash(){
  Serial.println("[WRITEFLASH]");
}

void update_state_writeflash(){


  data_buffer_index = 0;
  sd_write(data_buffer, MAX_READINGS);

  set_state(STATE_READY);
}


void enter_state_upload(){
  Serial.println("[UPLOAD]");
}

void update_state_upload(){

  // char dat[] = "22, 2019-04-24T13:29:13.5, 42.357, -71.091, .01, .01, 59.04\n227,  2019-02-23T13:32:16.5, 42.355, -71.094, .1, .1, 589.09\n22, 2019-04-24T13:32:15.5, 42.356, -71.094, .13, .13, 59.04\n22, 2019-04-24T13:35:16.8, 42.353, -71.100, .3, .3, 59.04";
  wifi_connect();
  unsigned char dat[5000];
  readFile(SD, "/data.txt", dat);
  Serial.println(dat);
  //readFile()
  char response[5000];
  send_info(dat,response);
  if(!strcmp(response,"1")){
    set_state(STATE_ROOT);
  }
  else{
    Serial.println("Error uploading");
  }
}


void setup(){
  init_sd();
  init_state();
  init_gps();
  init_imu();
}

void loop(){
  advance_state();
}
