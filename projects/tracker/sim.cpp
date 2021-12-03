#include "sim.h"

// Constructor
SIM::SIM(SoftwareSerial *softSerial)
{
    fonaSerial = softSerial;

    fona = Adafruit_FONA(FONA_RST);
}

//--------------------------------------------------------------------
void SIM::setup()
{
    fonaSerial->begin(SIMBaud);
    if (DEBUG)
    {
        if (!fona.begin(*fonaSerial))
        {
            Serial.println(F("Couldn't find FONA"));
            while (1)
                ;
        }
    }

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

//--------------------------------------------------------------------
uint8_t SIM::readline(char *buff, uint8_t maxbuff, uint16_t timeout)
{
    uint16_t buffidx = 0;
    boolean timeoutvalid = true;
    if (timeout == 0)
        timeoutvalid = false;

    while (true)
    {
        if (buffidx > maxbuff)
        {
            //Serial.println(F("SPACE"));
            break;
        }

        while (Serial.available())
        {
            char c = Serial.read();

            //Serial.print(c, HEX); Serial.print("#"); Serial.println(c);

            if (c == '\r')
                continue;
            if (c == 0xA)
            {
                if (buffidx == 0) // the first 0x0A is ignored
                    continue;

                timeout = 0; // the second 0x0A is the end of the line
                timeoutvalid = true;
                break;
            }
            buff[buffidx] = c;
            buffidx++;
        }

        if (timeoutvalid && timeout == 0)
        {
            //Serial.println(F("TIMEOUT"));
            break;
        }
        delay(1);
    }
    buff[buffidx] = 0; // null term
    return buffidx;
}