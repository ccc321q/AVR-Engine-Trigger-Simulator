
/*****************************************************************
 * This simulates N-M trigger wheel.
 * You must capture on falling and rising edge for this to work.
 * 
 * Tested on Arduino Duemilanove with ATmega328
 * 
 * Christiaan Aspeling
 * Auckland, New Zealand
 * 16.10.2013
 *****************************************************************/


#define TOTAL_TEETH 30
#define MISSING_TEETH 2

// PB5 is connected to arduinos onboard led and to digital pin 13
#define PORT_CRANK_TRIGGER PORTB
#define PIN_CRANK_TRIGGER 5

// PB4 is connected to arduinos digital pin 2
#define PORT_CAM_TRIGGER PORTD
#define PIN_CAM_TRIGGER 2

/*****************************************************************/


#include <avr/interrupt.h>
#include <avr/io.h>

#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define TRUE 1
#define FALSE 0

#if TOTAL_TEETH < MISSING_TEETH
	#error "Too many teeth missing!"
#endif

volatile uint8_t triggerCount = 1;
volatile uint8_t actualTeeth;
volatile uint8_t camBool = FALSE;

int main(void) {
	DDRB = 0xff;	// all ouput
	DDRC = 0xff;	// all ouput
	DDRD = 0xff;	// all ouput
	
	// setup timer1 (16bit) to interrupt
	
	//---------------------------------------//
	//  Teeth : RPM
	// 1 tooth: 1500
	// 4 teeth: 375
	// 12 teeth: 125
	// 30 teeth: 50
	// 36 teeth: 41.7
	// 60 teeth: 25
	//TCCR1B = (1<<WGM12) | (1<<CS12);	// CTC mode, prescalar 256
	//OCR1A = 1249;						// timer counts
	//TIMSK1 = _BV(OCIE1A);  				// enable output-compare interrupt TIMER0_COMPA
	
	/*
	//  Teeth : RPM
	// 1 tooth: 312
	// 4 teeth: 78.1
	// 12 teeth: 26
	// 30 teeth: 10
	// 36 teeth: 8.7
	// 60 teeth: 5.2
	TCCR1B = (1<<WGM12) | (1<<CS12);	// CTC mode, prescalar 256
	OCR1A = 6000;						// timer counts
	TIMSK1 = _BV(OCIE1A);  				// enable output-compare interrupt TIMER0_COMPA
	*/
	
	// 12 teeth: 7528
	//TCCR1B = (1<<WGM12) | (1<<CS12);	// CTC mode, prescalar 256
	//OCR1A = 82;						// timer counts
	//TIMSK1 = _BV(OCIE1A);  				// enable output-compare interrupt TIMER0_COMPA
	
	// 12 teeth: 2490rpm
	// 30 teeth: 996rpm
	TCCR1B = (1<<WGM12) | (1<<CS12);	// CTC mode, prescalar 256
	OCR1A = 250;						// timer counts
	TIMSK1 = _BV(OCIE1A);  				// enable output-compare interrupt TIMER0_COMPA
	/*
	//---- 30,000rpm with 1 tooth 0 missing ----//
	TCCR1B = (1<<WGM12) | (1<<CS12);	// CTC mode, prescalar 256
	OCR1A = 249;						// timer counts
	TIMSK1 = _BV(OCIE1A);  				// enable output-compare interrupt TIMER0_COMPA
	*/
	/*
	//---- 720,000rpm with 1 tooth 0 missing ----//
	// 12,000rpm with 60 tooth
	TCCR1B = (1<<WGM12) | (1<<CS12);	// CTC mode, prescalar 256
	OCR1A = 9;							// timer counts
	TIMSK1 = _BV(OCIE1A);  				// enable output-compare interrupt TIMER0_COMPA
	
	//---- 1,500,000rpm with 1 tooth ----//
	// 25,000rpm with 60 teeth
	// 41666rpm with 36 teeth
	TCCR1B = (1<<WGM12) | (1<<CS12);	// CTC mode, prescalar 256
	OCR1A = 4;							// timer counts
	TIMSK1 = _BV(OCIE1A);  				// enable output-compare interrupt TIMER0_COMPA
	*/
	
	actualTeeth = TOTAL_TEETH - MISSING_TEETH;
	sei();	// enable interrupts
	while(1){}
	return 0;
}

/** timer1 interrupt **/
ISR(TIMER1_COMPA_vect) {
	if (triggerCount > actualTeeth) {
		// do nothing
	} else {
		BIT_FLIP(PORT_CRANK_TRIGGER, PIN_CRANK_TRIGGER); //toggle crank trigger pin
	}
	
	// every second revolution toggle the cam trigger pin
	if (triggerCount == 2) {
		if (camBool) {
			BIT_FLIP(PORT_CAM_TRIGGER, PIN_CAM_TRIGGER); //toggle cam trigger pin
			camBool = FALSE;
		} else {
			camBool = TRUE;
		}
	}
	
	triggerCount++;
	if (triggerCount > TOTAL_TEETH)
		triggerCount = 1;
}
