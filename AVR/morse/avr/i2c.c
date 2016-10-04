#include <avr/io.h>

#include "i2c.h"

typedef unsigned char BYTE;

static inline void start_transmission(void);
static inline void stop_transmission(void);
static inline void send_byte_to_line(BYTE byte);
static inline void send_addr(I2C_ADDR slave_addr);


void i2c_send_char(I2C_ADDR slave_addr, char byte) {
	start_transmission();
	send_addr(slave_addr);
	send_byte_to_line(byte);
	stop_transmission();
}


static inline
void wait_TWINT() {
	while (!(TWCR & (1 << TWINT)));
}

static inline
void start_transmission() {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	wait_TWINT();
}

static inline
void stop_transmission() {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

static inline
void send_byte_to_line(BYTE byte) {
	TWDR = byte;
	TWCR = (1 << TWINT) | (1 << TWEN);
	wait_TWINT();
}

static inline
void send_addr(I2C_ADDR slave_addr) {
	send_byte_to_line(slave_addr << 1);
}

