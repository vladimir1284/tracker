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

        // TODO haandle events
        sprintf(msg, "%s,%d,%d,%d,%.5f,%.5f,%d,%d,%d,%d", imei, seq_num++, mode, 0, latitude, longitude, (int)speed_kph, heading, 0, vbat);

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
            Serial.println(msg);
        }
        return true;
    }
}

//--------------------------------------------------------------------
bool Sim7000::checkSMS()
{
    fona.setFunctionality(1); // AT+CFUN=1
    delay(100);

    int numSMS = fona.getNumSMS();
    if (DEBUG)
    {
        Serial.print("Number of SMS available: ");
        Serial.println(numSMS);
    }
    if (numSMS > 0)
    {
        // Retrieve SMS value.
        for (size_t i = 0; i < numSMS; i++)
        {
            uint16_t smslen;
            if (fona.readSMS(i, smsBuffer, MAX_INPUT_LENGTH, &smslen))
            { // pass in buffer and max len!
                if (DEBUG)
                {
                    Serial.print("Read SMS in slot ");
                    Serial.println(i);
                }
            }
            // Delete the original message after it is processed.
            if (fona.deleteSMS(i))
            {
                if (DEBUG)
                {
                    Serial.println(F("OK!"));
                }
                return true;
            }
            else
            {
                if (DEBUG)
                {
                    Serial.print(F("Couldn't delete SMS in slot "));
                    Serial.print(i);
                    Serial.println(F("!"));
                }
            }
        }
    }
    else
    {
        if (DEBUG)
        {
            Serial.println("There are no SMS available!");
        }
    }

    return false;
}

//--------------------------------------------------------------------
void Sim7000::turnOFF()
{
    digitalWrite(SIM_PWR, LOW);
}

//--------------------------------------------------------------------
void Sim7000::turnON()
{
    digitalWrite(SIM_PWR, HIGH);
    for (int i = 0; i < 3; i++)
    {
        if (configure())
        {
            break;
        }
        turnOFF();
        delay(100); // Short pause to let the capacitors discharge
        digitalWrite(SIM_PWR, HIGH);
    }
}

//--------------------------------------------------------------------
void Sim7000::reset()
{
    turnOFF();
    delay(100); // Short pause to let the capacitors discharge
    turnON();
}

//--------------------------------------------------------------------
bool Sim7000::configure()
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

    if (!fona.begin(fonaSS, PWRKEY))
    {
        if (DEBUG)
        {
            Serial.println(F("Couldn't find FONA"));
        }
        return false;
        // while (1)
        //     ; // Don't proceed if it couldn't find the device
    }
    if (DEBUG)
    {
        Serial.println(F("FONA is OK"));
    }

    // Turn off modem
    fona.setFunctionality(0); // AT+CFUN=0

    // Configure
    fona.setNetworkSettings(F(APN_NAME)); // For Hologram SIM card
    fona.setPreferredMode(38);            // Use LTE only, not 2G
    fona.setPreferredLTEMode(1);          // Use LTE CAT-M only, not NB-IoT
    return true;
}

//--------------------------------------------------------------------
void Sim7000::setup()
{
    pinMode(SIM_PWR, OUTPUT);

    // Turn on the module
    turnON();

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
boolean Sim7000::uploadData()
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

    // HTTP send data
    if (!fona.HTTP_connect(SERVER))
    {
        if (DEBUG)
        {
            Serial.println(F("Failed to connect to server!"));
        }
        return false;
    }

    // Upload data
    if (!fona.HTTP_POST(ADDR, msg, strlen(msg)))
    {
        if (DEBUG)
        {
            Serial.println(F("Failed to upload!")); // Send GPS location
        }
        return false;
    }
}
