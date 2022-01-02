#include <Arduino.h>
#line 1 "/home/vladimir/Dicso/Salvas-LAP-ene2017/Documents/personales/Empresa/JA/GPS/custom/projects/tracker/tracker.ino"
#line 1 "/home/vladimir/Dicso/Salvas-LAP-ene2017/Documents/personales/Empresa/JA/GPS/custom/projects/tracker/tracker.ino"
#include "configs.h"
#include "sleep.h"
#include "sim.h"
#include "fsm.h"
#include "gps.h"
FSM fsm = FSM();

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SIM sim_device = SIM(&fonaSS, &fsm);

SoftwareSerial ss(RXPinGPS, TXPinGPS);
GPS gps_controller = GPS(&ss);

void setup()
{
  watchdogBegin();

  if (DEBUG)
  {
    // Initialize serial port
    Serial.begin(115200);
  }

  // Initialize GPS
  gps_controller.setup();

  // Initialize FSM
  fsm.setup(PIN12V, &gps_controller, &sim_device);

  // Initialize SIM
  sim_device.setup();
}

void loop()
{
  fsm.run();
  // gps_controller.run();

  // if (Serial.available())
  // {
  //   char command = Serial.read();

  //   switch (command)
  //   {
  //   case 's':
  //     // Get SoftwareSerial listenning
  //     fonaSS.listen();
  //     sim_device.uploadData(gps_controller.lastLat, gps_controller.lastLon, true);
  //     break;

  //   case 'p':
  //     Serial.print("Lat: ");
  //     Serial.println(gps_controller.lastLat);
  //     Serial.print("Lon: ");
  //     Serial.println(gps_controller.lastLon);
  //     break;

  //   default:
  //     break;
  //   }
  // }
}

