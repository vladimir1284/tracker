#include "sim.h"

SoftwareSerial ss(FONA_RX, FONA_TX);
SIM sim_device = SIM(&ss);

void setup()
{
  if (DEBUG)
  {
    // Initialize serial port
    Serial.begin(115200);
  }

  sim_device.setup();
}

void loop()
{
  
}