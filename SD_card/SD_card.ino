#include <SPI.h>
#include <SD.h>

/**************************************************************************************
 * Example program demonstrating I/O for SD card
 *
 * Device: http://www.gearbest.com/development-boards/pp_63013.html
 * References:
 *  - SD:  http://www.arduino.cc/en/Reference/SD
 *  - SPI: http://www.arduino.cc/en/Reference/SPI
 *         https://learn.sparkfun.com/tutorials/serial-peripheral-interface-spi
 * Connection:
 *    SD BOARD                    ARDUINO NANO
 *      3V3    --------------------- +3.3V
 *      CS/SS  --------------------- D10
 *      MOSI   --------------------- D11
 *      CLK    --------------------- D13
 *      MSIO   --------------------- D12
 *      GND    --------------------- GND
 *   
 **************************************************************************************/

void setup() {

  Serial.begin(9600);
  
  Serial.println("SD card init...");
  
  if(SD.begin()) { // no arg begin() uses default CS pin of D10 on nano
    Serial.println("init complete");
  } else {
    Serial.println("INIT FAILED");
  }
  
  File myFile = SD.open("/testfile.txt", FILE_WRITE);
  myFile.println("whatevs!");
  myFile.close();
  
  Serial.println("file written");
}
  

void loop() {
  // do nothing
}
