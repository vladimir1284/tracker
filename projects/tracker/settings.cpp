#include "settings.h"

// Constructor
Settings::Settings()
{
}

//--------------------------------------------------------------------
void Settings::setup()
{
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
            // Serial.print("Tcheck: ");
            // Serial.println(Tcheck);
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

}

// //--------------------------------------------------------------------
// void Settings::setTcheck(int val)
// {
//     if (Tcheck != val)
//     {
//         Tcheck = val;
//         EEPROM.write(Tcheck_ADDR, val); // save the value in eeprom
//     }
// }

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

// void Sim7000::updateTrackerID(int value)
// {
//     if (trackerID != value)
//     {
//         //Init EEPROM
//         EEPROM.begin(EEPROM_SIZE);

//         trackerID = value;
//         byte hiByte = highByte(value);
//         byte loByte = lowByte(value);
//         EEPROM.write(trackerID_ADDR, hiByte);
//         EEPROM.write(trackerID_ADDR + 1, loByte);
//         EEPROM.write(KEY_ADDR, EEPROM_KEY + 1); // write the Key to indicate valid ID data
//         if (DEBUG)
//         {
//             Serial.print("Tracker ID from remote server: ");
//             Serial.println(value);
//         }

//         EEPROM.end();
//     }
// }

// void Sim7000::updateTcheck(int value)
// {
//     if (value > 0)
//     {
//         if (DEBUG)
//         {
//             Serial.print("Tcheck: ");
//             Serial.println(value);
//         }
//         _fsm->setTcheck(value);
//     }
// }
// void Sim7000::updateMAX_ERRORS(int value)
// {
//     if (value > 0)
//     {
//         if (DEBUG)
//         {
//             Serial.print("MAX_ERRORS: ");
//             Serial.println(value);
//         }
//         _fsm->setMAX_ERRORS(value);
//     }
// }
// void Sim7000::updateTintB(int value)
// {
//     if (value > 0)
//     {
//         if (DEBUG)
//         {
//             Serial.print("TintB: ");
//             Serial.println(value);
//         }
//         _fsm->setTintB(value);
//     }
// }
// void Sim7000::updateTsendB(int value)
// {
//     if (value > 0)
//     {
//         if (DEBUG)
//         {
//             Serial.print("TsendB: ");
//             Serial.println(value);
//         }
//         _fsm->setTsendB(value);
//     }
// }
// void Sim7000::updateTGPSB(int value)
// {
//     if (value > 0)
//     {
//         if (DEBUG)
//         {
//             Serial.print("TGPSB: ");
//             Serial.println(value);
//         }
//         _fsm->setTGPSB(value);
//     }
// }
// void Sim7000::updateSMART(int value)
// {
//     if (value > -1)
//     {
//         if (DEBUG)
//         {
//             Serial.print("SMART: ");
//             Serial.println(value);
//         }
//         _fsm->setSMART(value);
//     }
// }
// void Sim7000::updateTGPS(int value)
// {
//     if (value > 0)
//     {
//         if (DEBUG)
//         {
//             Serial.print("TGPS: ");
//             Serial.println(value);
//         }
//         _fsm->setTGPS(value);
//     }
// }
// void Sim7000::updateTint(int value)
// {
//     if (value > 0)
//     {
//         if (DEBUG)
//         {
//             Serial.print("Tint: ");
//             Serial.println(value);
//         }
//         _fsm->setTint(value);
//     }
// }

// void Sim7000::updateTsend(int value)
// {
//     if (value > 0)
//     {
//         if (DEBUG)
//         {
//             Serial.print("Tsend: ");
//             Serial.println(value);
//         }
//         _fsm->setTsend(value);
//     }
// }