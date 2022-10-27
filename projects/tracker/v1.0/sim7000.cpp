#include "sim7000.h"

HardwareSerial SerialAT(1);
TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
HttpClient http(client, server, port);

// Constructor
Sim7000::Sim7000()
{
}

void Sim7000::setup()
{
    pinMode(PWRKEY, OUTPUT);
    SerialAT.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX); // baud rate, protocol, ESP32 RX pin, ESP32 TX pin
}

//--------------------------------------------------------------------
void Sim7000::powerON()
{
    digitalWrite(PWRKEY, HIGH);
    delay(1000); // Datasheet Ton = 1S
    digitalWrite(PWRKEY, LOW);
}

//--------------------------------------------------------------------
void Sim7000::powerOFF()
{
    digitalWrite(PWRKEY, HIGH);
    delay(1500); // Datasheet Toff = 1.2S
    digitalWrite(PWRKEY, LOW);
}

//--------------------------------------------------------------------
String Sim7000::init()
{
    String imei(NULL);
    if (DEBUG)
    {

        Serial.println("Initializing modem...");
    }
    if (!modem.init())
    {
        reset();
        if (DEBUG)
        {
            Serial.println("Failed to restart modem");
        }
        return imei;
    }

    String name = modem.getModemName();
    String modemInfo = modem.getModemInfo();

    if (DEBUG)
    {
        Serial.print("Modem Name: ");
        Serial.println(name);
        Serial.print("Modem Info: ");
        Serial.println(modemInfo);
    }
    imei = modem.getIMEI();
    modem.setNetworkMode(38); /*LTE only*/
    return (imei);
}

//--------------------------------------------------------------------
bool Sim7000::connect2LTE()
{
    int16_t signal = modem.getSignalQuality();
    bool isConnected = modem.isNetworkConnected();
    if (DEBUG)
    {
        Serial.print("Signal: ");
        Serial.print(signal);
        Serial.print(" ");
        Serial.print("isNetworkConnected: ");
        Serial.println(isConnected ? "CONNECT" : "NO CONNECT");
    }
    return isConnected;
}

//--------------------------------------------------------------------
bool Sim7000::connect2Internet()
{
    // GPRS connection parameters are usually set after network registration
    if (DEBUG)
    {
        Serial.print(F("Connecting to "));
        Serial.print(apn);
    }
    if (!modem.gprsConnect(apn, gprsUser, gprsPass))
    {
        if (DEBUG)
        {
            Serial.println(" fail");
        }
        return false;
    }
    if (DEBUG)
    {
        Serial.println(" success");
    }
    if (modem.isGprsConnected())
    {
        if (DEBUG)
        {
            Serial.println("Internet connected");
        }
        return true;
    }
    else
    {
        if (DEBUG)
        {
            Serial.println("Reporting as disconnected!");
        }
        return false;
    }
}

//--------------------------------------------------------------------
String Sim7000::getCellID()
{
    String res(NULL);
    int colons = 0;
    int i;
    bool result = false;

    if (DEBUG)
    {
        Serial.println("=====Inquiring UE system information=====");
    }
    modem.sendAT("+CPSI?");
    if (modem.waitResponse(1000L, res) == 1)
    {
        res.replace(GSM_NL "OK" GSM_NL, "");
        if (DEBUG)
        {
            Serial.println(res);
        }
    }
    int index = res.indexOf("Online");
    if (index != -1)
    {
        res = res.substring(index + 7);
        for (i = 0; i < res.length(); i++)
        {
            if (res[i] == ',')
            {
                colons++;
            }
            if (colons == 3)
            {
                break;
            }
        }
        if (colons != 3) // Wrong format
        {
            if (DEBUG)
            {
                Serial.println("Wrong formatted response!");
            }
            return String(NULL);
        }
        res = res.substring(0, i);
        return res;
    }
    else
    {
        if (DEBUG)
        {
            Serial.println("Device isn't online!");
        }
        return String(NULL); // Not online
    }
}

