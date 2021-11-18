#if !defined(GPS_H)
#define GPS_H

#include "configs.h"
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define VALID_LOCATION 1
#define INVALID_LOCATION 0

class GPS
{

public:
    GPS(SoftwareSerial *softSerial);

    void clearPendingData(),
    setup();

    int run();

    bool detectNewPosition();

    float lastLon,
        lastLat;

    bool pendingData; // Data available to be send

    int resolution,   // Maximum HPOS value
        newPosRadius; // Minimum displacement for considering a new position

private:
    // The TinyGPS++ object
    TinyGPSPlus gps;
    SoftwareSerial *ss;

    float previousLon,
        previousLat;

    // void trunOnSpeed0(),
    //     trunOffSpeed0();
};

#endif // GPS_H