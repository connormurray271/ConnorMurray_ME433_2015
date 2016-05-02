/*
* File:  uart test.c
* Author: Ian Rogers
*
* Created on 12 November 2014, 18:34
*/
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

void Serinit(void)
{
  TRISC = 0xFF;
  RCSTA = 0xB0;
  TXSTA = 0x24;
  SPBRG = 0x12;
}
void putch(char data)
{

  while(!TXIF);
TXREG = data;
}
/*
*
*/
void main() {
  char x;

  Serinit();

  while(1)
  {
  printf("Hello world \n\r");
  }

}
