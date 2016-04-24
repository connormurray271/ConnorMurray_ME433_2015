#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be callled in the correct order as per the I2C protocol
// Change I2C1 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k

void i2c_master_setup(void) {
  I2C2BRG = 37;            // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2
                                    // look up PGD for your PIC32
  I2C2CONbits.ON = 1;               // turn on the I2C2 module
}

// Start a transmission on the I2C bus
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {
    I2C2CONbits.RSEN = 1;           // send a restart
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
  I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
  while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
  if(I2C2STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
    // ("I2C2 Master: failed to receive ACK\r\n");
  }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    while(!I2C2STATbits.RBF) { ; }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
  I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
  while(I2C2CONbits.PEN) { ; }        // wait for STOP to complete
}

// Set up the pins for I2C2
void initI2C2(void){
  ANSELBbits.ANSB2 = 0;         // turn off analog on B2
  ANSELBbits.ANSB3 = 0;         // turn off analog on B3
}

// Set up the pins on the expander
void initExpander(void){
  i2c_master_start();
  i2c_master_send(0x20<<1|0);    // send slave address
  i2c_master_send(0x00);        // IODIR register
  i2c_master_send(0xF0);        // GP0-3 outputs, GP4-7 inputs
  i2c_master_stop();
}

// Read pin levels on expander
char getExpander(void){
  i2c_master_start();
  i2c_master_send(0x20 << 1|0);         // slave address with writing
  i2c_master_send(0x09);                // GPIO register
  i2c_master_restart();
  i2c_master_send(0x20 << 1|1);          // slave address with reading
  char r = i2c_master_recv();   // save value
  i2c_master_ack(1);                    // acknowledge that data is received
  i2c_master_stop();
  return r;
}

// Set a pin on the expander to a level
void setExpander(char pin, char level){
  i2c_master_start();
  i2c_master_send(0x20 << 1|0);         // send slave address
  i2c_master_send(0x0A);                // OLAT register
  char status = getExpander();  // get current values
  char change = level << pin;   // set level on pin position
  i2c_master_send(status | change);     // send changes
  i2c_master_stop();
}
