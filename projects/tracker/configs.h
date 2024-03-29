#if !defined(SETTINGS_H)
#define SETTINGS_H

#include <Arduino.h>
#include "esp_system.h"

#define DEBUG true

// --------------- watchdog ------------------

#define wdtTimeout 60000000ULL // time in us to trigger the watchdog

// --------------------------------------

// --------------- SIM ------------------

#define MSG_SIZE 128
#define APN_NAME "hologram"

#define RETRIES 3
#define CREDIT_INIT 7
#define CREDIT_LEN 6

#define FONA_RX 27
#define FONA_TX 26
#define SIM_PWR 5    // NRESET
#define PWRKEY 4     // GPIO4 -> RTC_GPIO10
#define STATUSLED 12 // blue led near ESP32

// --------------------------------------

// --------------- MQTT PARAMETERS -----------------
#define SERVER "http://trailerrental.pythonanywhere.com"
#define ADDR "/towit/tracker_data"

// --------------------------------------

// --------------- initial Configs ------------------

#define iMAX_ERRORS 3

// On Battery
#define iTintB 720 // min
#define iTsendB 2  // min
#define iTGPSB 5   // min
#define iSMART true

// On Power connected
#define iTGPS 10 // min
#define iTint 60 // min
#define iTsend 3 // min

// --------------------------------------

// --------------- FSM ------------------

#define S_TO_uS_FACTOR 1000000ULL    /* Conversion factor for seconds to micro seconds */
#define MIN_TO_uS_FACTOR 60000000ULL // 60000000ULL /* Conversion factor for minutes to micro seconds */
#define MIN_TO_S_FACTOR 60           // 0 x0.1 for debug
#define PIN12V 36                    // Input pin for checking 12V connection
#define MIN_VBAT 2900                // Minimum operational battery voltage (mV)
#define PWR_PIN_BITMASK 0x1000000000 // 2^PIN12V 0b10000,00000000,00000000,00000000,00000000

enum modes
{
    // Power connected
    POWER_ON,
    BATTERY
};

enum states
{
    // Power connected
    IDLE,
    READ_GPS,
    SEND_DATA,
    ERROR
};

// --------------------------------------

// --------------- EEPROM ADDRESSES ------------------

#define EEPROM_SIZE 32

const byte EEPROM_KEY = 0x89; // used to identify if valid data in EEPROM

const int KEY_ADDR = 0; // the EEPROM address used to store the ID
// const int Tcheck_ADDR = 1;     // the EEPROM address used to store Tcheck
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
#define WRONG_ID 201

// --------------------------------------

// --------------- Vibration sensor ------------------
#define MOVTHRESHOLD 1
#define MOVDELAY 10
#define PINVBR 33
#define BAT_ADC 35
#define VBR_PIN_BITMASK 0x200000000 // 2^PINVBR 0b10,00000000,00000000,00000000,00000000

// --------------------------------------

#endif // SETTINGS_H