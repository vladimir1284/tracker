#if !defined(SIM_H)
#define SIM_H

#include "configs.h"
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"

class FSM;

class SIM
{

public:
    SIM(SoftwareSerial *softSerial, FSM *fsm);
    bool setup(),
        uploadData(float lat, float lon, bool power);

private:
    // Adafruit_FONA fona;
    Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
    SoftwareSerial *fonaSerial;

    uint16_t statuscode;
    int16_t length;
    // char url[URL_SIZE];
    char imei[16] = {0}; // MUST use a 16 character buffer for IMEI!
    int trackerID = 0;

    int readBattery();
    float getCredit();

    bool getSerialData(int length),
        communicate(char *url),
        parseResponse(int length);

    float previousLon,
        previousLat;

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

    char input[MAX_INPUT_LENGTH];
    FSM *_fsm;

    void updateTsend(int value),
        updateTint(int value),
        updateTGPS(int value),
        updateSMART(int value),
        updateTGPSB(int value),
        updateTsendB(int value),
        updateTintB(int value),
        updateMAX_ERRORS(int value),
        updateTcheck(int value),
        updateTrackerID(int value),
        getRemoteConfigs();
};

#endif // SIM_H