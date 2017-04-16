# GPSLogger
GPS Logger

Simple project which logs data from GPS module (NEO 6M) into SD card. 
Locations are stored as file (yyyyMMdd.txt) and the file will contain one row per location (dd.MM.yyyy HH:mm:ss lat,lon). 
Location is stored for each interval given as configuration variable 'frequency'. 

Led modes:
continuous - error
blinking - looking for location
off - everything ok

Connecting modules:
Pin3 - GPS-module-RX
Pin4 - GPS-module-TX
Pin10 - SD-module-SS
Pin11/MOSI - SD-module-MOSI
Pin12/MISO - SD-module-MISO
Pin13/SCK - SD-module-SCK

Dependency(TinyGPS++ library): <http://arduiniana.org/libraries/tinygpsplus/>
  

