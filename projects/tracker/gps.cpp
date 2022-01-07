#include "gps.h"

// Constructor
GPS::GPS(HardwareSerial *softSerial)
{
    ss = softSerial;
}

//--------------------------------------------------------------------
void GPS::setup()
{
    ss->begin(GPSBaud, SERIAL_8N1, RXPinGPS, TXPinGPS);
}

//--------------------------------------------------------------------
void GPS::turnOn()
{
    gpio_hold_dis((gpio_num_t)GPS_PWR);
    pinMode(GPS_PWR, OUTPUT);
    digitalWrite(GPS_PWR, HIGH);
    gpsOn = true;
}

//--------------------------------------------------------------------
void GPS::turnOff()
{
    gpio_hold_dis((gpio_num_t)GPS_PWR);
    pinMode(GPS_PWR, OUTPUT);
    digitalWrite(GPS_PWR, LOW);
    gpsOn = false;
}

//--------------------------------------------------------------------
int GPS::run()
{
    // Feed the GPS object
    readGPS(READ_GPS_DELAY);

    if (gps.hdop.isValid())
    {
        if (DEBUG)
        {
            Serial.print("HDOP: ");
            Serial.println(gps.hdop.hdop());
        }

        if (gps.hdop.hdop() < iHPOS)
        {
            // Aceptable resolution achieved
            if (gps.location.isValid())
            {
                gpsData.pending = true;
                gpsData.new_pos = detectNewPosition();
                gpsData.latitude = gps.location.lat();
                gpsData.longitude = gps.location.lng();
                gpsData.speed = gps.speed.kmph();
                gpsData.sat_num = gps.satellites.value();
                gpsData.heading = gps.course.deg();
                gpsData.year = gps.date.year();
                gpsData.month = gps.date.month();
                gpsData.day = gps.date.day();
                gpsData.hour = gps.time.hour();
                gpsData.minute = gps.time.minute();
                gpsData.second = gps.time.second();

                if (DEBUG)
                {
                    Serial.print("String: ");
                    // 10+190109091803+22.63827+114.02922+2.14+69
                    char str[64];
                    sprintf(str, "%d+%02d%02d%02d%02d%02d%02d+%.5f+%.5f+%.2f+%d\0",
                            gpsData.sat_num,
                            gpsData.year-2000,
                            gpsData.month,
                            gpsData.day,
                            gpsData.hour,
                            gpsData.minute,
                            gpsData.second,
                            gpsData.latitude,
                            gpsData.longitude,
                            gpsData.speed,
                            gpsData.heading);
                    Serial.println(str);
                }

                return VALID_LOCATION;
            }
            else
            {
                if (DEBUG)
                {
                    Serial.println("Invalid location!");
                }
            }
        }
        else
        {
            if (DEBUG)
            {
                Serial.println("HDOP > 20!");
            }
        }
    }
    else
    {
        if (DEBUG)
        {
            Serial.println("Invalid HDOP!");
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
            gps.location.lat(),
            gps.location.lng(),
            gpsData.latitude,
            gpsData.longitude);

    if (DEBUG)
    {
        Serial.print("Distance from previous location: ");
        Serial.print(distanceFromPreviousLocation);
        Serial.println("m");
    }
    return (distanceFromPreviousLocation > iRADIUS);
}

//--------------------------------------------------------------------
void GPS::readGPS(unsigned long ms)
{
    unsigned long start = getMillis();
    do
    {
        timerWrite(timer, 0); //reset timer (feed watchdog)
        while (ss->available())
            gps.encode(ss->read());
    } while (getMillis() - start < ms);
}
