#include "time.h"
#include "configs.h"
#include "sim.h"
#include "fsm.h"
#include "gps.h"

RTC_DATA_ATTR states currentState;
RTC_DATA_ATTR unsigned long currentLastInterval;
RTC_DATA_ATTR unsigned long millisOffset;

FSM fsm = FSM();

HardwareSerial fonaSS(1);
SIM sim_device = SIM(&fonaSS, &fsm);

HardwareSerial ss(2);
GPS gps_controller = GPS(&ss);

void setup()
{
  // watchdogBegin();

  // We only keep RTC values when waking up from deep sleep
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason != ESP_SLEEP_WAKEUP_TIMER)
  {
    currentState = ENERGY;
    currentLastInterval = 0;
    millisOffset = 0;
  }

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

unsigned long getMillis()
{
  return millisOffset + millis();
}