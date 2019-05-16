#include "sd_handler.h"

char data_message[MAX_READINGS * 100];
// int boatnum, datetime time, float lat, float lon, float x_accel, float y_accel, float z_accel
void sd_write(SENSOR_READING_T *data_buffer, int size){
  for(int i = 0; i < size; i++){
    sprintf(data_message, "%d,%d-%02d-%02dT%02d:%02d:%02d,%f,%f,%f,%f,%f\n",
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

void clear_data_file() {
  //writeFile(SD, "/data.txt", "");
}

bool read_and_upload(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\n", path);

  char output[2000];
  char response[100];

  String buffer;

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return 0;
  }
  Serial.println("Read from file: ");
  while(file.available()){
    strcpy(output, "");
    for(int i = 0; i < 20; i++){
      if(file.available()){
        buffer = file.readStringUntil('\n');
        strcat(output, buffer.c_str());
        strcat(output, "\n");
      }
    }
    send_info(output, response);
    if(strcmp(response, "1")){
      Serial.println("Error uploading");
      return 0;
    }
  }
  file.close();
  return 1;
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

    String buffer;

    //unsigned char* temp = (unsigned char*) output;

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }
    strcpy(output, "");
    Serial.print("Read from file: ");
    while(file.available()){
        //file.read(temp, 4500);
        buffer = file.readStringUntil('\n');
        strcat(output, buffer.c_str());
        // if(file.available()){
        strcat(output, "\n");
        // }

    }
    file.close();

    // Serial.println(buffer);

    // strcpy(output, buffer.c_str());
    // Serial.println(output);
    //output[strlen(output)-1] = 0; // kills trailing newline
    //output[strlen(output)-1] = 0;
    // Serial.println("after strcpy");
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
