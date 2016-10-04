#include "morse.h"

#define TABLE_STR "*ETIANMSURWDKGOHVF*L*PJBXCYZQ"

static const char* const MORSE_TABLE = TABLE_STR;

static const unsigned char TABLE_LENGTH = sizeof(TABLE_STR);

static unsigned char current_index = 0;

void morse_reset() {
	current_index = 0;
}

char morse_retrieve() {
	if (current_index >= TABLE_LENGTH || current_index == 0) {
		return '!';	
	}
	
	return MORSE_TABLE[current_index];
}

void morse_next_signal(DURATION duration) {
	current_index = (current_index << 1) + 1 + (char)duration;
}