//--------------------------------------------------------------------
String Sim7000::uploadData(String msg)
{
    String body(NULL);

    if (DEBUG)
    {
        Serial.print(F("Performing HTTP GET request... "));
    }

    String contentType = "application/x-www-form-urlencoded";

    int err = http.post(resource, contentType, msg);
    if (err != 0)
    {
        if (DEBUG)
        {
            Serial.println(F("failed to connect"));
        }
        return body;
    }

    int status = http.responseStatusCode();
    if (DEBUG)
    {
        Serial.print(F("Response status code: "));
        Serial.println(status);
    }
    if (status != 200)
    {
        return body;
    }
    if (DEBUG)
    {
        Serial.println(F("Response Headers:"));
        while (http.headerAvailable())
        {
            String headerName = http.readHeaderName();
            String headerValue = http.readHeaderValue();
            Serial.println("    " + headerName + " : " + headerValue);
        }
    }

    int length = http.contentLength();
    if (length >= 0)
    {
        body = http.responseBody();
        if (DEBUG)
        {
            Serial.print(F("Content length is: "));
            Serial.println(length);

            if (http.isResponseChunked())
            {
                Serial.println(F("The response is chunked"));
            }

            Serial.println(F("Response:"));
            Serial.println(body);

            Serial.print(F("Body length is: "));
            Serial.println(body.length());
        }
    }

    // Shutdown
    http.stop();
    if (DEBUG)
    {
        Serial.println(F("Server disconnected"));
    }
    return body;
}

//--------------------------------------------------------------------
void Sim7000::hardReset()
{
    digitalWrite(PWRKEY, HIGH);
    delay(300); // Datasheet Treset = 252ms
    digitalWrite(PWRKEY, LOW);
}

//--------------------------------------------------------------------
void Sim7000::reset()
{
    powerOFF();
    delay(2000); // Short pause to let the capacitors discharge
    powerON();
}

//--------------------------------------------------------------------
bool Sim7000::powerOnGPS()
{
    if (DEBUG)
    {
        Serial.println("Enabling GPS/GNSS/GLONASS");
    }
    if (enableGPS())
    {
        return true;
    }
    return false;
}

//--------------------------------------------------------------------
bool Sim7000::enableGPS()
{
    // Set SIM7000G GPIO4 HIGH ,turn on GPS power
    // CMD:AT+SGPIO=0,4,1,1
    // Only in version 20200415 is there a function to control GPS power
    modem.sendAT("+SGPIO=0,4,1,1");
    if (modem.waitResponse(10000L) != 1)
    {
        if (DEBUG)
        {
            Serial.println(" SGPIO=0,4,1,1 false ");
        }
        return false;
    }
    return (modem.enableGPS());
}

//--------------------------------------------------------------------
bool Sim7000::disableGPS()
{
    // Set SIM7000G GPIO4 LOW ,turn off GPS power
    // CMD:AT+SGPIO=0,4,1,0
    // Only in version 20200415 is there a function to control GPS power
    modem.sendAT("+SGPIO=0,4,1,0");
    if (modem.waitResponse(10000L) != 1)
    {
        if (DEBUG)
        {
            Serial.println(" SGPIO=0,4,1,0 false ");
        }
        return false;
    }
    return (modem.disableGPS());
}

//--------------------------------------------------------------------
bool Sim7000::powerOffGPS()
{
    if (DEBUG)
    {
        Serial.println("Disabling GPS...");
    }
    if (disableGPS())
    {
        return true;
    }
    return false;
}

//--------------------------------------------------------------------
String Sim7000::getGPS()
{
    String result(NULL);

    Serial.println("Requesting current GPS/GNSS/GLONASS location...");
    if (modem.getGPS(&lat2, &lon2, &speed2, &alt2, &vsat2, &usat2, &accuracy2,
                     &year2, &month2, &day2, &hour2, &min2, &sec2))
    {
        if (DEBUG)
        {
            Serial.print("Latitude: ");
            Serial.print(String(lat2, 8));
            Serial.print("\tLongitude: ");
            Serial.println(String(lon2, 8));
            Serial.print("Speed: ");
            Serial.print(speed2);
            Serial.print("\tAltitude:");
            Serial.println(alt2);
            Serial.print("Visible Satellites: ");
            Serial.print(vsat2);
            Serial.print("\tUsed Satellites: ");
            Serial.println(usat2);
            Serial.print("Accuracy: ");
            Serial.println(accuracy2);
            Serial.print("Year: ");
            Serial.print(year2);
            Serial.print("\tMonth: ");
            Serial.print(month2);
            Serial.print("\tDay: ");
            Serial.println(day2);
            Serial.print("Hour: ");
            Serial.print(hour2);
            Serial.print("\tMinute: ");
            Serial.print(min2);
            Serial.print("\tSecond: ");
            Serial.println(sec2);
        }
    }
    else
    {
        if (DEBUG)
        {
            Serial.println(modem.getGPSraw());
            Serial.println("Couldn't get GPS/GNSS/GLONASS location.");
        }
        return result;
    }

    // Serial.println("Retrieving GPS/GNSS/GLONASS location again as a string");
    // String gps_raw = modem.getGPSraw();
    // Serial.print("GPS/GNSS Based Location String: ");

    // Form return string
    result += String(lat2, 6) + "," + String(lon2, 6) + "," +
              String((int)speed2) + "," + String((int)accuracy2);
    return result;
}