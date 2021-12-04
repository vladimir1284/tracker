#include "sim.h"

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SIM sim_device = SIM(&fonaSS);

void setup()
{
  if (DEBUG)
  {
    // Initialize serial port
    Serial.begin(115200);
  }

  // Initialize SIM
  sim_device.setup();

  
}

void loop()
{
  
}