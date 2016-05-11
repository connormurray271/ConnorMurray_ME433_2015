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
    LCD_clearScreen(BLACK);

    while(1) {

      drawCharacter(50,100,0x4d);
      drawCharacter(50,110,0x42);
      drawCharacter(50,120,0x43);
      drawCharacter(50,130,0x44);
      drawCharacter(50,140,0x45);
        

      LCD_drawPixel(50,50,WHITE);
      LCD_drawPixel(51,50,WHITE);
      LCD_drawPixel(52,50,WHITE);
      LCD_drawPixel(53,50,WHITE);
      LCD_drawPixel(54,50,WHITE);
    }

}
