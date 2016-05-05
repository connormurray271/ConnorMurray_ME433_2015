#include <xc.h>
#include "pic_setup.h"
#include "i2c.h"
#include "imu.h"
#include <math.h>

#define ADDRESS 0b1101011   //IMU address

int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    initI2C2();
    i2c_master_setup();
    initIMU();

    __builtin_enable_interrupts();
    char length = 14;
    unsigned char data[14];

    TRISAbits.TRISA4 = 0;    //make A4 an output
    LATAbits.LATA4 = 0;      //turn on A4 LED

    //Timer2
    T2CONbits.TCKPS = 0b111;        //set prescaler to 256
    PR2 = 3749;                     //period = (PR2 + 1) * N * 1/48e6 = 50 Hz
    TMR2 = 0;                       //initial timer count is 0
    T2CONbits.ON = 1;               //turn on Timer2

    //OC1
    RPA0Rbits.RPA0R = 0b0101;       //set OC1 on A0
    OC1CONbits.OCM = 0b110;         //PWM mode without fault pin
    OC1RS = 1875;                   //set duty cycle 50%
    OC1R = 1875;
    OC1CONbits.ON = 1;              //turn on OC1


    //OC2
    RPA1Rbits.RPA1R = 0b0101;       //set OC2 on A1
    OC2CONbits.OCM = 0b110;         //PWM mode without fault pin
    OC2RS = 1875;                   //set duty cycle 50%
    OC2R = 1875;
    OC2CONbits.ON = 1;              //turn on OC2

    while(1) {
      // char r = I2C_read_WHOAMI();

      char check[1];

      I2C_read_multiple(ADDRESS, 0x0F, check, 1);

      char r = check[0];

      if(r == 0b01101001){
        LATAbits.LATA4 = 1;      //turn on A4 LED
      }else{
        LATAbits.LATA4 = 0;
      }


      I2C_read_multiple(ADDRESS, 0x20, data, length);

      short temp = (data[1] << 8) | data[0];

      short gyro_x = (data[3] << 8) | data[2];
      short gyro_y = (data[5] << 8) | data[4];
      short gyro_z = (data[7] << 8) | data[6];

      short accel_x = (data[9] << 8) | data[8];
      short accel_y = (data[11] << 8) | data[10];
      short accel_z = (data[13] << 8) | data[12];

      OC1RS = (int)((((float)accel_x*2 + 32767.0)/65535.0)*(3749));
      OC2RS = (int)((((float)accel_y*2 + 32767.0)/65535.0)*(3749));
    }


}
