#include "sim.h"

// Constructor
SIM::SIM(SoftwareSerial *softSerial, FSM *fsm)
{
    fonaSerial = softSerial;
    _fsm = fsm;
}

//--------------------------------------------------------------------
void SIM::setup()
{
    fonaSerial->begin(SIMBaud);

    if (!fona.begin(*fonaSerial))
    {
        if (DEBUG)
        {
            Serial.println(F("Couldn't find FONA"));
        }
        while (1)
            ;
    }

    // Configure APN
    fona.setGPRSNetworkSettings(F(APN_NAME), F(""), F(""));

    // Get tracker ID
    byte key = EEPROM.read(KEY_ADDR); // read the first byte from the EEPROM
    if (key == (EEPROM_KEY + 1))
    {
        byte hiByte = EEPROM.read(trackerID_ADDR);
        byte lowByte = EEPROM.read(trackerID_ADDR + 1);
        trackerID = word(hiByte, lowByte); // see word function in Recipe 3.15
        if (DEBUG)
        {
            Serial.print(F("Tracker ID:"));
            Serial.println(trackerID);
        }
    }
    else
    {
        uint8_t imeiLen = fona.getIMEI(imei);
        if (DEBUG)
        {
            Serial.print("Module IMEI: ");
            Serial.println(imei);
        }
        if (imeiLen > 0)
        {
            // Get ID URL
            sprintf(input, "%s/%s/%s/%s", BASE_URL, ID_URL, PASSWD, imei);
            if (!communicate(input))
            {
                if (DEBUG)
                {
                    Serial.println(F("Couldn't get ID from remote server"));
                }
                while (1)
                    ;
            }
        }
        else
        {
            if (DEBUG)
            {
                Serial.println(F("Couldn't find IMEI"));
            }
            while (1)
                ;
        }
    }
    readBattery();
}

//--------------------------------------------------------------------
bool SIM::communicate(char *url)
{
    // turn GPRS on
    for (int i = 0; i < RETRAYS; i++)
    {
        if (!fona.enableGPRS(true))
        {
            if (DEBUG)
            {
                Serial.println(F("Failed to turn on"));
            }
            delay(SIMDELAY);
        }
        else
        {
            break;
        }
    }
    if (DEBUG)
    {
        Serial.println(url);
    }
    for (int i = 0; i < RETRAYS; i++)
    {
        if (!fona.HTTP_GET_start(url, &statuscode, (uint16_t *)&length))
        {
            if (DEBUG)
            {
                Serial.println("Failed!");
            }
            delay(SIMDELAY);
        }
        else
        {
            break;
        }
    }

    bool state = getSerialData(length);

    // turn GPRS off
    for (int i = 0; i < RETRAYS; i++)
    {
        if (!fona.enableGPRS(false))
        {
            if (DEBUG)
            {
                Serial.println(F("Failed to turn off"));
            }
            delay(SIMDELAY);
        }
        else
        {
            break;
        }
    }

    return state;
}

//--------------------------------------------------------------------
void SIM::getRemoteConfigs()
{
    // Get Parameters URL
    sprintf(input, "%s/%s/%s/%u", BASE_URL, CONFIGS_URL, PASSWD, trackerID);
    if (!communicate(input))
    {
        if (DEBUG)
        {
            Serial.println(F("Couldn't get parameters from remote server"));
        }
    }
}

//--------------------------------------------------------------------
float SIM::getCredit()
{
    char value[CREDIT_LEN];
    uint16_t ussdlen;
    if (!fona.sendUSSD("*222#", input, MAX_INPUT_LENGTH, &ussdlen))
    { // pass in buffer and max len!
        if (DEBUG)
        {
            Serial.println(F("Failed get credit"));
        }
    }
    else
    {
        for (int i = 0; i < (CREDIT_LEN - 1); i++)
        {
            value[i] = input[i + CREDIT_INIT];
        }
        float val = String(value).toFloat();
        if (DEBUG)
        {
            Serial.println(F("Sent!"));
            Serial.print(F("***** USSD Reply"));
            Serial.print(" (");
            Serial.print(ussdlen);
            Serial.println(F(") bytes *****"));
            Serial.println(input);
            Serial.println(val);
            Serial.println(F("*****"));
        }
        return val;
    }
}

//--------------------------------------------------------------------
int SIM::readBattery()
{
    // read the battery voltage
    uint16_t tmpV;
    bool ok = fona.getBattVoltage(&tmpV);

    if (!ok)
    {
        if (DEBUG)
        {
            Serial.println(F("Failed to read Batt"));
        }
        return 0;
    }
    else
    {
        if (DEBUG)
        {
            Serial.print(F("VBat = "));
            Serial.print(tmpV);
            Serial.println(F(" mV"));
        }
        return tmpV;
    }
}

