/*
  GPS Logger

  Simple project which logs data from GPS module (NEO 6M) into SD card. 
  Locations are stored as file (yyyyMMdd.txt) and the file will contain one row per location (dd.MM.yyyy HH:mm:ss lat,lon). 
  Location is stored for each interval given as configuration variable 'frequency'. 

  Led modes:
  continuous -> error
  blinking -> looking for location
  off -> everything ok

  Connecting modules:
  Pin3 -> GPS-module-RX
  Pin4 -> GPS-module-TX
  Pin10 -> SD-module-SS
  Pin11/MOSI -> SD-module-MOSI
  Pin12/MISO -> SD-module-MISO
  Pin13/SCK -> SD-module-SCK

  Dependency(TinyGPS++ library): http://arduiniana.org/libraries/tinygpsplus/
  
  created   Apr 2017
  by CheapskateProjects

  ---------------------------
  The MIT License (MIT)

  Copyright (c) 2017 CheapskateProjects

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

// Pins used for communicating with GPS module
static const int RXPin = 4, TXPin = 3;
// Status led
static const int GpsLedPin = 9;
// Baud rate of your GPS module (usually 4800 or 9600)
static const uint32_t GPSBaud = 9600;
// How frequently should we save current location (milliseconds)
static const unsigned long frequency = 5000;

// gps object
TinyGPSPlus gps;
// true when we have found location since last restart and file has been opened
boolean opened = false;
// current data file
File dataFile;
// file name
String fileName;
// timestamp of previous location capture. Used to check if we should save this location or skip it
unsigned long previous=0;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  ss.begin(GPSBaud);
  pinMode(GpsLedPin, OUTPUT);
  digitalWrite(GpsLedPin, LOW);

  if (!SD.begin(10))
  {
    digitalWrite(GpsLedPin, HIGH);
    while(true);
  }
}

void loop()
{
  // If we have data, decode and log the data
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      logInfo();

  // Test that we have had something from GPS module within first 10 seconds
  if (millis() > 10000 && gps.charsProcessed() < 10)
  {
    // Set error led
    digitalWrite(GpsLedPin, HIGH);
    // Wiring error so stop trying
    while(true);
  }
}

// Help function to pad 0 prefix when valus < 10
void printIntValue(int value)
{
  if(value < 10)
  {
    dataFile.print(F("0"));
  }
  dataFile.print(value);
}

// Log current info if we have valid location
void logInfo()
{
  // Wait until we have location locked!
  if(!gps.location.isValid())
  {
    digitalWrite(GpsLedPin, HIGH);
    delay(20);
    digitalWrite(GpsLedPin, LOW);
    return;
  }

  if(!opened)
  {
    // When we first get something to log we take file name from that time
    fileName = "";
    fileName += gps.date.year();
    if(gps.date.month() < 10) fileName += "0";
    fileName += gps.date.month();
    if(gps.date.day() < 10) fileName += "0";
    fileName += gps.date.day();
    fileName += ".txt";
    opened = true;
  }

  // Show that everything is ok
  digitalWrite(GpsLedPin, LOW);

  if(millis() - previous > frequency)
  {
    previous = millis();
    // Write data row (dd.MM.yyyy HH:mm:ss lat,lon)
    dataFile = SD.open(fileName, FILE_WRITE);
    printIntValue(gps.date.day());
    dataFile.print(F("."));
    printIntValue(gps.date.month());
    dataFile.print(F("."));
    dataFile.print(gps.date.year());
    dataFile.print(F(" "));
    printIntValue(gps.time.hour());
    dataFile.print(F(":"));
    printIntValue(gps.time.minute());
    dataFile.print(F(":"));
    printIntValue(gps.time.second());
    dataFile.print(F(" "));
    dataFile.print(gps.location.lat(), 6);
    dataFile.print(F(","));
    dataFile.print(gps.location.lng(), 6);
    dataFile.println();
    dataFile.close();
  }
}
