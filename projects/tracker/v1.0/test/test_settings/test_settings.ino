#include <EEPROMsettings.h>

Settings stgs = Settings();

void setup()
{
    Serial.begin(115200);
    stgs.setup();
    stgs.processConfigs("{mode: 0}");
}

void loop()
{
    delay(3000);
    Serial.print("Mode: ");
    Serial.println(stgs.Mode);
}
