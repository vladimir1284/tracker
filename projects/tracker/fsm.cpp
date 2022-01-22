#include "fsm.h"
#include "sim.h"

// Constructor
FSM::FSM()
{
}

//--------------------------------------------------------------------
void FSM::setup(int pin, GPS *gps, SIM *sim)
{
    _gps = gps;
    _sim = sim;

    // Pin for 12V check
    pin12V = pin;
    pinMode(pin12V, INPUT);

    if (Tcheck == 0)
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
}

//--------------------------------------------------------------------
void FSM::run()
{
    time_t now;
    time(&now);

    switch (state)
    {
    // ------------------------------------------
    case IDLE:
        if (now - lastCheck > ((time_t)Tcheck * MIN_TO_S_FACTOR))
        { // Check energy
            lastCheck = now;
            state = ENERGY;
            if (DEBUG)
            {
                Serial.print(now);
                Serial.println("-> State: Energy");
            }
        }
        break;
    // ------------------------------------------
    case ENERGY:
        if (false) //digitalRead(pin12V)) // TODO: read input
        {          // 12V connected
            if (now - lastInterval > ((time_t)Tint * MIN_TO_S_FACTOR))
            { // Beging location update
                lastInterval = now;
                stateChange = lastInterval;
                state = READ_GPS;
                if (DEBUG)
                {
                    Serial.print(now);
                    Serial.println("-> State: READ_GPS");
                }
            }
            else
            {
                // Back to IDLE state
                state = IDLE;
                if (DEBUG)
                {
                    Serial.print(now);
                    Serial.println("-> State: IDLE");
                }
            }
        }
        else
        { // 12V disconnected
            // TODO: Turn off GPS and GSM
            _gps->turnOff();
            _sim->turnOff();
            state = SLEEPING;
            if (DEBUG)
            {
                Serial.print(now);
                Serial.println("-> State: SLEEPING");
            }
        }
        break;

    // ------------------------------------------
    case READ_GPS:
        if (_gps->run() == VALID_LOCATION)
        { // GPS data ready
            stateChange = now;
            state = SEND_DATA;
            if (DEBUG)
            {
                Serial.print(now);
                Serial.println("-> State: SEND_DATA");
            }
        }
        else
        {
            if (now - stateChange > ((time_t)TGPS * MIN_TO_S_FACTOR))
            { // No GPS data in the time window allowed
                gpsErrors++;

                if (gpsData.pending)
                { // Old data that haven't been sent
                    stateChange = now;
                    state = SEND_DATA;
                    if (DEBUG)
                    {
                        Serial.print(now);
                        Serial.println("-> State: SEND_DATA (old)");
                    }
                }
                else
                { // No data to be send
                    state = ERROR;
                    if (DEBUG)
                    {
                        Serial.print(now);
                        Serial.println("-> State: ERROR");
                    }
                }
            }
        }
        break;

    // ------------------------------------------
    case SEND_DATA:
        if (_sim->uploadData(true))
        { // Data sent
            state = IDLE;
            if (DEBUG)
            {
                Serial.print(now);
                Serial.println("-> State: IDLE");
            }
        }
        else
        {
            if (now - stateChange > ((time_t)Tsend * MIN_TO_S_FACTOR))
            { // Data upload not achieved
                gsmErrors++;
                state = ERROR;
                if (DEBUG)
                {
                    Serial.print(now);
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
            Serial.print(now);
            Serial.println("-> State: IDLE");
        }
        break;

    // ------------------------------------------
    case SLEEPING:
        // ----- save state before sleeping ----
        state = BATTERY;
        rtc_sleep((time_t)Tcheck * MIN_TO_uS_FACTOR);
        break;

    // ------------------------------------------
    case BATTERY:
        if (DEBUG)
        {
            Serial.print(now);
            Serial.println("-> State: BATTERY");
        }
        if (false) //digitalRead(pin12V))
        {          // 12V connected
            _gps->turnOn();
            _sim->turnOn();
            lastInterval = now;
            stateChange = lastInterval;
            state = READ_GPS;
            if (DEBUG)
            {
                Serial.print(now);
                Serial.println("-> State: READ_GPS");
            }
        }
        else
        {
            if (now - lastInterval > ((time_t)TintB * MIN_TO_S_FACTOR / 5))
            { // Beging location update on battery mode
                _gps->turnOn();
                lastInterval = now;
                stateChange = lastInterval;
                state = BAT_GPS;
                if (DEBUG)
                {
                    Serial.print(now);
                    Serial.println("-> State: BAT_GPS");
                }
            }
            else
            {
                // Back to SLEEPING state
                state = SLEEPING;
                if (DEBUG)
                {
                    Serial.print(now);
                    Serial.println("-> State: SLEEPING");
                }
            }
        }
        break;

    // ------------------------------------------
    case BAT_GPS:
        if (_gps->run() == VALID_LOCATION)
        { // GPS data ready
            _gps->turnOff();

            if (!SMART || gpsData.new_pos)
            {
                // TODO: turn on GSM
                stateChange = now;
                state = BAT_SEND;
                if (DEBUG)
                {
                    Serial.print(now);
                    Serial.println("-> State: BAT_SEND");
                }
            }
            else
            {
                state = SLEEPING;
                if (DEBUG)
                {
                    Serial.print(now);
                    Serial.println("-> State: SLEEPING");
                }
            }
        }
        else
        {
            if (now - stateChange > ((time_t)TGPSB * MIN_TO_S_FACTOR))
            { // No GPS data in the time window allowed
                gpsErrors++;
                // TODO: turn off GPS
                _gps->turnOff();
                if (gpsData.pending)
                { // Old data that haven't been sent
                    // TODO: turn on GSM
                    _sim->turnOn();
                    stateChange = now;
                    state = BAT_SEND;
                    if (DEBUG)
                    {
                        Serial.print(now);
                        Serial.println("-> State: BAT_SEND (old)");
                    }
                }
                else
                { // No data to be send
                    state = BAT_ERROR;
                    if (DEBUG)
                    {
                        Serial.print(now);
                        Serial.println("-> State: BAT_ERROR");
                    }
                }
            }
        }
        break;

    // ------------------------------------------
    case BAT_SEND:
        if (_sim->uploadData(false))
        { // Data sent
            _sim->turnOff();
            state = SLEEPING;
            if (DEBUG)
            {
                Serial.print(now);
                Serial.println("-> State: SLEEPING");
            }
        }
        else
        {
            if (now - stateChange > ((time_t)TsendB * MIN_TO_S_FACTOR))
            { // Data upload not achieved
                _sim->turnOff();
                gsmErrors++;
                state = BAT_ERROR;
                if (DEBUG)
                {
                    Serial.print(now);
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
            // Reset GSM
        }
        if (gpsErrors > MAX_ERRORS)
        {
            gpsErrors = 0;
            // Reset GPS
        }
        state = SLEEPING;
        if (DEBUG)
        {
            Serial.print("GSM errors: ");
            Serial.println(gsmErrors);
            Serial.print("GPS errors: ");
            Serial.println(gpsErrors);
            Serial.print(now);
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
            Serial.print(now);
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
