#if !defined(SETTINGS_H)
#define SETTINGS_H

#include <Arduino.h>
#include <SoftwareSerial.h>
// #include <ArduinoJson.h>
#define DEBUG true

// --------------- SIM ------------------

#define UPLOAD_URL "http://trailerrental.pythonanywhere.com/towit/tracker_data/c0ntr453n1a/"
#define ID_URL "http://trailerrental.pythonanywhere.com/towit/tracker_id/c0ntr453n1a/"
#define APN_NAME "nauta"
#define URL_SIZE 120

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4
#define SIMBaud 4800

// --------------------------------------

// --------------- GPS ------------------

#define RXPinGPS 6
#define TXPinGPS 5
#define GPSBaud 9600 // USA 4800 | CU 9600

#define READ_GPS_DELAY 1000 // ms Time for continuosly collecting serial data from GPS module
#define iHPOS 20
#define iRADIUS 100 // m

// --------------------------------------

// --------------- initial Configs ------------------

#define iTcheck 15 // min
#define iMAX_ERRORS 3

// On Battery
#define iTintB 360 // min
#define iTsendB 10 // min
#define iTGPSB 10  // min
#define iSMART true

// On Power connected
#define iTGPS 10  // min
#define iTint 60  // min
#define iTsend 10 // min

// --------------------------------------

// --------------- FSM ------------------

#define PIN12V A0  // Input pin for checking 12V connection

// --------------------------------------

// --------------- EEPROM ADDRESSES ------------------

const byte EEPROM_KEY = 0x99; // used to identify if valid data in EEPROM

const int KEY_ADDR = 0;         // the EEPROM address used to store the ID
const int Tcheck_ADDR = 1;     // the EEPROM address used to store Tcheck
const int MAX_ERRORS_ADDR = 2; // the EEPROM address used to store MAX_ERRORS
const int Tint_ADDR = 3;       // the EEPROM address used to store Tint (2 bytes)
const int TintB_ADDR = 5;      // the EEPROM address used to store TintB (2 bytes)
const int TGPS_ADDR = 7;       // the EEPROM address used to store TGPS
const int TGPSB_ADDR = 8;      // the EEPROM address used to store TGPSB
const int SMART_ADDR = 9;      // the EEPROM address used to store SMART
const int Tsend_ADDR = 10;     // the EEPROM address used to store Tsend
const int TsendB_ADDR = 11;    // the EEPROM address used to store TsendB
const int trackerID_ADDR = 12; // the EEPROM address used to store trackerID (2 bytes)
// const int any = 14;    // the EEPROM address used to store

// --------------------------------------

// --------------- Handle Dynamic Configs ------------------
#define MAX_INPUT_LENGTH 200

// Error codes
#define WRONG_PASS 200
#define WRONG_ID   201


// --------------------------------------

#endif // SETTINGS_H