#if !defined(SET_H)
#define SET_H

#include <EEPROM.h>
#include <ArduinoJson.h>
#include "config.h"

// --------------- Handle Dynamic Configs ------------------
#define MAX_INPUT_LENGTH 200

// Error codes
#define WRONG_PASS 200
#define WRONG_ID 201

// --------------------------------------

// --------------- EEPROM ADDRESSES ------------------

#define EEPROM_SIZE 32

const byte EEPROM_KEY = 0x89; // used to identify if valid data in EEPROM

const int KEY_ADDR = 0;        // the EEPROM address used to store the ID
const int Mode_ADDR = 2;       // the EEPROM address used to store MAX_ERRORS
const int Tint_ADDR = 3;       // the EEPROM address used to store Tint (2 bytes)
const int TintB_ADDR = 5;      // the EEPROM address used to store TintB (2 bytes)
const int TGPS_ADDR = 7;       // the EEPROM address used to store TGPS
const int TGPSB_ADDR = 8;      // the EEPROM address used to store TGPSB
const int SMART_ADDR = 9;      // the EEPROM address used to store SMART
const int Tsend_ADDR = 10;     // the EEPROM address used to store Tsend
const int TsendB_ADDR = 11;    // the EEPROM address used to store TsendB
const int trackerID_ADDR = 12; // the EEPROM address used to store trackerID (2 bytes)

// --------------------------------------

// --------------- initial Configs ------------------

#define iMode 0 // Keepalive

// On Battery
#define iTintB 720 // min
#define iTsendB 2  // min
#define iTGPSB 5   // min
#define iSMART true

// On Power connected
#define iTGPS 10 // min
#define iTint 60 // min
#define iTsend 4 // min

// --------------------------------------

class Settings
{

public:
    Settings();

    int Mode,   // (0: Keepalive - 1: Tracking)
        Tint,   // Time interval for position updates (0 - 65535) min
        TintB,  // Time interval for position updates on battery (0 - 65535) min
        TGPS,   // Time allow for fixing location (0 - 255) min
        TGPSB,  // Time allow for fixing location on battery (0 - 255) min
        SMART,  // Smart behaviour on battery (0 - 1)
        TsendB, // Time allow for sending data on battery (0 - 255) min
        Tsend;  // Time allow for sending data (0 - 255) min

    void processConfigs(String json_str),
        setup(),
        setTcheck(int val),
        setMode(int val),
        setTint(unsigned int val),
        setTintB(unsigned int val),
        setTGPS(int val),
        setTGPSB(int val),
        setSMART(int val),
        setTsendB(int val),
        setTsend(int val);
};

#endif // SET_H