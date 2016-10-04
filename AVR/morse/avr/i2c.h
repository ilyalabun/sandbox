#ifndef __I2C_H
#define __I2C_H

typedef unsigned char I2C_ADDR;

void i2c_send_char(I2C_ADDR slave_addr, char byte);

#endif // __I2C_H
