#include <xc.h>
#include "spi.h"

#define CS LATBbits.LATB7      //chip select pin is B7

void initSPI1(void){
  RPB13Rbits.RPB13R = 0b0011;    //assign SDO1 to B13
  SDI1Rbits.SDI1R = 0b0100;      //assign SDI1 to B8
  TRISBbits.TRISB7 = 0;          //set B7  as output
  CS = 1;                        //turn off communiation

  SPI1CON = 0;              //turn off spi and reset it
  SPI1BUF;                  //clear rx buffer by reading from it
  SPI1BRG = 47999;
  SPI1STATbits.SPIROV = 0;  //clear overflow bit
  SPI1CONbits.CKE = 1;      //data changes when clock goes from hi to low
  SPI1CONbits.MSTEN = 1;    //master enabled
  SPI1CONbits.ON = 1;       //turn on SPI1
}


char SPI1_IO(char write){
  SPI1BUF = write;        //send to buffer
  while(!SPI1STATbits.SPIRBF){
    ;
  }
  return SPI1BUF;                //read buffer to clear
}

void setVoltage(char channel, char voltage){
  CS = 0;             //open communication
  SPI1_IO((channel << 7 | 0b01110000)|(voltage >> 4));
  SPI1_IO(voltage << 4);
  CS = 1;         //close communication
}
