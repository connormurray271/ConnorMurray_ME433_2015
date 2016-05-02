#include "pic_setup.h"
#include "i2c.h"
#include "imu.h"
#include <stdio.h>

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

    // unsigned char data[];

    char r = I2C_read_WHOAMI();
    TRISAbits.TRISA4 = 0;    //make A4 an output
    LATAbits.LATA4 = 0;      //turn on A4 LED

    if(r == 0b01101001){
      LATAbits.LATA4 = 1;      //turn on A4 LED
    }

    while(1) {
      // I2C_read_multiple(ADDRESS, 0x20, data, 14);
    }


}
