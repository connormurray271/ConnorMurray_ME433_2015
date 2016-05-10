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

    LCD_init();
    SPI1_init();
    LCD_clearScreen(WHITE);

    while(1) {

      // drawCharacter(0,0,0x30);

      LCD_drawPixel(50,50,BLACK);
      LCD_drawPixel(51,50,BLACK);
      LCD_drawPixel(52,50,BLACK);
      LCD_drawPixel(53,50,BLACK);
      LCD_drawPixel(54,50,BLACK);
    }

}
