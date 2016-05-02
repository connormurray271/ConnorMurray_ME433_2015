#include <xc.h>
#include "pic_setup.h"
#include "i2c.h"
#include "imu.h"

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
      I2C_read_multiple(ADDRESS, 0x20, data, length);

      short temp = (data[2] << 8) | data[1];

      short gyro_x = (data[4] << 8) | data[3];
      short gyro_y = (data[6] << 8) | data[5];
      short gyro_z = (data[8] << 8) | data[7];

      short accel_x = (data[10] << 8) | data[9];
      short accel_y = (data[12] << 8) | data[11];
      short accel_z = (data[14] << 8) | data[13];

      // OC1RS = accel_x;
      // OC2RS = accel_y;
    }


}
