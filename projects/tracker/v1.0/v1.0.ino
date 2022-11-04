#include "energy.h"
#include "controller.h"
#include "EEPROMsettings.h"
#include <esp_task_wdt.h>
#include "esp_system.h"

Settings stgs;
Controller ctrl;

// ---------------------------------------------
// Compute the proper sleep time
int computeSleepTime()
{
  if (charging || stgs.Mode)
  {
    return TRACKING_INTERVAL * MIN_TO_S_FACTOR; // 1h sleeping
  }
  else
  {
    // Keepalive
    return stgs.Tint * MIN_TO_S_FACTOR; // configurable delay
  }
}

// ---------------------------------------------
// Check the total time awaken
void checkAwakeTime()
{
  time_t now;
  time(&now);

  if (now - lastWakeup > (time_t)(stgs.TGPS + stgs.Tsend) * MIN_TO_S_FACTOR)
  {
    if (DEBUG)
    {
      Serial.print("Stopping execution after ");
      Serial.print((now - lastWakeup) / 60);
      Serial.println("min for saving battery!");
    }
    rtc_deep_sleep(computeSleepTime());
  }
}

void setup()
{
  // Status LED
  pinMode(STATUSLED, OUTPUT);
  digitalWrite(STATUSLED, HIGH);

  // Save battery asap if needed
  checkBatteryVoltage();

  if (DEBUG)
  {
    // Initialize serial port
    Serial.begin(921600); // 115200 in the serial monitor scaled x8
  }

  reduceFreq();

  Serial.println();
  Serial.println();
  Serial.println();

  rtc_handle_wakeup();

  watchdogConfig(WDT_TIMEOUT);

  stgs.setup();
  ctrl.setup(&sim, &stgs);
}

void loop()
{
  timerWrite(timer, 0); // reset timer (feed watchdog)
  detectCharging();
  checkBatteryVoltage();
  checkAwakeTime();
  if (ctrl.run(charging, vbat, seq_num, wakeup_reason, wdg_rst_count))
  {
    time(&lastInterval);
    rtc_deep_sleep(computeSleepTime());
  }
}
