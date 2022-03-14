#include "settings.h"

// Constructor
Settings::Settings()
{
}

//--------------------------------------------------------------------
void Settings::setup(Sim7000 *sim_device)
{
    _sim_device = sim_device;

    if (MAX_ERRORS == 0)
    {
        //Init EEPROM
        EEPROM.begin(EEPROM_SIZE);

        byte key = EEPROM.read(KEY_ADDR); // read the first byte from the EEPROM
        if (key == EEPROM_KEY || key == (EEPROM_KEY + 1))
        {
            // here if the key value read matches the value saved when writing eeprom
            if (DEBUG)
            {
                Serial.println("Using data from EEPROM");
            }
            // Tcheck = EEPROM.read(Tcheck_ADDR);
            MAX_ERRORS = EEPROM.read(MAX_ERRORS_ADDR);

            byte hiByte = EEPROM.read(Tint_ADDR);
            byte lowByte = EEPROM.read(Tint_ADDR + 1);
            Tint = word(hiByte, lowByte); // see word function in Recipe 3.15

            hiByte = EEPROM.read(TintB_ADDR);
            lowByte = EEPROM.read(TintB_ADDR + 1);
            TintB = word(hiByte, lowByte); // see word function in Recipe 3.15

            TGPS = EEPROM.read(TGPS_ADDR);
            TGPSB = EEPROM.read(TGPSB_ADDR);
            SMART = EEPROM.read(SMART_ADDR);
            Tsend = EEPROM.read(Tsend_ADDR);
            TsendB = EEPROM.read(TsendB_ADDR);
        }
        else
        {
            // here if the key is not found, so write the default data
            if (DEBUG)
            {
                Serial.println("Writing default data to EEPROM");
            }
            // setTcheck(iTcheck);
            setMAX_ERRORS(iMAX_ERRORS);
            setTint(iTint);
            setTintB(iTintB);
            setTGPS(iTGPS);
            setTGPSB(iTGPSB);
            setSMART(iSMART);
            setTsend(iTsend);
            setTsendB(iTsendB);

            EEPROM.write(KEY_ADDR, EEPROM_KEY); // write the KEY to indicate valid data
        }

        EEPROM.end();

        if (DEBUG)
        {
            Serial.print("MAX_ERRORS: ");
            Serial.println(MAX_ERRORS);
            Serial.print("Tint: ");
            Serial.println(Tint);
            Serial.print("TintB: ");
            Serial.println(TintB);
            Serial.print("TGPS: ");
            Serial.println(TGPS);
            Serial.print("TGPSB: ");
            Serial.println(TGPSB);
            Serial.print("SMART: ");
            Serial.println(SMART);
            Serial.print("Tsend: ");
            Serial.println(Tsend);
            Serial.print("TsendB: ");
            Serial.println(TsendB);
        }
    }
}

