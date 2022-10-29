
#include "controller.h"

//-------------------------- Constructor -----------------------------
Controller::Controller()
{
}

//--------------------------------------------------------------------
void Controller::setup(Sim7000 *sim_device, Settings *settings)
{
    clear();

    sim = sim_device;
    stgs = settings;
}

//--------------------------------------------------------------------
bool Controller::clear()
{
    state = MODEM_PWR_ON;
    tries = 0;
    timeout = false;
    return true;
}

//--------------------------------------------------------------------
bool Controller::run(bool charging, int vbat, int seq, int wur, int wdgc)
{
    time(&now);

    switch (state)
    {
    case MODEM_PWR_ON:
        imei = sim->init();
        if (imei.equals(String(NULL)))
        {
            if (DEBUG)
            {
                Serial.println("Delaying 10s and retrying...");
            }
            delay(10000);
        }
        else
        {
            if (DEBUG)
            {
                Serial.println("IMEI: " + imei);
                Serial.println("Delaying 3s...");
            }
            setState(LTE_NETWORK);
            delay(3000);
        }
        break;

    case LTE_NETWORK:
        if (sim->connect2LTE())
        {
            // Mode (0: Keepalive - 1: Tracking)
            if (charging || stgs->Mode == 1)
            {
                // Tracking or charging
                setState(GPS_PWR_ON);
            }
            else
            {
                // Keepalive not charging
                setState(CELL_ID);
            }
        }
        else
        {
            delay(1000);
        }
        break;

    case GPS_PWR_ON:
        if (sim->powerOnGPS())
        {
            if (DEBUG)
            {
                Serial.println("Waiting 15s for warm-up...");
            }
            delay(15000);
            setState(GPS_FIX);
        }
        break;

    case GPS_FIX:
        if (now - stateChange > (time_t)stgs->TGPS * MIN_TO_S_FACTOR)
        {
            if (DEBUG)
            {
                Serial.print("Aborting GPS fixing after ");
                Serial.print(now - stateChange);
                Serial.println("s!");
            }
            setState(GPS_PWR_OFF);
            timeout = true;
        }

        gpsData = sim->getGPS();
        if (gpsData.equals(String(NULL)))
        {
            if (DEBUG)
            {
                Serial.println("Retrying in 2s...");
            }
            delay(2000);
        }
        else
        {
            if (DEBUG)
            {
                Serial.println("GPD data str: " + gpsData);
            }
            msg = imei + "," + seq + "," + charging + "," + vbat + "," +
                  wur + "," + wdgc + "," + "GPS," + gpsData;
            setState(GPS_PWR_OFF);
        }
        break;

    case GPS_PWR_OFF:
        if (sim->powerOffGPS())
        {
            if (DEBUG)
            {
                Serial.println("GPS disabled!");
            }
            if (timeout)
            {
                // Send cellular antenna position
                setState(CELL_ID);
            }
            else
            {
                // Send GPS position
                setState(CONNECT);
            }
        }
        break;

    case CELL_ID:
        cellID = sim->getCellID();
        if (cellID.equals(String(NULL)))
        {
            delay(1000);
        }
        else
        {
            if (DEBUG)
            {
                Serial.println("UE system information: " + cellID);
            }
            msg = imei + "," + seq + "," + charging + "," + vbat + "," +
                  wur + "," + wdgc + "," + "LTE," + cellID;
            setState(CONNECT);
        }
        break;

    case CONNECT:
        if (sim->connect2Internet())
        {
            setState(UPLOAD);
        }
        else
        {
            if (DEBUG)
            {
                Serial.println("Delaying 10s and retrying...");
            }
            delay(10000);
        }
        break;

    case UPLOAD:
        tries++;
        if (DEBUG)
        {
            Serial.print("Uploading datagram: ");
            Serial.println(msg);
            Serial.print("Attempt #");
            Serial.println(tries);
        }
        config = sim->uploadData(msg);
        if (config != String(NULL))
        {
            if (DEBUG)
            {
                Serial.println("Processing configs");
            }
            stgs->processConfigs(config);

            return clear();
        }
        if (tries >= UPLOAD_RETRIES)
        {
            return clear();
        }
        break;

    default:
        if (DEBUG)
        {
            Serial.println("Shouldn't be here!");
        }
        setState(MODEM_PWR_ON);
        break;
    }
    return false;
}

//--------------------------------------------------------------------
void Controller::setState(states newState)
{
    String state_str;

    time(&now);
    stateChange = now;
    state = newState;
    switch (newState)
    {
    case MODEM_PWR_ON:
        state_str = "MODEM_PWR_ON";
        break;
    case LTE_NETWORK:
        state_str = "LTE_NETWORK";
        break;
    case GPS_PWR_ON:
        state_str = "GPS_PWR_ON";
        break;
    case GPS_FIX:
        state_str = "GPS_FIX";
        break;
    case GPS_PWR_OFF:
        state_str = "GPS_PWR_OFF";
        break;
    case CELL_ID:
        state_str = "CELL_ID";
        break;
    case CONNECT:
        state_str = "CONNECT";
        break;
    case UPLOAD:
        state_str = "UPLOAD";
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