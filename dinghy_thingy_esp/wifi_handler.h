#ifndef __WIFI_HANDLER_H
#define __WIFI_HANDLER_H
#include <Arduino.h>

#include <Wire.h>
#include <SPI.h>
#include <cstdlib>
#include <string.h>
#include <WiFi.h>
// #include <WiFiClientSecure.h>

uint8_t char_append(char* buff, char c, uint16_t buff_size);
void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial);
void wifi_connect();
void send_info(char* info, char* response_buffer);

#endif
