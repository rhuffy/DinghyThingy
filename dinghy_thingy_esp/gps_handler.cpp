#include "gps_handler.h"

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

  data->valid = valid;
}

void extractGNRMC_gps(GPS_READING_T* data){
  while (gps.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    gps.readBytesUntil('\n', buffer, BUFFER_LENGTH); // read it and send it out Serial (USB)
    char* info = strstr(buffer,"GNRMC");
    Serial.println(info);
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
