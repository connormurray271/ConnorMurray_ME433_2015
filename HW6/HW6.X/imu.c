#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "i2c.h"

#define ADDRESS 0b1101011   //IMU address

void initIMU(void){
  i2c_master_start();
  i2c_master_send(ADDRESS << 1|0);      //IMU address
  i2c_master_send(0x10);                 //CTRL1_XL
  i2c_master_send(0b10000000);            //1.66 kHz, 2g, 400Hz
  i2c_master_stop();

  i2c_master_start();
  i2c_master_send(ADDRESS << 1|0);     //IMU address
  i2c_master_send(0x11);                //CTRL2_G
  i2c_master_send(0b1000001);            //1.66 kHz, 245 dps, gyroscope enabled
  i2c_master_stop();

  i2c_master_start();
  i2c_master_send(ADDRESS << 1|0);     //IMU address
  i2c_master_send(0x12);                //CTRL3_C
  i2c_master_send(0b00000100);           //IF_INC enabled
  i2c_master_stop();
}

void I2C_read_multiple(char address, char register_, unsigned char* data, char length){
  i2c_master_start();
  i2c_master_send(address << 1|0);
  i2c_master_send(register_);
  i2c_master_restart();
  i2c_master_send(address << 1|1);

  int i;
  for(i = 0; i < length-1; i++){
    data[i] = i2c_master_recv();
    i2c_master_ack(0);
  }

  data[length-1] = i2c_master_recv();
  i2c_master_ack(1);
  i2c_master_stop();
}

char I2C_read_WHOAMI(void){
  i2c_master_start();
  i2c_master_send(ADDRESS << 1|0);    //IMU address
  i2c_master_send(0x0F);              //WHO_AM_I
  i2c_master_restart();
  i2c_master_send(ADDRESS << 1|1);
  char r = i2c_master_recv();
  i2c_master_ack(1);
  i2c_master_stop();

  return r;
}
