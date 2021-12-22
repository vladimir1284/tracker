#include "sim.h"
#include "fsm.h"

FSM fsm = FSM();

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SIM sim_device = SIM(&fonaSS, &fsm);

SoftwareSerial ss(RXPinGPS, TXPinGPS);
GPS gps = GPS(&ss);

void setup()
{
  if (DEBUG)
  {
    // Initialize serial port
    Serial.begin(115200);
  }

  // Initialize GPS
  gps.setup();

  // Initialize FSM
  fsm.setup(PIN12V, &gps);

  // Initialize SIM
  sim_device.setup();
}

void loop()
{
}