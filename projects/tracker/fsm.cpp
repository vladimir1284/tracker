#include "fsm.h"
#include "sim.h"

// Constructor
FSM::FSM()
{
    state = currentState;
    lastInterval = currentLastInterval;
}

//--------------------------------------------------------------------
void FSM::setup(int pin, GPS *gps, SIM *sim)
{
    _gps = gps;
    _sim = sim;

    gpsErrors = 0;
    gsmErrors = 0;

    // Pin for 12V check
    pin12V = pin;
    pinMode(pin12V, INPUT);

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

    EEPROM.end();

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
    // wdt_reset(); //Reset the watchdog
    switch (state)
    {
    // ------------------------------------------
    case IDLE:
        if (getMillis() - lastCheck > ((unsigned long)Tcheck * MIN2MILLIS))
        { // Check energy
            lastCheck = getMillis();
            state = ENERGY;
            if (DEBUG)
            {
                Serial.print(getMillis());
                Serial.println("-> State: Energy");
            }
        }
        break;
    // ------------------------------------------
    case ENERGY:
        if (false) //digitalRead(pin12V)) // TODO: read input
        {          // 12V connected
            if (getMillis() - lastInterval > ((unsigned long)Tint * MIN2MILLIS))
            { // Beging location update
                lastInterval = getMillis();
                stateChange = lastInterval;
                state = READ_GPS;
                if (DEBUG)
                {
                    Serial.print(getMillis());
                    Serial.println("-> State: READ_GPS");
                }
            }
            else
            {
                // Back to IDLE state
                state = IDLE;
                if (DEBUG)
                {
                    Serial.print(getMillis());
                    Serial.println("-> State: IDLE");
                }
            }
        }
        else
        { // 12V disconnected
            // TODO: Turn off GPS and GSM
            state = SLEEPING;
            if (DEBUG)
            {
                Serial.print(getMillis());
                Serial.println("-> State: SLEEPING");
            }
        }
        break;

    // ------------------------------------------
    case READ_GPS:
        if (_gps->run() == VALID_LOCATION)
        { // GPS data ready
            stateChange = getMillis();
            state = SEND_DATA;
            if (DEBUG)
            {
                Serial.print(_gps->lastLat, 6);
                Serial.print(", ");
                Serial.print(_gps->lastLon, 6);
                Serial.print(", ");
                _gps->detectNewPosition();
                Serial.print(getMillis());
                Serial.println("-> State: SEND_DATA");
            }
        }
        else
        {
            if (getMillis() - stateChange > ((unsigned long)TGPS * MIN2MILLIS))
            { // No GPS data in the time window allowed
                gpsErrors++;

                if (_gps->pendingData)
                { // Old data that haven't been sent
                    stateChange = getMillis();
                    state = SEND_DATA;
                    if (DEBUG)
                    {
                        Serial.print(getMillis());
                        Serial.println("-> State: SEND_DATA (old)");
                    }
                }
                else
                { // No data to be send
                    state = ERROR;
                    if (DEBUG)
                    {
                        Serial.print(getMillis());
                        Serial.println("-> State: ERROR");
                    }
                }
            }
        }
        break;

    // ------------------------------------------
    case SEND_DATA:
        if (_sim->uploadData(_gps->lastLat, _gps->lastLon, true))
        { // Data sent
            state = IDLE;
            if (DEBUG)
            {
                Serial.print(getMillis());
                Serial.println("-> State: IDLE");
            }
        }
        else
        {
            if (getMillis() - stateChange > ((unsigned long)Tsend * MIN2MILLIS))
            { // Data upload not achieved
                gsmErrors++;
                state = ERROR;
                if (DEBUG)
                {
                    Serial.print(getMillis());
                    Serial.println("-> State: ERROR");
                }
            }
        }
        break;

    // ------------------------------------------
    case ERROR:
        if (gsmErrors > MAX_ERRORS)
        {
            gsmErrors = 0;
            // TODO: restart GSM
        }
        if (gpsErrors > MAX_ERRORS)
        {
            gpsErrors = 0;
            // TODO: restart GPS
        }
        state = IDLE;
        if (DEBUG)
        {
            Serial.print("GSM errors: ");
            Serial.println(gsmErrors);
            Serial.print("GPS errors: ");
            Serial.println(gpsErrors);
            Serial.print(getMillis());
            Serial.println("-> State: IDLE");
        }
        break;

    // ------------------------------------------
    case SLEEPING:
        esp_sleep_enable_timer_wakeup((unsigned long)Tcheck * uS_TO_S_FACTOR);

         // ----- save state before sleeping ----
        currentState = BATTERY;
        currentLastInterval = lastInterval;
        // This will be the offset on wake up
        millisOffset = getMillis() + (unsigned long)Tcheck * MIN2MILLIS;
        // ----------------------------------------
        
        // Going to sleep
        esp_deep_sleep_start();
        break;

    // ------------------------------------------
    case BATTERY:
        if (DEBUG)
        {
            Serial.print(getMillis());
            Serial.println("-> State: BATTERY");
        }
        if (false) //digitalRead(pin12V))
        {          // 12V connected
            // TODO: Turn on GPS and GSM
            lastInterval = getMillis();
            stateChange = lastInterval;
            state = READ_GPS;
            if (DEBUG)
            {
                Serial.print(getMillis());
                Serial.println("-> State: READ_GPS");
            }
        }
        else
        {
            if (getMillis() - lastInterval > ((unsigned long)TintB * MIN2MILLIS))
            { // Beging location update on battery mode
                // TODO: Turn on GPS
                lastInterval = getMillis();
                stateChange = lastInterval;
                state = BAT_GPS;
                if (DEBUG)
                {
                    Serial.print(getMillis());
                    Serial.println("-> State: BAT_GPS");
                }
            }
            else
            {
                // Back to SLEEPING state
                state = SLEEPING;
                if (DEBUG)
                {
                    Serial.print(getMillis());
                    Serial.println("-> State: SLEEPING");
                }
            }
        }
        break;

    // ------------------------------------------
    case BAT_GPS:
        if (_gps->run() == VALID_LOCATION)
        { // GPS data ready
            // TODO: turn off GPS
            if (!SMART || _gps->detectNewPosition())
            {
                // TODO: turn on GSM
                stateChange = getMillis();
                state = BAT_SEND;
                if (DEBUG)
                {
                    Serial.print(_gps->lastLat, 6);
                    Serial.print(", ");
                    Serial.print(_gps->lastLon, 6);
                    Serial.print(", ");
                    _gps->detectNewPosition();
                    Serial.print(getMillis());
                    Serial.println("-> State: BAT_SEND");
                }
            }
            else
            {
                state = SLEEPING;
                if (DEBUG)
                {
                    Serial.print(getMillis());
                    Serial.println("-> State: SLEEPING");
                }
            }
        }
        else
        {
            if (getMillis() - stateChange > ((unsigned long)TGPSB * MIN2MILLIS))
            { // No GPS data in the time window allowed
                gpsErrors++;
                // TODO: turn off GPS
                if (_gps->pendingData)
                { // Old data that haven't been sent
                    // TODO: turn on GSM
                    stateChange = getMillis();
                    state = BAT_SEND;
                    if (DEBUG)
                    {
                        Serial.print(getMillis());
                        Serial.println("-> State: BAT_SEND (old)");
                    }
                }
                else
                { // No data to be send
                    state = BAT_ERROR;
                    if (DEBUG)
                    {
                        Serial.print(getMillis());
                        Serial.println("-> State: BAT_ERROR");
                    }
                }
            }
        }
        break;

    // ------------------------------------------
    case BAT_SEND:
        if (_sim->uploadData(_gps->lastLat, _gps->lastLon, false))
        { // Data sent
            // TODO: turn off GSM
            state = SLEEPING;
            if (DEBUG)
            {
                Serial.print(getMillis());
                Serial.println("-> State: SLEEPING");
            }
        }
        else
        {
            if (getMillis() - stateChange > ((unsigned long)TsendB * MIN2MILLIS))
            { // Data upload not achieved
                // TODO: turn off GSM
                gsmErrors++;
                state = BAT_ERROR;
                if (DEBUG)
                {
                    Serial.print(getMillis());
                    Serial.println("-> State: BAT_ERROR");
                }
            }
        }
        break;

    // ------------------------------------------
    case BAT_ERROR:
        if (gsmErrors > MAX_ERRORS)
        {
            gsmErrors = 0;
            setTsendB(2 * TsendB);
        }
        if (gpsErrors > MAX_ERRORS)
        {
            gpsErrors = 0;
            setTGPSB(2 * TGPSB);
        }
        state = SLEEPING;
        if (DEBUG)
        {
            Serial.print("GSM errors: ");
            Serial.println(gsmErrors);
            Serial.print("GPS errors: ");
            Serial.println(gpsErrors);
            Serial.print(getMillis());
            Serial.println("-> State: SLEEPING");
        }
        break;

    // ---------------------------------------------------------
    default:
        // We shouldn't be here!
        state = IDLE;
        if (DEBUG)
        {
            Serial.println("Undefined STATE!!!");
            Serial.print(getMillis());
            Serial.println("-> State: IDLE");
        }
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
