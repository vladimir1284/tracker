
#include <sim7000.h>

#define DEBUG true

enum states
{
    // Power connected
    MODEM_PWR_ON,
    GPS_PWR_ON,
    GPS_FIX,
    GPS_PWR_OFF
};

states state;
String imei, gpsData;
Sim7000 sim;

void setup()
{
    sim.setup();
    state = MODEM_PWR_ON;
}

void loop()
{
    switch (state)
    {
    case MODEM_PWR_ON:
        imei = sim.turnON();
        if (imei.equals(String(NULL)))
        {
            if (DEBUG)
            {
                Serial.println("Delaying 10s and retrying...");
            }
            delay(10000);
        }
        else
        {
            if (DEBUG)
            {
                Serial.println("IMEI: " + imei);
            }
            state = GPS_PWR_ON;
        }
        break;

    case GPS_PWR_ON:
        if (sim.powerOnGPS())
        {
            if (DEBUG)
            {
                Serial.println("Waiting 15s for warm-up...");
            }
            delay(15000);
            state = GPS_FIX;
        }
        break;

    case GPS_FIX:
        gpsData = sim.getGPS();
        if (gpsData.equals(String(NULL)))
        {
            if (DEBUG)
            {
                Serial.println("Retrying in 5s...");
            }
            delay(5000);
        }
        else
        {
            if (DEBUG)
            {
                Serial.println("GPD data str: " + gpsData);
            }
            state = GPS_PWR_OFF;
        }
        break;

    case GPS_PWR_ON:
        if (sim.powerOffGPS())
        {
            if (DEBUG)
            {
                Serial.println("GPS disabled!");
            }

            // Terminate test
            if (DEBUG)
            {
                Serial.println("Default state. Stop the execution!");
            }
            while (1)
                ;
        }
        break;

    default:
        break;
    }
}
