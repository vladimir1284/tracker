#include <ArduinoJson.h>
#define MAX_INPUT_LENGTH 200
#define LBRACE 123
#define RBRACE 125

#define DEBUG true
int len, open;

StaticJsonDocument<96> doc;
char input[MAX_INPUT_LENGTH];

void setup()
{
  len = 0;
  open = 0;

  if (DEBUG)
  {
    // Initialize serial port
    Serial.begin(115200);
  }
  // parseJSON();
}

void parseJSON()
{
  // {"status": "ok"}
  // {"status":"ok","configs":{"Tcheck":15,"MAX_ERRORS":3,"TintB":360,"TsendB":10,"TGPSB":10,"SMART":1,"TGPS":10,"Tint":60,"Tsend":10}}

  DeserializationError error = deserializeJson(doc, input, MAX_INPUT_LENGTH);
  // DeserializationError error = deserializeJson(doc, Serial);

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
        int configs_Tcheck = configs["Tcheck"];         // 15
        int configs_MAX_ERRORS = configs["MAX_ERRORS"]; // 3
        int configs_TintB = configs["TintB"];           // 360
        int configs_TsendB = configs["TsendB"];         // 10
        int configs_TGPSB = configs["TGPSB"];           // 10
        int configs_SMART = configs["SMART"] | -1;      // 1
        int configs_TGPS = configs["TGPS"];             // 10
        int configs_Tint = configs["Tint"];             // 60
        int configs_Tsend = configs["Tsend"];           // 10

        if (configs_Tcheck > 0)
        {
          Serial.print("Tcheck: ");
          Serial.println(configs_Tcheck);
        }

        if (configs_MAX_ERRORS > 0)
        {
          Serial.print("MAX_ERRORS: ");
          Serial.println(configs_MAX_ERRORS);
        }

        if (configs_TintB > 0)
        {
          Serial.print("TintB: ");
          Serial.println(configs_TintB);
        }

        if (configs_TsendB > 0)
        {
          Serial.print("TsendB: ");
          Serial.println(configs_TsendB);
        }

        if (configs_TGPSB > 0)
        {
          Serial.print("TGPSB: ");
          Serial.println(configs_TGPSB);
        }

        if (configs_SMART > -1)
        {
          Serial.print("SMART: ");
          Serial.println(configs_SMART);
        }

        if (configs_TGPS > 0)
        {
          Serial.print("TGPS: ");
          Serial.println(configs_TGPS);
        }

        if (configs_Tint > 0)
        {
          Serial.print("Tint: ");
          Serial.println(configs_Tint);
        }

        if (configs_Tsend > 0)
        {
          Serial.print("Tsend: ");
          Serial.println(configs_Tsend);
        }
      }
    }
  }
}

void loop()
{
  char c;
  do
  {
    if (Serial.available())
    {
      c = Serial.read();
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
  // Serial.println(input);
  parseJSON();
}