//--------------------------------------------------------------------
void Settings::run()
{
    if (_sim_device->checkSMS())
    {
        if (DEBUG)
        {
            Serial.println(_sim_device->smsBuffer);
        }
        char input[MAX_INPUT_LENGTH] = "{\"Tcheck\":15,\"MAX_ERRORS\":3,\"TintB\":360,\"TsendB\":10,\"TGPSB\":10,\"SMART\":true,\"TGPS\":10,\"Tint\":60,\"Tsend\":10}";

        StaticJsonDocument<96> doc;

        DeserializationError error = deserializeJson(doc, input, MAX_INPUT_LENGTH);
        if (error)
        {
            if (DEBUG)
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.c_str());
            }
            return;
        }

        int configs_MAX_ERRORS = doc["MAX_ERRORS"]; // 3
        if (configs_MAX_ERRORS)
        {
            if (DEBUG)
            {
                Serial.print("MAX_ERRORS: ");
                Serial.println(configs_MAX_ERRORS);
            }
            setMAX_ERRORS(configs_MAX_ERRORS);
        }

        int configs_TintB = doc["TintB"]; // 360
        if (configs_TintB)
        {
            if (DEBUG)
            {
                Serial.print("configs_TintB: ");
                Serial.println(configs_TintB);
            }
            setMAX_ERRORS(configs_TintB);
        }

        int configs_TsendB = doc["TsendB"]; // 10
        if (configs_TsendB)
        {
            if (DEBUG)
            {
                Serial.print("configs_TsendB: ");
                Serial.println(configs_TsendB);
            }
            setMAX_ERRORS(configs_TsendB);
        }

        int configs_TGPSB = doc["TGPSB"]; // 10
        if (configs_TGPSB)
        {
            if (DEBUG)
            {
                Serial.print("configs_TGPSB: ");
                Serial.println(configs_TGPSB);
            }
            setMAX_ERRORS(configs_TGPSB);
        }

        bool configs_SMART = doc["SMART"]; // true
        if (configs_SMART)
        {
            if (DEBUG)
            {
                Serial.print("configs_SMART: ");
                Serial.println(configs_SMART);
            }
            setMAX_ERRORS(configs_SMART);
        }

        int configs_TGPS = doc["TGPS"]; // 10
        if (configs_TGPS)
        {
            if (DEBUG)
            {
                Serial.print("configs_TGPS: ");
                Serial.println(configs_TGPS);
            }
            setMAX_ERRORS(configs_TGPS);
        }

        int configs_Tint = doc["Tint"]; // 60
        if (configs_Tint)
        {
            if (DEBUG)
            {
                Serial.print("configs_Tint: ");
                Serial.println(configs_Tint);
            }
            setMAX_ERRORS(configs_Tint);
        }

        int configs_Tsend = doc["Tsend"]; // 10
        if (configs_Tsend)
        {
            if (DEBUG)
            {
                Serial.print("configs_Tsend: ");
                Serial.println(configs_Tsend);
            }
            setMAX_ERRORS(configs_Tsend);
        }
    }
}

//--------------------------------------------------------------------
void Settings::setMAX_ERRORS(int val)
{
    if (MAX_ERRORS != val)
    {
        MAX_ERRORS = val;
        EEPROM.write(MAX_ERRORS_ADDR, val); // save the value in eeprom
    }
}

//--------------------------------------------------------------------
void Settings::setTint(unsigned int val)
{
    if (Tint != val)
    {
        Tint = val;
        byte hiByte = highByte(val);
        byte loByte = lowByte(val);
        EEPROM.write(Tint_ADDR, hiByte);
        EEPROM.write(Tint_ADDR + 1, loByte);
    }
}

//--------------------------------------------------------------------
void Settings::setTintB(unsigned int val)
{
    if (TintB != val)
    {
        TintB = val;
        byte hiByte = highByte(val);
        byte loByte = lowByte(val);
        EEPROM.write(TintB_ADDR, hiByte);
        EEPROM.write(TintB_ADDR + 1, loByte);
    }
}

//--------------------------------------------------------------------
void Settings::setTGPS(int val)
{
    if (TGPS != val)
    {
        TGPS = val;
        EEPROM.write(TGPS_ADDR, val); // save the value in eeprom
    }
}

//--------------------------------------------------------------------
void Settings::setTGPSB(int val)
{
    if (TGPSB != val)
    {
        TGPSB = val;
        EEPROM.write(TGPSB_ADDR, val); // save the value in eeprom
    }
}

//--------------------------------------------------------------------
void Settings::setSMART(int val)
{
    if (SMART != val)
    {
        SMART = val;
        EEPROM.write(SMART_ADDR, val); // save the value in eeprom
    }
}

//--------------------------------------------------------------------
void Settings::setTsendB(int val)
{
    if (TsendB != val)
    {
        TsendB = val;
        EEPROM.write(TsendB_ADDR, val); // save the value in eeprom
    }
}

//--------------------------------------------------------------------
void Settings::setTsend(int val)
{
    if (Tsend != val)
    {
        Tsend = val;
        EEPROM.write(Tsend_ADDR, val); // save the value in eeprom
    }
}
