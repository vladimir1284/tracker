#include <EEPROMsettings.h>

#define DEBUG true

Settings stgs = Settings();

void setup()
{
    stgs.processConfigs("{mode: 0}");
}

void loop()
{
}
