/*
 * analogReadTemp.cpp
 *
 * Created: 5/15/2015 7:27:01 PM
 *  Author: Jesse
 *
 * Example program demonstrating I/O for LM35 temperature sensor
 *
 * Device: http://www.ti.com/lit/ds/symlink/lm35.pdf
 * References:
 *  - Analog Read:    http://www.arduino.cc/en/Reference/AnalogRead
 *  - Analog Inputs:  http://www.arduino.cc/en/Tutorial/AnalogInputPins
 * Connection:
 *      LM35                   ARDUINO NANO
 *      VCC    --------------------- +5V
 *      DATA   --------------------- A0
 *      GND    --------------------- GND
 */ 

#include <Arduino.h>
#include <pins_arduino.h>
void setup();
void loop();
int analogReadValue = 0;


void setup() {
	Serial.begin(9600);
	pinMode(A0, INPUT);
}

void loop() {
	int adcVal = analogRead(A0);
	
	//               vcc/adc_res * adcVal * 1000mv per V / 10mv per 1'C 
	float celcius = (5.0/1023.0) * adcVal * 1000         /      10;
	Serial.println(celcius);
	
	float fahr = (celcius + 40) * (9.0/5.0) - 40;
	Serial.println(fahr);
	
	delay(1000);
}

/* NOTE: main() is defined in C:\arduino-1.6.3\hardware\arduino\avr\cores\arduino\main.cpp */


