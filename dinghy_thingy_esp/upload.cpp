#include <cstdlib>
#include <string.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include "upload.h"


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//Helper functions
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@


char request_buffer[1000];
char response_buffer[1000];

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

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//Main function for the file to make Post requests to the server with the information
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@



void send_info(char* info){
  char body[1000];
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
}
