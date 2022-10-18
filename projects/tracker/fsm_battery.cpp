#include "fsm_battery.h"

// Constructor
FSMbattery::FSMbattery()
{
}

//--------------------------------------------------------------------
void FSMbattery::setup(Sim7000 *sim_device)
{
    time_t now;
    time(&now);
    stateChange = now;

    _sim_device = sim_device;
    tries = RETRIES;
}

//--------------------------------------------------------------------
void FSMbattery::run()
{
    time_t now;
    timerWrite(timer, 0); // reset timer (feed watchdog)
    switch (state)
    {
    // ------------------------------------------
    case IDLE:
        setState(READ_GPS);
        _sim_device->powerOFF();
        rtc_sleep(TintB * MIN_TO_uS_FACTOR);
        break;

    // ------------------------------------------
    case READ_GPS:
        if (moving)
        {
            time(&now);
            if (now - stateChange > (TGPS * MIN_TO_S_FACTOR))
            { // No GPS data in the time window allowed
                gpsErrors++;

                if (pending)
                { // Old data that haven't been sent
                    setState(SEND_DATA);
                }
                else
                { // No data to be send
                    setState(ERROR);
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
                        tries = RETRIES; // Back to its original value
                        if (DEBUG)
                        {
                            Serial.println(F("Reseting the SIM module..."));
                        }
                    }
                    if (DEBUG)
                    {
                        Serial.println(F("Failed to turn on GPS, retrying..."));
                    }
                    timerWrite(timer, 0); // reset timer (feed watchdog)
                    rtc_light_sleep(2);   // Retry every 2s
                    break;
                }
                tries = RETRIES; // Back to its original value
                Serial.println(F("Turned on GPS!"));
            }

            if (!_sim_device->prepareMessage())
            {
                if (DEBUG)
                {
                    Serial.println(F("Failed to get GPS location, retrying..."));
                }
                timerWrite(timer, 0); // reset timer (feed watchdog)
                rtc_light_sleep(5);   // Retry every 1min
                break;
            }
            else // Message ready
            {
                // GPS data ready
                setState(SEND_DATA);
            }
        }
        else // Not moving
        {
            // GPS data ready
            if (DEBUG)
            {
                Serial.println(F("Not moving! Avoiding GPS activation!"));
            }
            setState(SEND_DATA);
        }

        break;

    // ------------------------------------------
    case SEND_DATA:
        time(&now);
        if (now - stateChange > (Tsend * MIN_TO_S_FACTOR))
        {                      // Data upload not achieved
            set_handler.run(); // check SMS
            gsmErrors++;
            setState(ERROR);
        }

        fona.setFunctionality(1); // AT+CFUN=1

        if (!_sim_device->netStatus())
        {
            if (--tries < 0)
            {
                _sim_device->reset();
                tries = RETRIES; // Back to its original value
                if (DEBUG)
                {
                    Serial.println(F("Reseting the SIM module..."));
                }
            }
            if (DEBUG)
            {
                Serial.println(F("Failed to connect to cell network, retrying..."));
            }
            timerWrite(timer, 0); // reset timer (feed watchdog)
            rtc_light_sleep(2);   // Retry every 2s
            break;
        }
        tries = RETRIES; // Back to its original value
        if (DEBUG)
        {
            Serial.println(F("Connected to cell network!"));
        }
        if (!_sim_device->uploadData())
        {
            if (--tries < 0)
            {
                _sim_device->reset();
                tries = RETRIES; // Back to its original value
                if (DEBUG)
                {
                    Serial.println(F("Reseting the SIM module..."));
                }
            }
            if (DEBUG)
            {
                Serial.println(F("Failed to send data, retrying..."));
            }
            timerWrite(timer, 0); // reset timer (feed watchdog)
            rtc_light_sleep(2);   // Retry every 2s
            break;
        }
        else
        {
            // Data sent
            tries = RETRIES; // Back to its original value
            setState(IDLE);
            pending = false;
            set_handler.run(); // check SMS
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
        setState(IDLE);
        if (DEBUG)
        {
            Serial.print("GSM errors: ");
            Serial.println(gsmErrors);
            Serial.print("GPS errors: ");
            Serial.println(gpsErrors);
        }
        break;

    // ---------------------------------------------------------
    default:
        // We shouldn't be here!
        if (DEBUG)
        {
            Serial.println("Undefined STATE!!!");
        }
        setState(IDLE);
        break;
    }
}

//--------------------------------------------------------------------
void FSMbattery::setState(states newState)
{
    String state_str;
    time_t now;

    time(&now);
    stateChange = now;
    state = newState;
    switch (newState)
    {
    case IDLE:
        state_str = "IDLE";
        break;
    case READ_GPS:
        state_str = "READ_GPS";
        break;
    case SEND_DATA:
        state_str = "SEND_DATA";
        break;
    case ERROR:
        state_str = "ERROR";
        break;
    default:
        state_str = "UNKNOWN";
        break;
    }
    if (DEBUG)
    {
        Serial.print(now);
        Serial.print("-> State: ");
        Serial.println(state_str);
    }
}
