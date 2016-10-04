TARGET=morse

CFLAGS=-MMD -O3

ARCH=atmega328p
USER=pi
HOST=192.168.1.33
REMOTE_CMD=ssh ${USER}@${HOST}

DUDE_PROGRAMMER=pi_1
DUDE_ARCH=m328p
DUDE_CFG=/home/pi/avrdude_gpio.conf

objects := $(patsubst %.c,%.o,$(wildcard *.c))

.PHONY: all clean burn burn_eeprom erase

all: ${TARGET}

-include $(objects:.o=.d)

%.o: %.c
	avr-gcc -c ${CFLAGS} -mmcu=${ARCH} -o $@ $<
		
${TARGET}: $(objects)
	avr-gcc -mmcu=${ARCH} -o ${TARGET} $^; \
	avr-objcopy -j .text -j .data -O binary ${TARGET} ${TARGET}.hex; \
	avr-objcopy -j .eeprom -O binary ${TARGET} ${TARGET}.eeprom
	
clean:
	-rm *.o *.d *.hex *.eeprom ${TARGET}
	
burn_flash: ${TARGET}
	scp ${TARGET}.hex ${USER}@${HOST}:/tmp; \
	${REMOTE_CMD} sudo avrdude -C ${DUDE_CFG} -c ${DUDE_PROGRAMMER} -p ${DUDE_ARCH} -U flash:w:/tmp/${TARGET}.hex
	
burn_eeprom: ${TARGET}
	[ -s ${TARGET}.eeprom ] && scp ${TARGET}.eeprom ${USER}@${HOST}:/tmp; \
	[ -s ${TARGET}.eeprom ] && ${REMOTE_CMD} sudo avrdude -C ${DUDE_CFG} -c ${DUDE_PROGRAMMER} -p ${DUDE_ARCH} -U eeprom:w:/tmp/${TARGET}.eeprom

burn_all: burn_flash burn_eeprom
	
erase:
	${REMOTE_CMD} "sudo avrdude -C ${DUDE_CFG} -c ${DUDE_PROGRAMMER} -p ${DUDE_ARCH} -e"
	
	
	

