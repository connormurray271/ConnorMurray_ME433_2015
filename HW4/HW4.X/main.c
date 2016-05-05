#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "spi.h"
#include "i2c.h"
#include <math.h>

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // free up secondary osc pins
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz (divide by 2)
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV (multiply by 24)
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz (divide by 2)
#pragma config UPLLIDIV = DIV_4 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0x0001 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

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

    initSPI1();
    initI2C2();
    i2c_master_setup();
    initExpander();

    //sine wave
    int sine[1000];
    int i;
    for(i = 0; i < 1000; i++){
      sine[i] = 128 + 127*sin(2*3.14*10*i/1000);
    }

    int triangle[1000];
    i = 0;
    for(i = 0; i < 1000; i++){
      triangle[i] = .256*i;
    }

    i = 0;

    while(1) {
      _CP0_SET_COUNT(0);

      if(_CP0_GET_COUNT() > 24000){
        i++;
        setVoltage(0, sine[i]);
        setVoltage(1, triangle[i]);
        _CP0_SET_COUNT(0);
      }

      if(i > 1000){
        i = 0;
      }

      char status = getExpander();          //read the expander
      char g7 = (status & 0x80) >> 7;       //get level of pin g7
      setExpander(0, g7);                   //set pin 0 to level of g7



    }


}
