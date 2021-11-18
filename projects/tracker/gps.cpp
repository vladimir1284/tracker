#include "gps.h"

// Constructor
GPS::GPS(SoftwareSerial *softSerial)
{
    ss = softSerial;
    resolution = iHPOS;
    newPosRadius = iRADIUS;
    pendingData = false;
    lastLon = 0;
    lastLat = 0;
    previousLon = 0;
    previousLat = 0;
}

//--------------------------------------------------------------------
void GPS::setup()
{
    ss->begin(GPSBaud);
}

//--------------------------------------------------------------------
int GPS::run()
{
    // Feed the GPS object
    while (ss->available())
    {
        gps.encode(ss->read());
    }

    if (gps.hdop.isValid())
    {
        if (gps.hdop.hdop() < resolution)
        {
            // Aceptable resolution achieved
            if (gps.location.isValid())
            {
                pendingData = true;
                previousLon = lastLon;
                previousLat = lastLat;
                lastLon = gps.location.lng();
                lastLat = gps.location.lat();
                return VALID_LOCATION;
            }
        }
    }
    return INVALID_LOCATION;
}

//--------------------------------------------------------------------
bool GPS::detectNewPosition()
{
    // Distance in meters
    unsigned long distanceFromPreviousLocation =
        (unsigned long)TinyGPSPlus::distanceBetween(
            lastLat,
            lastLon,
            previousLat,
            previousLon);

    return (distanceFromPreviousLocation > newPosRadius);
}