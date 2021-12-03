#if !defined(SETTINGS_H)
#define SETTINGS_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#define DEBUG true

// --------------- SIM ------------------

#define UPLOAD_URL "http://trailerrental.pythonanywhere.com/towit/tracker_data/c0ntr453n1a/"
#define ID_URL "http://trailerrental.pythonanywhere.com/towit/tracker_id/c0ntr453n1a/"
#define APN_NAME "nauta"

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4
#define SIMBaud 4800

// --------------------------------------

// --------------- GPS ------------------

#define RXPinGPS 6
#define TXPinGPS 5
#define GPSBaud 9600

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

// --------------- Handle Dynamic Configs ------------------
#define MAX_INPUT_LENGTH 200
#define LBRACE 123 // { ascii code
#define RBRACE 125 // } ascii code

class Configs
{

public:
    Configs(SoftwareSerial *softSerial);

    void getSerialData();

private:
    int len, open;

    int configs_Tcheck,
        configs_MAX_ERRORS,
        configs_Tint,
        configs_TintB,
        configs_TsendB,
        configs_TGPSB,
        configs_SMART,
        configs_TGPS,
        configs_Tsend;

    StaticJsonDocument<96> doc;
    char input[MAX_INPUT_LENGTH];
    SoftwareSerial *ss;

    void updateTsend(),
        updateTint(),
        updateTGPS(),
        updateSMART(),
        updateTGPSB(),
        updateTsendB(),
        updateTintB(),
        updateMAX_ERRORS(),
        updateTcheck(),
        parseJSON();
};

// --------------------------------------

#endif // SETTINGS_H