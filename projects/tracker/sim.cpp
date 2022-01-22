#include "sim.h"
#include "fsm.h"

// Constructor
SIM::SIM(HardwareSerial *softSerial, FSM *fsm)
{
    fonaSerial = softSerial;
    _fsm = fsm;
}

//--------------------------------------------------------------------
void SIM::setup()
{

    fonaSerial->begin(SIMBaud, SERIAL_8N1, FONA_RX, FONA_TX);

    // Configure APN
    // fona.setGPRSNetworkSettings(F(APN_NAME), F(""), F(""));
}

//--------------------------------------------------------------------
void SIM::turnOn()
{
    gpio_hold_dis((gpio_num_t)SIM_PWR);
    pinMode(SIM_PWR, OUTPUT);
    digitalWrite(SIM_PWR, HIGH);
    simOn = true;
}

//--------------------------------------------------------------------
void SIM::turnOff()
{
    gpio_hold_dis((gpio_num_t)SIM_PWR);
    pinMode(SIM_PWR, OUTPUT);
    digitalWrite(SIM_PWR, LOW);
    simOn = false;
}

//--------------------------------------------------------------------
bool SIM::communicate(char *url)
{
    // Initialize the module
    if (!fona.begin(*fonaSerial))
    {
        if (DEBUG)
        {
            Serial.println(F("Couldn't find FONA"));
        }
        return false;
    }

    // Get device imei if needed
    if (imei_len == 0)
    {
        imei_len = fona.getIMEI(imei);
        if (imei_len > 0)
        {
            if (DEBUG)
            {
                Serial.print("Module IMEI: ");
                Serial.println(imei);
            }
        }
        else
        {
            if (DEBUG)
            {
                Serial.println(F("Couldn't find IMEI"));
            }
            return false;
        }
    }
    return true;

    // // turn GPRS on
    // if (!fona.enableGPRS(true))
    // {
    //     if (DEBUG)
    //     {
    //         Serial.println(F("Failed to turn on"));
    //     }
    //     delay(SIMDELAY);
    // }

    // if (!fona.HTTP_GET_start(url, &statuscode, (uint16_t *)&length))
    // {
    //     if (DEBUG)
    //     {
    //         Serial.println("Failed!");
    //     }
    //     delay(SIMDELAY);
    // }

    // bool state = getSerialData(length);

    // // turn GPRS off
    // if (!fona.enableGPRS(false))
    // {
    //     if (DEBUG)
    //     {
    //         Serial.println(F("Failed to turn off"));
    //     }
    //     delay(SIMDELAY);
    // }

    // return state;
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
    return 3765;
    // uint16_t tmpV;
    // bool ok = fona.getBattVoltage(&tmpV);

    // if (!ok)
    // {
    //     if (DEBUG)
    //     {
    //         Serial.println(F("Failed to read Batt"));
    //     }
    //     return 0;
    // }
    // else
    // {
    //     if (DEBUG)
    //     {
    //         Serial.print(F("VBat = "));
    //         Serial.print(tmpV);
    //         Serial.println(F(" mV"));
    //     }
    //     return tmpV;
    // }
}

//--------------------------------------------------------------------
bool SIM::uploadData(bool power)
{
    // Upload data URL path('tracker_data/<passwd>/<int:tracker_id>/<str:lat>/<str:lon>/<int:battery>/<int:power>/<int:errors>',
    //http://trailerrental.pythonanywhere.com/towit/tracker_data/MT;6;864713037301317;R0;5+220109033521+21.38810+-77.91893+0.33+0+0+3765+9
    int mode = 1;
    if (power)
    {
        mode = 6;
    }
    sprintf(input, "%s/%s/MT;%d;%s;R0;%d+%02d%02d%02d%02d%02d%02d+%.5f+%.5f+%.2f+%d+0+%d+%d",
                            BASE_URL, 
                            UPLOAD_URL,
                            mode,
                            imei,
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
                            gpsData.heading,
                            readBattery(),
                            (int)seq_num
                            );
    // sprintf(input, "%s/%s/%s/%u/%ld/%ld/%u/%u/%u", BASE_URL, UPLOAD_URL, PASSWD, trackerID,
    //         latitude, longitude, vbat, power, 0);
    if (DEBUG)
    {
        Serial.println(input);
    }
    if (!communicate(input))
    {
        if (DEBUG)
        {
            Serial.println(F("Couldn't upload data"));
        }
        return false;
    }
    else
    {
        seq_num++;
        return true;
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
        //Init EEPROM
        EEPROM.begin(EEPROM_SIZE);

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

        EEPROM.end();
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