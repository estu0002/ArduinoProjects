
#include <DHT.h>
#include <SPI.h>
#include <SD.h>
#include <stdlib.h> // for dtostrf
#include <DS3231.h>

#define DHTPIN 2       // digital pin
#define DHTTYPE DHT11  // DHT 11

DS3231  rtc(SDA, SCL);  // uses hardware interface for I2C
DHT dht(DHTPIN, DHTTYPE);
File logFile;

void setup() {

  // start the temp/humidity sensor
  dht.begin();

  // start the SD card device
  SD.begin(10); // CS for SPI device on pin 10

  // start the RTC
  rtc.begin();

  // comment to set the date and time
//  rtc.setDOW(FRIDAY);     // Set Day-of-Week to SUNDAY
//  rtc.setTime(11, 24, 0);     // Set the time to 12:00:00 (24hr format)
//  rtc.setDate(2, 12, 2016);   // Set the date to January 1st, 2014
  
}

void loop() {
  // wait approximately 60 seconds between measurements.
  delay(60000); //TODO make this interrupt driven

  readAndLogData();
}

void readAndLogData() {
  // NOTE - temperature or humidity readings can take about 250 milliseconds
  // and readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();

  // temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);

  logFile = SD.open("dht11.log", FILE_WRITE);  // includes append

  // log data from the DS3231
  logFile.print(rtc.getDateStr());
  logFile.print(' ');
  logFile.print(rtc.getTimeStr());
  logFile.print(',');
  
  // log data from the DHT11
  char buf[4];

  // log Fahrenheit temperature
  dtostrf(f, 4, 1, buf);
  logFile.print(buf);
  logFile.print(',');

  // log humidity
  dtostrf(h, 4, 1, buf);
  logFile.print(buf);
  logFile.print(',');

  // log heat index
  dtostrf(hif, 4, 1, buf);
  logFile.println(buf); // write newline char
  
  logFile.close();
}

