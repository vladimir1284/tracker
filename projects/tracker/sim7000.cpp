#include "sim7000.h"

// Constructor
Sim7000::Sim7000()
{
}

//--------------------------------------------------------------------
boolean Sim7000::prepareMessage()
{
    float latitude, longitude, speed_kph;
    uint16_t vbat, heading;
    char sats;
    if (!fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &sats))
    {
        return false;
    }
    else
    {
        // Read the module's power supply voltage
        fona.getBattVoltage(&vbat);

        pending = true;

        msg[0] = seq_num++;
        msg[1] = (char)mode;
        msg[2] = 0; // TODO haandle events
        msg[3] = sats;
        memcpy(&msg[4], (char *)&vbat, sizeof(vbat));
        memcpy(&msg[6], (char *)&heading, sizeof(heading));
        memcpy(&msg[8], (char *)&latitude, sizeof(latitude));
        memcpy(&msg[12], (char *)&longitude, sizeof(longitude));
        memcpy(&msg[16], (char *)&speed_kph, sizeof(speed_kph));

        if (DEBUG)
        {
            Serial.print(F("Latitude: "));
            Serial.println(latitude, 6);
            Serial.print(F("Longitude: "));
            Serial.println(longitude, 6);
            Serial.print(F("Speed: "));
            Serial.println(speed_kph);
            Serial.print(F("Heading: "));
            Serial.println(heading);
            Serial.print(F("N sats: "));
            Serial.println(sats);
        }
        return true;
    }
}

//--------------------------------------------------------------------
void Sim7000::setup()
{
    // SIM7000 takes about 3s to turn on
    // Press Arduino reset button if the module is still turning on and the board doesn't find it.
    // When the module is on it should communicate right after pressing reset

    if (DEBUG)
    {
        Serial.println(F("Start!"));
    }
    // Software serial:
    fonaSS.begin(115200, SERIAL_8N1, FONA_TX, FONA_RX); // baud rate, protocol, ESP32 RX pin, ESP32 TX pin

    if (DEBUG)
    {
        Serial.println(F("Configuring to 9600 baud"));
    }
    fonaSS.println("AT+IPR=9600");                    // Set baud rate
    delay(100);                                       // Short pause to let the command run
    fonaSS.begin(9600, SERIAL_8N1, FONA_TX, FONA_RX); // Switch to 9600
    if (!fona.begin(fonaSS, SIM_PWR))
    {
        if (DEBUG)
        {
            Serial.println(F("Couldn't find FONA"));
        }
        while (1)
            ; // Don't proceed if it couldn't find the device
    }
    if (DEBUG)
    {
        Serial.println(F("FONA is OK"));
    }
    // Print module IMEI number.
    imei_len = fona.getIMEI(imei);
    if (imei_len > 0)
    {
        if (DEBUG)
        {
            Serial.print("Module IMEI: ");
            Serial.println(imei);
        }
    }

    // Turn off modem
    fona.setFunctionality(0); // AT+CFUN=0

    // Configure
    fona.setNetworkSettings(F("hologram")); // For Hologram SIM card
    fona.setPreferredMode(38);              // Use LTE only, not 2G
    fona.setPreferredLTEMode(1);            // Use LTE CAT-M only, not NB-IoT
}

//--------------------------------------------------------------------
boolean Sim7000::netStatus()
{
    int n = fona.getNetworkStatus();
    if (DEBUG)
    {
        Serial.print(F("Network status "));
        Serial.print(n);
        Serial.print(F(": "));
        if (n == 0)
            Serial.println(F("Not registered"));
        if (n == 1)
            Serial.println(F("Registered (home)"));
        if (n == 2)
            Serial.println(F("Not registered (searching)"));
        if (n == 3)
            Serial.println(F("Denied"));
        if (n == 4)
            Serial.println(F("Unknown"));
        if (n == 5)
            Serial.println(F("Registered roaming"));
    }
    if (!(n == 1 || n == 5))
        return false;
    else
        return true;
}

//--------------------------------------------------------------------
boolean Sim7000::uploadData(byte QoS)
{
    // Open wireless connection if not already activated
    if (!fona.wirelessConnStatus())
    {
        if (!fona.openWirelessConnection(true))
        {
            if (DEBUG)
            {
                Serial.println(F("Failed to enable connection, retrying..."));
            }
            return false;
        }
        if (DEBUG)
        {
            Serial.println(F("Enabled data!"));
        }
    }
    else
    {
        if (DEBUG)
        {
            Serial.println(F("Data already enabled!"));
        }
    }

    // If not already connected, connect to MQTT
    if (!fona.MQTT_connectionStatus())
    {
        // Set up MQTT parameters (see MQTT app note for explanation of parameter values)
        fona.MQTT_setParameter("URL", MQTT_SERVER, MQTT_PORT);
        // Set up MQTT username and password if necessary
        // fona.MQTT_setParameter("USERNAME", MQTT_USERNAME);
        // fona.MQTT_setParameter("PASSWORD", MQTT_PASSWORD);
        // fona.MQTT_setParameter("RETAIN", "1");     // Keep last message alaive
        if (!fona.MQTT_dataFormatHex(false))
        {
            return false;
        }
        fona.MQTT_setParameter("KEEPTIME", "3600"); // Time to connect to server, 60s by default
        if (DEBUG)
        {
            Serial.println(F("Connecting to MQTT broker..."));
        }
        if (!fona.MQTT_connect(true))
        {
            if (DEBUG)
            {
                Serial.println(F("Failed to connect to broker!"));
            }
            return false;
        }
    }
    else
    {
        if (DEBUG)
        {
            Serial.println(F("Already connected to MQTT server!"));
        }
    }

    // Publish data
    if (DEBUG)
    {
        for (int i = 0; i < MSG_SIZE; i++)
        {
            Serial.print(msg[i], DEC);
            Serial.print(',');
        }
        Serial.println();
    }

    // if (!fona.MQTT_publish(imei, msg, MSG_SIZE, QoS, 0))
    const char myChars[21] = "Hello TOWIT Houston!";
    if (!fona.MQTT_publish(imei, myChars, MSG_SIZE, QoS, 0))
    {
        if (DEBUG)
        {
            Serial.println(F("Failed to publish!")); // Send GPS location
        }
        return false;
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