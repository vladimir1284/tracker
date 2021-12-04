#if !defined(SIM_H)
#define SIM_H

#include "configs.h"
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"

class SIM
{

public:
    SIM(SoftwareSerial *softSerial);

    void setup();

    int run();

private:
    // Adafruit_FONA fona;
    Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
    SoftwareSerial *fonaSerial;

    char imei[16] = {0}; // MUST use a 16 character buffer for IMEI!
    int trackerID = 0;
    uint16_t vbat; // mV

    int readBattery();

    float previousLon,
        previousLat;
};

#endif // SIM_H