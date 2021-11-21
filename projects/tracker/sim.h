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
    Adafruit_FONA fona;
    SoftwareSerial *fonaSerial;

    char imei[16] = {0}; // MUST use a 16 character buffer for IMEI!
    int trackerID = 0;
    uint16_t vbat; // mV

    uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout);

    float previousLon,
        previousLat;
};

#endif // SIM_H