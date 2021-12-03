#include "configs.h"

// Constructor
Configs::Configs(SoftwareSerial *softSerial)
{
    ss = softSerial;
}

void Configs::getSerialData()
{
    char c;
    do
    {
        if (ss->available())
        {
            c = ss->read();
            input[len++] = c;

            // Check for nested braces
            if (c == LBRACE)
            {
                open++;
            }

            if (c == RBRACE)
            {
                open--;
                if (open == 0)
                {
                    break; // Complete JSON recieved
                }
            }
        }
    } while (len < MAX_INPUT_LENGTH);

    len = 0;

    parseJSON();
}

void Configs::parseJSON()
{
    // {"status": "ok"}
    // {"status":"ok","configs":{"Tcheck":15,"MAX_ERRORS":3,"TintB":360,"TsendB":10,"TGPSB":10,"SMART":1,"TGPS":10,"Tint":60,"Tsend":10}}

    DeserializationError error = deserializeJson(doc, input, MAX_INPUT_LENGTH);

    if (error)
    {
        if (DEBUG)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            return;
        }
    }

    const char *status = doc["status"]; // "ok"
    if (status != nullptr)
    {
        if (DEBUG)
        {
            Serial.print("status: ");
            Serial.println(status);
        }

        String status_str = String(status);
        if (status_str.equals("ok"))
        {
            JsonObject configs = doc["configs"];
            if (!configs.isNull())
            {
                configs_Tcheck = configs["Tcheck"];         // 15
                configs_MAX_ERRORS = configs["MAX_ERRORS"]; // 3
                configs_TintB = configs["TintB"];           // 360
                configs_TsendB = configs["TsendB"];         // 10
                configs_TGPSB = configs["TGPSB"];           // 10
                configs_SMART = configs["SMART"] | -1;      // 1
                configs_TGPS = configs["TGPS"];             // 10
                configs_Tint = configs["Tint"];             // 60
                configs_Tsend = configs["Tsend"];           // 10

                updateTcheck();
                updateMAX_ERRORS();
                updateTintB();
                updateTsendB();
                updateTGPSB();
                updateSMART();
                updateTGPS();
                updateTint();
                updateTsend();
            }
        }
    }
}

void Configs::updateTcheck()
{
    if (configs_Tcheck > 0)
    {
        if (DEBUG)
        {
            Serial.print("Tcheck: ");
            Serial.println(configs_Tcheck);
        }
    }
}
void Configs::updateMAX_ERRORS()
{
    if (configs_MAX_ERRORS > 0)
    {
        if (DEBUG)
        {
            Serial.print("MAX_ERRORS: ");
            Serial.println(configs_MAX_ERRORS);
        }
    }
}
void Configs::updateTintB()
{
    if (configs_TintB > 0)
    {
        if (DEBUG)
        {
            Serial.print("TintB: ");
            Serial.println(configs_TintB);
        }
    }
}
void Configs::updateTsendB()
{
    if (configs_TsendB > 0)
    {
        if (DEBUG)
        {
            Serial.print("TsendB: ");
            Serial.println(configs_TsendB);
        }
    }
}
void Configs::updateTGPSB()
{
    if (configs_TGPSB > 0)
    {
        if (DEBUG)
        {
            Serial.print("TGPSB: ");
            Serial.println(configs_TGPSB);
        }
    }
}
void Configs::updateSMART()
{
    if (configs_SMART > -1)
    {
        if (DEBUG)
        {
            Serial.print("SMART: ");
            Serial.println(configs_SMART);
        }
    }
}
void Configs::updateTGPS()
{
    if (configs_TGPS > 0)
    {
        if (DEBUG)
        {
            Serial.print("TGPS: ");
            Serial.println(configs_TGPS);
        }
    }
}
void Configs::updateTint()
{
    if (configs_Tint > 0)
    {
        if (DEBUG)
        {
            Serial.print("Tint: ");
            Serial.println(configs_Tint);
        }
    }
}

void Configs::updateTsend()
{
    if (configs_Tsend > 0)
    {
        if (DEBUG)
        {
            Serial.print("Tsend: ");
            Serial.println(configs_Tsend);
        }
    }
}