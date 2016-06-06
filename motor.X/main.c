#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "pic_setup.h"

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


    __builtin_enable_interrupts();

    T2CONbits.TCKPS = 0b111;        //set prescaler to 256
    PR2 = 3749;                     //period = (PR2 + 1) * N * 1/48e6 = 50 Hz
    TMR2 = 0;                       //initial timer count is 0
    T2CONbits.ON = 1;               //turn on Timer2

    RPA0Rbits.RPA0R = 0b0101;       //set OC1 on A0
    OC1CONbits.OCM = 0b110;         //PWM mode without fault pin
    OC1RS = 1875;                   //set duty cycle 50%
    OC1R = 1875;
    OC1CONbits.ON = 1;              //turn on OC1

    TRISAbits.TRISA4 = 0;

    while(1) {
        OC1RS = 1875;
        LATAbits.LATA4 = 0;
    }


}
