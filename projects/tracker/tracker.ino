#include "configs.h"
#include "rtc.h"
#include "sim.h"
#include "fsm.h"
#include "gps.h"

FSM fsm = FSM();

HardwareSerial fonaSS(1);
SIM sim_device = SIM(&fonaSS, &fsm);

HardwareSerial ss(2);
GPS gps_controller = GPS(&ss);

void setup()
{
  // watchdogBegin();

  rtc_handle_wakeup();

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
  // sim_device.turnOn();
  // sim_device.setup();
}

void loop()
{
  sim_device.turnOff();

  gps_controller.run();

  sim_device.turnOn();

  rtc_sleep((unsigned long)TGPS * uS_TO_S_FACTOR);
}