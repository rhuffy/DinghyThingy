#include<Wire.h>
#include<string.h>
#include <SPI.h>
#include <math.h>

HardwareSerial gps(2); //instantiate approporiate Serial object for GPS

uint32_t timer; //used for loop timing
const uint16_t LOOP_PERIOD = 50; //period of system

const int BUFFER_LENGTH = 200;  //size of char array we'll use for
char buffer[BUFFER_LENGTH] = {0}; //dump chars into the 

int lat_deg; //degrees portion of lattitude
float lat_dm; //latitude decimal minutes
char lat_dir; //latitude direction
int lon_deg; //longitude in degrees
float lon_dm; //longitude decimal minutes
char lon_dir; //longitude direction
int year; //year
int month; //month 
int day; //day of month
int hour; //hour (24 clock GMT)
int minute; //minute
int second; //second
bool valid; //is the data valid

void setup() {
  Serial.begin(115200);
  gps.begin(9600,SERIAL_8N1,32,33);
  timer=millis();
}

void get(char* data_array,int s, float* j){
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


void extract(char* data_array){
  if(data_array[18]=='A'){
    valid=1;
    hour = (data_array[7]-48)*10+data_array[8]-48;
    minute = (data_array[9]-48)*10+data_array[10]-48;
    second = (data_array[11]-48)*10+data_array[12]-48;
    lat_deg = (data_array[20]-48)*10+data_array[21]-48;
    int i=22;
    float j[2];
    get(data_array,i,j);
    lat_dm = j[0]; 
    i = j[1];
    lat_dir = data_array[++i];
    i+=2;
    lon_deg = (data_array[i]-48)*100+(data_array[++i]-48)*10+(data_array[++i]-48);
    get(data_array,++i,j);
    lon_dm = j[0];
    i = j[1];
    lon_dir = data_array[++i];
    i+=12;
   day = (data_array[i]-48)*10+data_array[++i]-48;
   month = (data_array[++i]-48)*10+data_array[++i]-48;
   year = (data_array[++i]-48)*10+data_array[++i]-48;
}
  else valid = 0;
}
void extractGNRMC(){
  while (gps.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    gps.readBytesUntil('\n', buffer, BUFFER_LENGTH); // read it and send it out Serial (USB)
    char* info = strstr(buffer,"GNRMC");
    if (info!=NULL){
      Serial.println(buffer);
      extract(buffer);
    }
  }
}
void loop() {
  extractGNRMC();
  char info[200]={0};
  
  if (valid){
    sprintf(info, "Date: %02d/%02d/%02d\nTime: %02d:%02d:%02d\nLat: %d %2.4f'%c\nLon:%d %2.4f'%c",day,month,year,hour,minute,second,lat_deg,lat_dm,lat_dir,lon_deg,lon_dm,lon_dir);
  }else{
    sprintf(info, "No valid Fix       \n                 \n                  \n                   \n               \n            ");
  }
  
  Serial.println(info); //for debugging
  while(millis()-timer<LOOP_PERIOD);//pause
  timer = millis();
}
