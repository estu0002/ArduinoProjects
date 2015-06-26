/*
 * clapper.cpp
 *
 * Created: 5/15/2015 8:26:38 PM
 *  Author: Jesse Estum
 * 
 * Equipment
 * ---------
 * Sound sensor: http://www.gearbest.com/development-boards/pp_156160.html
 * 10A relay:    http://www.gearbest.com/development-boards/pp_116113.html
 * Arduino Nano
 *
 *
 * Rough Schematic
 * ---------------
 *
 * sound sensor          arduino nano         10a relay
 * VCC ----------------- +5V ---------------- +
 * GND ----------------- GND ---------------- -
 *                       D5  ----------------------------- R1/220ohm ------ Green LED ------ GND
 * OUT ----------------- D6
 *                       D7 ----------------- S ---------- R2/220ohm ------ Red LED   ------ GND
 */ 


#include <Arduino.h>
#include <pins_arduino.h>

#include <avr/interrupt.h>

// if defined, send debugging information over serial
#undef DEBUG_ENABLED

// function prototypes
void setup();
void loop();
void control_load(bool state);
bool get_next_load_state();

// set up for a ring buffer to capture ms timestamp for sound sensor pin change
#define SOUND_BUFFER_LENGTH 20
volatile unsigned long sound_sensed_ms_buf[SOUND_BUFFER_LENGTH];
volatile int sound_sensed_ms_buf_pos = 0;

// our tolerance limits - if we get two samples within these limits then we know it's time to change state
#define MAX_DIFFERENCE_TOLERANCE_MS 1000
#define MIN_DIFFERENCE_TOLERANCE_MS 400

// a release flag for our cyclic executive indicating it's time to examine the ring buffer
volatile bool release_calculation = false;

// state variable to track load
bool is_load_currently_active;


void setup() {

#ifdef DEBUG_ENABLED	
	Serial.begin(9600);
#endif	

	// for good measure, zero out our buffer
	memset((void *)&sound_sensed_ms_buf, 0, sizeof(sound_sensed_ms_buf));

	DDRD |= (1 << PIND7); // set D7 as output
	DDRD &= ~(1 << PIND6); // set D6 as input
	DDRD |= (1 << PIND5); // set D5 as output

	// PCICR - Pin Change Interrupt Control Register
	// -
	// -
	// -
	// -
	// -
	// 1 - PCIE2 - Pin Change Interrupt Enable 2 - PCINT[23:16] pin will cause an interrupt
	// 0 - PCIE1
	// 0 - PCIE0 
	PCICR = 0b00000100;
	
	// D6 is PCINT22 pin
	// PCMSK2 - Pin Change Mask Register 2
	// 0 - PCINT23 / D7
	// 1 - PCINT22 / D6
	// 0 - PCINT21 / D5
	// 0 - PCINT20 / D4
	// 0 - PCINT19 / D3
	// 0 - PCINT18 / D2
	// 0 - PCINT17 / D1
	// 0 - PCINT16 / D0
	PCMSK2 = 0b01000000;	

	// start with our load off
	control_load(false);
	
	// enable system interrupts
	sei(); 
}

void loop() {
	
	if(release_calculation) {
		control_load(get_next_load_state());
		release_calculation = false;
	}

}

/* Runs on pin change for D6 which is connected to signal line of sound sensor */
ISR(PCINT2_vect) {
	// just for some visual indicator, toggle the LED connected to D5 
	PORTD ^= (HIGH << PORTD5);
	
	// capture the milliseconds and stick it in the ring buffer
	unsigned long sound_sensed_ms = millis();
	
	// no sense in recording the same value multiple times
	if(sound_sensed_ms == sound_sensed_ms_buf[sound_sensed_ms_buf_pos]) {
		return;
	}
	
	if(sound_sensed_ms_buf_pos < SOUND_BUFFER_LENGTH-1) {
		sound_sensed_ms_buf_pos++;
	} else {
		sound_sensed_ms_buf_pos = 0;
	}
	sound_sensed_ms_buf[sound_sensed_ms_buf_pos] = sound_sensed_ms;
	
	release_calculation = true;
}

// control the load based on the provided argument
void control_load(bool activate) {
	if(activate) {
		// turn on load
		PORTD |= (HIGH << PORTD7);
		is_load_currently_active = true;
	} else {
		// turn off load
		PORTD &= ~(HIGH << PORTD7);
		is_load_currently_active = false;
	}
}

// figure out the next state for the load taking into consideration the samples in our ring buffer
bool get_next_load_state() {

#ifdef DEBUG_ENABLED
	cli();
	for(int i=0;i<SOUND_BUFFER_LENGTH;i++) {
		Serial.print((sound_sensed_ms_buf_pos+i)%SOUND_BUFFER_LENGTH);
		Serial.print(": ");
		Serial.println(sound_sensed_ms_buf[(sound_sensed_ms_buf_pos+i)%SOUND_BUFFER_LENGTH]);
	}
	Serial.println("");
	sei();
#endif

	// it's possible that we may experience interrupts during this function call, so store a copy of the buffer position
	int buf_pos_at_invocation = sound_sensed_ms_buf_pos;
	
	// loop over each element in the buffer
	for(int i=0;i<SOUND_BUFFER_LENGTH;i++) {
		// grab the value for this iteration of outer most loop and store it for convenience
		unsigned long compare_value = sound_sensed_ms_buf[(buf_pos_at_invocation+i)%SOUND_BUFFER_LENGTH];
	
		// compare this to everything else in the buffer - this includes self but we don't care because that should come out as zero	
		for(int j=0;j<SOUND_BUFFER_LENGTH;j++) {
			unsigned long next_val =  sound_sensed_ms_buf[(buf_pos_at_invocation+j)%SOUND_BUFFER_LENGTH];
			unsigned long difference = next_val - compare_value;
			
			// check to see if we've hit on sounds within range
			if(difference < MAX_DIFFERENCE_TOLERANCE_MS && difference > MIN_DIFFERENCE_TOLERANCE_MS) {
				// we need to zero out the buffer, otherwise we'll have two samples within range left in the buffer
				memset((void *)&sound_sensed_ms_buf, 0, sizeof(sound_sensed_ms_buf));
				
				// we should toggle
				return !is_load_currently_active;
			}
		}
	}
	
	// fall back - no samples within range thus keep load in the same state
	return is_load_currently_active;
}