#include "EEPROMsettings.h"

// Constructor
Settings::Settings()
{
}

//--------------------------------------------------------------------
void Settings::setup()
{
    if (TGPS == 0)
    {
        // Init EEPROM
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
            Mode = EEPROM.read(Mode_ADDR);

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
            setMode(iMode);
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
            Serial.print("Mode: ");
            Serial.println(Mode);
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
void Settings::processConfigs(String json_str)
{

    if (DEBUG)
    {
        Serial.println(json_str);
    }
    StaticJsonDocument<96> doc;

    DeserializationError error = deserializeJson(doc, json_str.c_str(), MAX_INPUT_LENGTH);
    if (error)
    {
        if (DEBUG)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
        }
        return;
    }

    // Init EEPROM
    EEPROM.begin(EEPROM_SIZE);

    int configs_Mode = doc["Mode"]; // 3
    if (configs_Mode)
    {
        if (DEBUG)
        {
            Serial.print("Mode: ");
            Serial.println(configs_Mode);
        }
        setMode(configs_Mode);
    }

    int configs_TintB = doc["TintB"]; // 360
    if (configs_TintB)
    {
        if (DEBUG)
        {
            Serial.print("configs_TintB: ");
            Serial.println(configs_TintB);
        }
        setTintB(configs_TintB);
    }

    int configs_TsendB = doc["TsendB"]; // 10
    if (configs_TsendB)
    {
        if (DEBUG)
        {
            Serial.print("configs_TsendB: ");
            Serial.println(configs_TsendB);
        }
        setTsendB(configs_TsendB);
    }

    int configs_TGPSB = doc["TGPSB"]; // 10
    if (configs_TGPSB)
    {
        if (DEBUG)
        {
            Serial.print("configs_TGPSB: ");
            Serial.println(configs_TGPSB);
        }
        setTGPSB(configs_TGPSB);
    }

    bool configs_SMART = doc["SMART"]; // true
    if (configs_SMART)
    {
        if (DEBUG)
        {
            Serial.print("configs_SMART: ");
            Serial.println(configs_SMART);
        }
        setSMART(configs_SMART);
    }

    int configs_TGPS = doc["TGPS"]; // 10
    if (configs_TGPS)
    {
        if (DEBUG)
        {
            Serial.print("configs_TGPS: ");
            Serial.println(configs_TGPS);
        }
        setTGPS(configs_TGPS);
    }

    int configs_Tint = doc["Tint"]; // 60
    if (configs_Tint)
    {
        if (DEBUG)
        {
            Serial.print("configs_Tint: ");
            Serial.println(configs_Tint);
        }
        setTint(configs_Tint);
    }

    int configs_Tsend = doc["Tsend"]; // 10
    if (configs_Tsend)
    {
        if (DEBUG)
        {
            Serial.print("configs_Tsend: ");
            Serial.println(configs_Tsend);
        }
        setTsend(configs_Tsend);
    }

    EEPROM.end();
}

//--------------------------------------------------------------------
void Settings::setMode(int val)
{
    if (Mode != val)
    {
        Mode = (byte)val;
        EEPROM.write(Mode_ADDR, val); // save the value in eeprom
        if (DEBUG)
        {
            Serial.print("Written Mode to EEPROM! val: ");
            Serial.println(val);
        }
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
        if (DEBUG)
        {
            Serial.print("Written Tint to EEPROM! val: ");
            Serial.println(val);
        }
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
        if (DEBUG)
        {
            Serial.print("Written TintB to EEPROM! val: ");
            Serial.println(val);
        }
    }
}

//--------------------------------------------------------------------
void Settings::setTGPS(int val)
{
    if (TGPS != val)
    {
        TGPS = val;
        EEPROM.write(TGPS_ADDR, val); // save the value in eeprom
        if (DEBUG)
        {
            Serial.print("Written TGPS to EEPROM! val: ");
            Serial.println(val);
        }
    }
}

//--------------------------------------------------------------------
void Settings::setTGPSB(int val)
{
    if (TGPSB != val)
    {
        TGPSB = val;
        EEPROM.write(TGPSB_ADDR, val); // save the value in eeprom
        if (DEBUG)
        {
            Serial.print("Written TGPSB to EEPROM! val: ");
            Serial.println(val);
        }
    }
}

//--------------------------------------------------------------------
void Settings::setSMART(int val)
{
    if (SMART != val)
    {
        SMART = val;
        EEPROM.write(SMART_ADDR, val); // save the value in eeprom
        if (DEBUG)
        {
            Serial.print("Written SMART to EEPROM! val: ");
            Serial.println(val);
        }
    }
}

//--------------------------------------------------------------------
void Settings::setTsendB(int val)
{
    if (TsendB != val)
    {
        TsendB = val;
        EEPROM.write(TsendB_ADDR, val); // save the value in eeprom
        if (DEBUG)
        {
            Serial.print("Written TsendB to EEPROM! val: ");
            Serial.println(val);
        }
    }
}

//--------------------------------------------------------------------
void Settings::setTsend(int val)
{
    if (Tsend != val)
    {
        Tsend = val;
        EEPROM.write(Tsend_ADDR, val); // save the value in eeprom
        if (DEBUG)
        {
            Serial.print("Written Tsend to EEPROM! val: ");
            Serial.println(val);
        }
    }
}
