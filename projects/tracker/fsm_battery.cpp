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
    timerWrite(timer, 0); //reset timer (feed watchdog)
    switch (state)
    {
    // ------------------------------------------
    case IDLE:
        state = READ_GPS;
        if (DEBUG)
        {
            Serial.print(now);
            Serial.println("-> State: READ_GPS");
        }
        _sim_device->turnOFF();
        rtc_sleep(60 * MIN_TO_uS_FACTOR);
        // rtc_sleep(TintB * MIN_TO_uS_FACTOR);
        break;

    // ------------------------------------------
    case READ_GPS:
        time(&now);
        if (now - stateChange > (TGPS * MIN_TO_S_FACTOR))
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
                timerWrite(timer, 0); //reset timer (feed watchdog)
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
            timerWrite(timer, 0); //reset timer (feed watchdog)
            rtc_light_sleep(59);  // Retry every 1min
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
        if (now - stateChange > (Tsend * MIN_TO_S_FACTOR))
        { // Data upload not achieved
            set_handler.run(); // check SMS
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
            timerWrite(timer, 0); //reset timer (feed watchdog)
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
            timerWrite(timer, 0); //reset timer (feed watchdog)
            rtc_light_sleep(2);   // Retry every 2s
            break;
        }
        else
        {
            // Data sent
            tries = RETRIES; // Back to its original value
            state = IDLE;
            pending = false;
            set_handler.run(); // check SMS
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
