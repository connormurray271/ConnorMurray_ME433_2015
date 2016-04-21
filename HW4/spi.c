void initSPI1(){
  RPB8Rbits.RPB8R = 0b0011;    //assign SDO1 to B8
  SDI1Rbits.SDI1R = 0b0000;   //assign SDI1 to A1
  SS1Rbits.SS1R = 0b0000;     //assign SS1 to A0

  
}
