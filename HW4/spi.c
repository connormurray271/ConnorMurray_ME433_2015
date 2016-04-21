void initSPI1(){
  RPB8Rbits.RPB8R = 0b0011;    //assign SDO1 to B8
  SDI1Rbits.SDI1R = 0b0000;   //assign SDI1 to A1
  SS1Rbits.SS1R = 0b0000;     //assign SS1 to A0

  SPI1BUF;                   //clear rx buffer by reading from it
  SPI1BRG = 1;              //set clock frequency to 2 MHz
  SPI1STATbits.SPIROV = 0;  //clear overflow bit
  SPI1CONbits.MSTEN = 1;    //master enabled
  SPI1CONbits.MSSEN = 0;    //auto slave select diabled
  SPI1CONbits.ON = 1;       //turn on SPI1

}
