#ifndef IMU_H
#define IMU_H

void initIMU(void);
void I2C_read_multiple(char address, char register_, unsigned char* data, char length);
char I2C_read_WHOAMI(void);

#endif
