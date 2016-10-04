#ifndef __MORSE_H
#define __MORSE_H

typedef enum {
	SHORT = 0,
	LONG  = 1
} DURATION;

/*
 * Reset current state. Should be called after morse_retrieve() 
 */
void morse_reset();

/*
 * Retrieve morse character
 */
char morse_retrieve();

/*
 * Should be called when new signal recieved. 
 * Changes internal state.
 */
void morse_next_signal(DURATION duration);

#endif // __MORSE_H
