#include "configs.h"
#include "rtc.h"
#include "sim.h"
#include "fsm.h"
#include "gps.h"

hw_timer_t *timer = NULL;

FSM fsm = FSM();

HardwareSerial fonaSS(1);
SIM sim_device = SIM(&fonaSS, &fsm);

HardwareSerial ss(2);
GPS gps_controller = GPS(&ss);


void IRAM_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart();
}

void setup()
{
  // ------- watchdog begin --------------
  timer = timerBegin(0, 80, true);                 //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true); //attach callback
  timerAlarmWrite(timer, wdtTimeout, false);       //set time in us
  timerAlarmEnable(timer);
  // -------------------------------------

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
