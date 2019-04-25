#include "gps.h"

void init_gps(){
  gps.begin(9600,SERIAL_8N1,32,33);
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
        data->hour = (data_array[7]-48)*10+data_array[8]-48-4;
        data->minute = (data_array[9]-48)*10+data_array[10]-48;
        data->second = (data_array[11]-48)*10+data_array[12]-48;
        data->lat_deg = (data_array[20]-48)*10+data_array[21]-48;
        int i=22;
        float j[2];
        get(data_array,i,j);
        data->lat_dm = j[0];
        i = j[1];
        data->lat_dir = data_array[++i];
        i+=2;
        data->lon_deg = (data_array[i]-48)*100+(data_array[++i]-48)*10+(data_array[++i]-48);
        get(data_array,++i,j);
        data->lon_dm = j[0];
        i = j[1];
        data->lon_dir = data_array[++i];
        i+=12;
        data->day = (data_array[i]-48)*10+data_array[++i]-48;
        data->month = (data_array[++i]-48)*10+data_array[++i]-48;
        data->year = (data_array[++i]-48)*10+data_array[++i]-48;
    }
    else valid = 0;
}

void read_gps(GPS_READING_T* data){
 extractGNRMC();
}
