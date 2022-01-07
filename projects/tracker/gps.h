#if !defined(GPS_H)
#define GPS_H

#include "configs.h"
#include <TinyGPS++.h>
#include <HardwareSerial.h>

#define VALID_LOCATION 1
#define INVALID_LOCATION 0

extern unsigned long getMillis();
extern bool gpsOn;
extern GPSdataStruct gpsData;
extern hw_timer_t *timer;

class GPS
{

public:
    GPS(HardwareSerial *softSerial);

    void clearPendingData(),
        setup(),
        turnOn(),
        turnOff();

    int run();

private:
    // The TinyGPS++ object
    TinyGPSPlus gps;
    HardwareSerial *ss;

    float previousLon,
        previousLat;

    void readGPS(unsigned long ms);

    bool detectNewPosition();
};

#endif // GPS_H