#include "fsm.h"

// Constructor
FSM::FSM()
{
    state = ENERGY;
}

//--------------------------------------------------------------------
void FSM::setup(int pin, GPS *gps)
{
    _gps = gps;

    // Pin for 12V check
    pin12V = pin;
    pinMode(pin12V, INPUT);

    byte key = EEPROM.read(KEY_ADDR); // read the first byte from the EEPROM
    if (key == EEPROM_KEY || key == (EEPROM_KEY + 1))
    {
        // here if the key value read matches the value saved when writing eeprom
        if (DEBUG)
        {
            Serial.println("Using data from EEPROM");
        }
        Tcheck = EEPROM.read(Tcheck_ADDR);
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
        setTcheck(iTcheck);
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
    if (DEBUG)
    {
        Serial.print("Tcheck: ");
        Serial.println(Tcheck);
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

//--------------------------------------------------------------------
void FSM::run()
{
    switch (state)
    {
    // ------------------------------------------
    case IDLE:
        if (millis() - lastCheck > Tcheck)
        { // Check energy
            lastCheck = millis();
            state = ENERGY;
        }
        break;
    // ------------------------------------------
    case ENERGY:
        if (digitalRead(pin12V))
        { // 12V connected
            if (millis() - lastInterval > Tint)
            { // Beging location update
                lastInterval = millis();
                startGPS = lastInterval;
                state = READ_GPS;
            }
            else
            {
                // Back to IDLE state
                state = IDLE;
            }
        }
        else
        { // 12V disconnected
            state = SLEEP;
        }
        break;
    // ------------------------------------------
    case READ_GPS:
        if (_gps->run() == VALID_LOCATION)
        { // GPS data ready
            state = SEND_DATA;
            if (DEBUG)
            {
                Serial.print(_gps->lastLat, 6);
                Serial.print(", ");
                Serial.print(_gps->lastLon, 6);
                Serial.print(", ");
                _gps->detectNewPosition();
            }
        }
        else
        {
            if (millis() - startGPS > TGPS)
            { // No GPS data in the time window allowed
                if (_gps->pendingData)
                { // Old data that haven't been sent
                    state = SEND_DATA;
                }
                else
                { // No data to be send
                    state = ERROR;
                }
            }
            else
            {
                if (DEBUG)
                {
                    Serial.println("...");
                }
            }
        }
        break;

    default:
        break;
    }
}

//--------------------------------------------------------------------
void FSM::setTcheck(int val)
{
    if (Tcheck != val)
    {
        Tcheck = val;
        EEPROM.write(Tcheck_ADDR, val); // save the value in eeprom
    }
}

//--------------------------------------------------------------------
void FSM::setMAX_ERRORS(int val)
{
    if (MAX_ERRORS != val)
    {
        MAX_ERRORS = val;
        EEPROM.write(MAX_ERRORS_ADDR, val); // save the value in eeprom
    }
}

//--------------------------------------------------------------------
void FSM::setTint(unsigned int val)
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
void FSM::setTintB(unsigned int val)
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
void FSM::setTGPS(int val)
{
    if (TGPS != val)
    {
        TGPS = val;
        EEPROM.write(TGPS_ADDR, val); // save the value in eeprom
    }
}

//--------------------------------------------------------------------
void FSM::setTGPSB(int val)
{
    if (TGPSB != val)
    {
        TGPSB = val;
        EEPROM.write(TGPSB_ADDR, val); // save the value in eeprom
    }
}

//--------------------------------------------------------------------
void FSM::setSMART(int val)
{
    if (SMART != val)
    {
        SMART = val;
        EEPROM.write(SMART_ADDR, val); // save the value in eeprom
    }
}

//--------------------------------------------------------------------
void FSM::setTsendB(int val)
{
    if (TsendB != val)
    {
        TsendB = val;
        EEPROM.write(TsendB_ADDR, val); // save the value in eeprom
    }
}

//--------------------------------------------------------------------
void FSM::setTsend(int val)
{
    if (Tsend != val)
    {
        Tsend = val;
        EEPROM.write(Tsend_ADDR, val); // save the value in eeprom
    }
}