//--------------------------------------------------------------------
void SIM::uploadData(float lat, float lon, bool power)
{
    long latitude = (long)(100000 * lat);
    long longitude = (long)(100000 * lon);
    int vbat = readBattery();

    // Upload data URL path('tracker_data/<passwd>/<int:tracker_id>/<str:lat>/<str:lon>/<int:battery>/<int:power>/<int:errors>',
    sprintf(input, "%s/%s/%s/%u/%ld/%ld/%u/%u/%u", BASE_URL, UPLOAD_URL, PASSWD, trackerID,
            latitude, longitude, vbat, power, 0);
    if (!communicate(input))
    {
        if (DEBUG)
        {
            Serial.println(F("Couldn't upload data"));
        }
    }
}

bool SIM::getSerialData(int length)
{
    char c;
    len = 0;
    while (length > 0)
    {
        while (fona.available())
        {
            c = fona.read();
            input[len++] = c;
            length--;
            if (!length)
                break;
        }
    }
    fona.HTTP_GET_end();

    return parseResponse(len);
}

bool SIM::parseResponse(int length)
{
    if (length > 0)
    {
        if (input[0] >= WRONG_PASS)
        {
            if (DEBUG)
            {
                switch (input[0])
                {
                case WRONG_PASS:
                    Serial.println("Password incorecto!");
                    break;

                case WRONG_ID:
                    Serial.println("ID incorecto!");
                    break;

                default:
                    break;
                }
            }
            return false; // Error from server
        }
        else
        {
            if (input[0] > 0)
            {
                for (int i = 0; i < input[0]; i++)
                {
                    byte pos = 3 * i + 1;
                    byte address = input[pos];
                    byte loByte = input[pos + 1];
                    byte hiByte = input[pos + 2];

                    switch (address)
                    {
                    case Tcheck_ADDR:
                        updateTcheck(loByte);
                        break;
                    case MAX_ERRORS_ADDR:
                        updateMAX_ERRORS(loByte);
                        break;
                    case Tint_ADDR:
                        updateTint(word(hiByte, loByte));
                        break;
                    case TintB_ADDR:
                        updateTintB(word(hiByte, loByte));
                        break;
                    case TGPS_ADDR:
                        updateTGPS(loByte);
                        break;
                    case TGPSB_ADDR:
                        updateTGPSB(loByte);
                        break;
                    case SMART_ADDR:
                        updateSMART(loByte);
                        break;
                    case Tsend_ADDR:
                        updateTsend(loByte);
                        break;
                    case TsendB_ADDR:
                        updateTsendB(loByte);
                        break;
                    case trackerID_ADDR:
                        updateTrackerID(word(hiByte, loByte));
                        break;
                    default:
                        return false;
                    }
                }
                return true; // Success
            }
            else
            {
                return true; // Nothing to process
            }
        }
    }
    else
    {
        return false; // Empty response
    }
}

void SIM::updateTrackerID(int value)
{
    if (trackerID != value)
    {
        trackerID = value;
        byte hiByte = highByte(value);
        byte loByte = lowByte(value);
        EEPROM.write(trackerID_ADDR, hiByte);
        EEPROM.write(trackerID_ADDR + 1, loByte);
        EEPROM.write(KEY_ADDR, EEPROM_KEY + 1); // write the Key to indicate valid ID data
        if (DEBUG)
        {
            Serial.print("Tracker ID from remote server: ");
            Serial.println(value);
        }
    }
}

void SIM::updateTcheck(int value)
{
    if (value > 0)
    {
        if (DEBUG)
        {
            Serial.print("Tcheck: ");
            Serial.println(value);
        }
        _fsm->setTcheck(value);
    }
}
void SIM::updateMAX_ERRORS(int value)
{
    if (value > 0)
    {
        if (DEBUG)
        {
            Serial.print("MAX_ERRORS: ");
            Serial.println(value);
        }
        _fsm->setMAX_ERRORS(value);
    }
}
void SIM::updateTintB(int value)
{
    if (value > 0)
    {
        if (DEBUG)
        {
            Serial.print("TintB: ");
            Serial.println(value);
        }
        _fsm->setTintB(value);
    }
}
void SIM::updateTsendB(int value)
{
    if (value > 0)
    {
        if (DEBUG)
        {
            Serial.print("TsendB: ");
            Serial.println(value);
        }
        _fsm->setTsendB(value);
    }
}
void SIM::updateTGPSB(int value)
{
    if (value > 0)
    {
        if (DEBUG)
        {
            Serial.print("TGPSB: ");
            Serial.println(value);
        }
        _fsm->setTGPSB(value);
    }
}
void SIM::updateSMART(int value)
{
    if (value > -1)
    {
        if (DEBUG)
        {
            Serial.print("SMART: ");
            Serial.println(value);
        }
        _fsm->setSMART(value);
    }
}
void SIM::updateTGPS(int value)
{
    if (value > 0)
    {
        if (DEBUG)
        {
            Serial.print("TGPS: ");
            Serial.println(value);
        }
        _fsm->setTGPS(value);
    }
}
void SIM::updateTint(int value)
{
    if (value > 0)
    {
        if (DEBUG)
        {
            Serial.print("Tint: ");
            Serial.println(value);
        }
        _fsm->setTint(value);
    }
}

void SIM::updateTsend(int value)
{
    if (value > 0)
    {
        if (DEBUG)
        {
            Serial.print("Tsend: ");
            Serial.println(value);
        }
        _fsm->setTsend(value);
    }
}