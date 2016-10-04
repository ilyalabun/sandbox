ARCH=atmega328p
USER=pi
HOST=192.168.1.33
REMOTE_CMD=ssh ${USER}@${HOST}

DUDE_PROGRAMMER=pi_1
DUDE_ARCH=m328p
DUDE_CFG=/home/pi/avrdude_gpio.conf

compile: hello_world.c
	avr-gcc -mmcu=${ARCH} -o hello_world hello_world.c
	
clean:
	rm hello_world
	
burn: compile
	scp hello_world ${USER}@${HOST}:/tmp; \
	${REMOTE_CMD} sudo avrdude -C ${DUDE_CFG} -c ${DUDE_PROGRAMMER} -p ${DUDE_ARCH} -U flash:w:/tmp/hello_world
	
erase:
	${REMOTE_CMD} "sudo avrdude -C ${DUDE_CFG} -c ${DUDE_PROGRAMMER} -p ${DUDE_ARCH} -e"
	
	
	

