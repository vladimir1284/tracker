#include "fsm_power_on.h"

// Constructor
FSMpower::FSMpower()
{
}

//--------------------------------------------------------------------
void FSMpower::setup(Sim7000 *sim_device)
{
    _sim_device = sim_device;
    tries = 3;
}

//--------------------------------------------------------------------
void FSMpower::run()
{
    time_t now;
    timerWrite(timer, 0); //reset timer (feed watchdog)
    switch (state)
    {
    // ------------------------------------------
    case IDLE:
        time(&now);
        if (now - lastInterval > ((time_t)Tint * MIN_TO_S_FACTOR))
        { // Beging location update
            lastInterval = now;
            stateChange = lastInterval;
            state = READ_GPS;

            // Enable GPS
            tries = 3; // After 3 unsuccessful attemps the module is restarted
            while (!fona.enableGPS(true))
            {
                if (--tries < 0)
                {
                    tries = 3; // Back to its original value
                    _sim_device->reset();
                    if (DEBUG)
                    {
                        Serial.println(F("Reseting the SIM module..."));
                    }
                }
                if (DEBUG)
                {
                    Serial.println(F("Failed to turn on GPS, retrying..."));
                }
                timerWrite(timer, 0); //reset timer (feed watchdog)
                delay(2000);          // Retry every 2s
            }
            tries = 3; // Back to its original value
            if (DEBUG)
            {
                Serial.print(now);
                Serial.println("-> State: READ_GPS");
            }
        }
        break;

    // ------------------------------------------
    case READ_GPS:
        time(&now);
        if (now - stateChange > ((time_t)TGPS * MIN_TO_S_FACTOR))
        { // No GPS data in the time window allowed
            gpsErrors++;

            if (pending)
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

        if (!fona.GPSstatus())
        {
            // Enable GPS
            if (!fona.enableGPS(true))
            {
                if (--tries < 0)
                {
                    _sim_device->reset();
                    tries = 3; // Back to its original value
                    if (DEBUG)
                    {
                        Serial.println(F("Reseting the SIM module..."));
                    }
                }
                if (DEBUG)
                {
                    Serial.println(F("Failed to turn on GPS, retrying..."));
                }
                timerWrite(timer, 0); //reset timer (feed watchdog)
                delay(2000);          // Retry every 2s
                break;
            }
            tries = 3; // Back to its original value
            Serial.println(F("Turned on GPS!"));
        }

        if (!_sim_device->prepareMessage())
        {
            // if (--tries < 0)
            // {
            //     _sim_device->reset();
            //     tries = 3; // Back to its original value
            //     if (DEBUG)
            //     {
            //         Serial.println(F("Reseting the SIM module..."));
            //     }
            // }
            if (DEBUG)
            {
                Serial.println(F("Failed to get GPS location, retrying..."));
            }
            timerWrite(timer, 0); //reset timer (feed watchdog)
            delay(2000);          // Retry every 2s
            break;
        }
        else
        {
            // tries = 3; // Back to its original value

            // GPS data ready
            time(&now);
            stateChange = now;
            state = SEND_DATA;
        }
        break;

    // ------------------------------------------
    case SEND_DATA:
        time(&now);
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

        fona.setFunctionality(1); // AT+CFUN=1

        if (!_sim_device->netStatus())
        {
            if (--tries < 0)
            {
                _sim_device->reset();
                tries = 3; // Back to its original value
                if (DEBUG)
                {
                    Serial.println(F("Reseting the SIM module..."));
                }
            }
            if (DEBUG)
            {
                Serial.println(F("Failed to connect to cell network, retrying..."));
            }
            timerWrite(timer, 0); //reset timer (feed watchdog)
            delay(2000);          // Retry every 2s
            break;
        }
        tries = 3; // Back to its original value
        if (DEBUG)
        {
            Serial.println(F("Connected to cell network!"));
        }
        if (!_sim_device->uploadData())
        {
            if (--tries < 0)
            {
                _sim_device->reset();
                tries = 3; // Back to its original value
                if (DEBUG)
                {
                    Serial.println(F("Reseting the SIM module..."));
                }
            }
            if (DEBUG)
            {
                Serial.println(F("Failed to send data, retrying..."));
            }
            timerWrite(timer, 0); //reset timer (feed watchdog)
            delay(2000);          // Retry every 2s
            break;
        }
        else
        {   
            // Data sent
            tries = 3; // Back to its original value
            state = IDLE;
            pending = false;
            if (DEBUG)
            {
                Serial.print(now);
                Serial.println("-> State: IDLE");
            }
        }
        break;

    // ------------------------------------------
    case ERROR:
        if (gsmErrors > MAX_ERRORS || gpsErrors > MAX_ERRORS)
        {
            gpsErrors = 0;
            gsmErrors = 0;
            _sim_device->reset();
            if (DEBUG)
            {
                Serial.println(F("Reseting the SIM module..."));
            }
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
