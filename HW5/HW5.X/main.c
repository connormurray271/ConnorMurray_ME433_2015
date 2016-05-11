#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "pic_setup.h"
#include "lcd.h"

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

    SPI1_init();
    LCD_init();
    LCD_clearScreen(BLUE);

    while(1) {

      char message[100];
      int n = 1337;
      sprintf(message, "Hello world %d!", 1337);
      drawMessage(28,32,message);
    }

}
