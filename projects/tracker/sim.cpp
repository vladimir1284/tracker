#include "sim.h"

// Constructor
SIM::SIM(SoftwareSerial *softSerial)
{
    fonaSerial = softSerial;
}

//--------------------------------------------------------------------
void SIM::setup()
{
    fonaSerial->begin(SIMBaud);
    Serial.println(F("Antes"));
    if (!fona.begin(*fonaSerial))
    {
        if (DEBUG)
        {
            Serial.println(F("Couldn't find FONA"));
        }
        while (1)
            ;
    }
    Serial.println(F("Despues"));

    uint8_t imeiLen = fona.getIMEI(imei);
    if (DEBUG)
    {
        if (imeiLen > 0)
        {
            Serial.print("Module IMEI: ");
            Serial.println(imei);
        }
    }

    fona.setGPRSNetworkSettings(F(APN_NAME), F(""), F(""));

    // Get tracker ID
    trackerID = 1; // TODO get the real value

    readBattery();
}

//--------------------------------------------------------------------
int SIM::readBattery()
{
    // read the battery voltage
    uint16_t tmpV;
    bool ok = fona.getBattVoltage(&tmpV);
    if (ok)
    {
        vbat = tmpV;
    }
    if (DEBUG)
    {
        if (!ok)
        {
            Serial.println(F("Failed to read Batt"));
        }
        else
        {
            Serial.print(F("VBat = "));
            Serial.print(vbat);
            Serial.println(F(" mV"));
        }
    }
}

//--------------------------------------------------------------------
int SIM::run()
{
}
