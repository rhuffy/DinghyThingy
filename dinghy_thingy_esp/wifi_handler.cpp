#include "wifi_handler.h"

char network[] = "6s08"; //"6s08";  //SSID for 6.08 Lab
char password[] = "iesc6s08"; //"iesc6s08"; //Password for 6.08 Lab
char host[] = "608dev.net";

char request_buffer[3000];

uint8_t char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len>buff_size) return false;
  buff[len] = c;
  buff[len+1] = '\0';
  return true;
}

void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial){
  Serial.println("a");
  WiFiClient client; //instantiate a client object
  Serial.println("b");
  if (client.connect(host, 80)) { //try to connect to host on port 80
    Serial.println("c");
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
  Serial.println("enter send info");
  char body[1000];
  // char info2[] = "24,2019-05-09T11:33:24.5,42.361073,-71.092361,-0.198120,0.007080,1.028809";
  Serial.println("b4 sprintf");
  sprintf(body,"categories=int boatnum, datetime time, float lat, float lon, float x_accel, float y_accel, float z_accel&data=%s",info/*"24,2019-05-09T11:33:24.5,42.361073,-71.092361,-0.198120,0.007080,1.028809"*/);
  Serial.println("after sprintf");
  int body_len = strlen(body);
  Serial.println("1");
  sprintf(request_buffer,"POST http://608dev.net/sandbox/sc/mayigrin/final_project/parse_data.py HTTP/1.1\r\n");
  Serial.println("2");
  strcat(request_buffer,"Host: 608dev.net\r\n");
  Serial.println("3");
  strcat(request_buffer,"Content-Type: application/x-www-form-urlencoded\r\n");
  Serial.println("4");
  sprintf(request_buffer+strlen(request_buffer),"Content-Length: %d\r\n", body_len);
  Serial.println("5");
  strcat(request_buffer,"\r\n");
  Serial.println("6");
  strcat(request_buffer,body);
  Serial.println("7");
  strcat(request_buffer,"\r\n");
  Serial.println("8");
  Serial.println(request_buffer);
  do_http_request("608dev.net", request_buffer, response_buffer, 1000, 6000,true);
  Serial.println("9");
  return;
}
