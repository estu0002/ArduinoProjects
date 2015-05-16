/**************************************************************************************
 * Example program demonstrating I/O for SD card
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
 *   
 **************************************************************************************/

int analogReadValue = 0;
  
void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
}

void loop() {
  int adcVal = analogRead(A0); // returns value 0-1023 where 0 is 0V and 1023 is ref voltage (+5V by default)
  
  //               vcc/adc_res * adcVal * 1000mv per V / 10mv per 1'C 
  float celcius = (5.0/1023.0) * adcVal * 1000         /      10;
  Serial.println(celcius);
  
  float fahr = (celcius + 40) * (9.0/5.0) - 40;
  Serial.println(fahr);
  
  delay(1000);
}
