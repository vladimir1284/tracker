#include "gps.h"

// Constructor
GPS::GPS(HardwareSerial *softSerial)
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
    ss->begin(GPSBaud, SERIAL_8N1, RXPinGPS, TXPinGPS);
}

//--------------------------------------------------------------------
int GPS::run()
{
    // Feed the GPS object
    smartDelay(READ_GPS_DELAY);

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

    if (DEBUG)
    {
        Serial.print(distanceFromPreviousLocation);
        Serial.println("m");
    }
    return (distanceFromPreviousLocation > newPosRadius);
}

//--------------------------------------------------------------------
void GPS::smartDelay(unsigned long ms)
{
    unsigned long start = getMillis();
    do
    {
        // wdt_reset(); //Reset the watchdog
        while (ss->available())
            gps.encode(ss->read());
    } while (getMillis() - start < ms);
}
