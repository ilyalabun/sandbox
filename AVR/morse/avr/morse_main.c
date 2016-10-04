#define F_CPU 1000000UL  // 1 MHz

#include <avr/io.h>
#include <avr/interrupt.h>

#include "morse.h"
#include "i2c.h"

//////////////////////////////////////////////////////////////////////////
//                          CONSTANTS                                   //
//////////////////////////////////////////////////////////////////////////

// Morse button defines
#define BTN_PORT    PORTC
#define BTN_PIN     PINC	
#define BTN_DDR     DDRC
#define BTN_PIN_N   0
#define BTN_MASK    (1 << BTN_PIN_N)

// Debounce defines
#define DEBOUNCE_THRESHOLD 50

//Timer defines
#define PRESCALER               1000
#define SECS_TO_TICKS(SECS)     ((F_CPU / PRESCALER) * SECS)        

#define LONG_PUSH_SECS          0.3
#define LONG_PUSH_TICKS         SECS_TO_TICKS(LONG_PUSH_SECS)

#define END_OF_SYMBOL_SECS      1
#define END_OF_SYMBOL_TICKS     SECS_TO_TICKS(END_OF_SYMBOL_SECS)

#define END_OF_WORD_SECS        1
#define END_OF_WORD_TICKS       SECS_TO_TICKS(END_OF_WORD_SECS)

//LEDs defines
#define LED_PORT        PORTD
#define LED_DDR         DDRD

#define SHORT_LED_PIN   2
#define LONG_LED_PIN    4
#define EOS_LED_PIN     5
#define EOW_LED_PIN     7

#define SHORT_LED_MASK  (1 << SHORT_LED_PIN)
#define LONG_LED_MASK   (1 << LONG_LED_PIN)
#define EOS_LED_MASK   (1 << EOS_LED_PIN)
#define EOW_LED_MASK   (1 << EOW_LED_PIN)

//I2C defines
#define RASP_ADDR 0x38


//////////////////////////////////////////////////////////////////////////
//                          GLOBAL STATE                                //
//////////////////////////////////////////////////////////////////////////

typedef enum {
	BUTTON,
	END_OF_SYMBOL,
	END_OF_WORD
} TIMER_MODE;

static volatile DURATION       g_button_state;
static volatile TIMER_MODE     g_timer_mode;


//////////////////////////////////////////////////////////////////////////

void init_btn_port() {
	// BTN_PIN_Nth pin is input, so set corresponding DDR bit to 0
	BTN_DDR &= ~BTN_MASK;
	// Turn on pull up resistor
	PORTC   |= BTN_MASK;
}

void init_leds() {
	LED_DDR |= 
		SHORT_LED_MASK 
	  | LONG_LED_MASK
	  | EOS_LED_MASK
	  | EOW_LED_MASK;	
}

void init_timer() {
	cli();
	// Enable timer compare interrupt (on register OCR1A)
    TIMSK1 |= 1<<OCIE1A;
    // Put timer in Clear Timer on Compare match mode (CTC)
    TCCR1B |= 1<<WGM12;
    sei();
}

static inline
void turn_off_all_leds() {
	LED_PORT &= ~(
		SHORT_LED_MASK 
	  | LONG_LED_MASK
	  | EOS_LED_MASK
	  | EOW_LED_MASK
	);
}

static inline
void turn_off_led(unsigned char led_mask) {
	LED_PORT &= ~led_mask;
}

static inline
void turn_on_led(unsigned char led_mask) {
	LED_PORT |= led_mask;
}

static inline
unsigned char read_btn() {
	unsigned int low_lev_cnt  = 0;
	unsigned int high_lev_cnt = 0;
	while (1) {
		unsigned char val = BTN_PIN & BTN_MASK;
		if (val) { //not pressed
			high_lev_cnt++;
			if (high_lev_cnt >= DEBOUNCE_THRESHOLD) {
				return 1;
			}
		} else {  //pressed
			low_lev_cnt++;
			if (low_lev_cnt >= DEBOUNCE_THRESHOLD) {
				return 0;
			}
		}
	}
}

static inline
void start_timer(TIMER_MODE timer_mode) {
	cli();
	g_timer_mode = timer_mode;
	switch (timer_mode) {
	case BUTTON:
		OCR1A = LONG_PUSH_TICKS;
		break;
	case END_OF_SYMBOL:
		OCR1A = END_OF_SYMBOL_TICKS;
		break;
	case END_OF_WORD:
		OCR1A = END_OF_WORD_TICKS;
		break;
	default:
		sei();
		return;	
	}
	
	// Turn on prescaler / 1024
	// This action launches timer
	TCCR1B &= ~(1 << CS11);
	TCCR1B |= (1 << CS12) | (1 << CS10);
	sei();
}

static inline 
stop_timer() {
	// set CS12, CS11, CS10 to 0
	TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
}

static inline
void reset_timer() {
	cli();
	TCNT1 = 0;
	sei();
}

static inline
void stop_and_reset_timer() {
	stop_timer();
	reset_timer();
}

void main(void) {
	init_btn_port();
	init_leds();
	init_timer();		
	while (1) {
		if (!read_btn()) {   // btn pressed
			stop_and_reset_timer();
			start_timer(BUTTON);
			turn_off_all_leds();
			turn_on_led(SHORT_LED_MASK);
			g_button_state = SHORT;
			//wait while button is pushed
			while (!read_btn());
			//Here botton is released
			
			morse_next_signal(g_button_state);
			stop_and_reset_timer();
			start_timer(END_OF_SYMBOL);			
		}		
	}
}

//Timer Interrupt Service Routine
ISR(TIMER1_COMPA_vect)
{
	switch (g_timer_mode) {
	case BUTTON:
		g_button_state = LONG;
		turn_off_all_leds();
		turn_on_led(LONG_LED_MASK);	
		break;
	case END_OF_SYMBOL:		
	    {
		char to_send = morse_retrieve();
		morse_reset();
		i2c_send_char(RASP_ADDR, to_send);
		}
		
		turn_on_led(EOS_LED_MASK);
		stop_and_reset_timer();
		start_timer(END_OF_WORD);
		break;
	case END_OF_WORD:
		i2c_send_char(RASP_ADDR, '\n');
		turn_off_led(EOS_LED_MASK);
		turn_on_led(EOW_LED_MASK);
		stop_and_reset_timer();
	default:
		; // Do nothing, unknown mode
	}
}
