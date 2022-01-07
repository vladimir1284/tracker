#if !defined(SETTINGS_H)
#define SETTINGS_H

#include <Arduino.h>
#include "esp_system.h"

#define DEBUG true

// --------------- watchdog ------------------

#define wdtTimeout 10000000ULL  //time in us to trigger the watchdog

// --------------------------------------


// --------------- SIM ------------------

#define BASE_URL "http://trailerrental.pythonanywhere.com/towit"
#define ID_URL "tracker_id"
#define UPLOAD_URL "tracker_data"
#define CONFIGS_URL "tracker_parameters"
#define PASSWD "c0ntr453n1a"
#define APN_NAME "nauta"

#define RETRAYS 3
#define SIMDELAY 10000 // ms
#define CREDIT_INIT 7
#define CREDIT_LEN 6

#define FONA_RX 18
#define FONA_TX 19
#define FONA_RST -1
#define SIM_PWR 4 // GPIO4 -> RTC_GPIO10
#define SIMBaud 4800

// --------------------------------------

// --------------- GPS ------------------

#define GPS_PWR 2
#define RXPinGPS 16
#define TXPinGPS 17
#define GPSBaud 9600 // USA 4800 | CU 9600

#define READ_GPS_DELAY 1500 // ms Time for continuosly collecting serial data from GPS module
#define iHPOS 20
#define iRADIUS 100 // m

// Data structure
struct GPSdataStruct
{
    bool pending,
        new_pos;
    int sat_num,
        month,
        day,
        year,
        hour,
        minute,
        second,
        heading;
    float latitude,
        longitude,
        speed;
};

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

#define uS_TO_S_FACTOR   1000000ULL /* Conversion factor for micro seconds to seconds */
#define MIN_TO_S_FACTOR 60000000ULL /* Conversion factor for micro seconds to seconds */
#define MIN2MILLIS 600            // 00 x0.1 for debug
#define PIN12V A0                 // Input pin for checking 12V connection

enum states
{
    // Power connected
    IDLE,
    ENERGY,
    READ_GPS,
    SEND_DATA,
    UPDATE_CFG,
    ERROR,
    // Battery powered
    SLEEPING,
    BATTERY,
    BAT_GPS,
    BAT_SEND,
    BAT_CFG,
    BAT_ERROR
};

// --------------------------------------

// --------------- EEPROM ADDRESSES ------------------

#define EEPROM_SIZE 32

const byte EEPROM_KEY = 0x99; // used to identify if valid data in EEPROM

const int KEY_ADDR = 0;        // the EEPROM address used to store the ID
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
#define MAX_INPUT_LENGTH 128

// Error codes
#define WRONG_PASS 200
#define WRONG_ID 201

// --------------------------------------

#endif // SETTINGS_H