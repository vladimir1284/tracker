# pylint: disable=import-outside-toplevel
# pylint: disable=consider-using-f-string
# pylint: disable=attribute-defined-outside-init
# pylint: disable=global-statement

import time
import json
import esp32
import machine
from micropython import const, alloc_emergency_exception_buf
from machine import WDT, deepsleep, lightsleep, RTC, Pin
from sim7000 import Sim7000
from settings import Settings
from fsm import FSM
try:
    from typing import Optional
    from ulogging import RootLogger
except ImportError:
    pass

# Debugging is simplified if the following code is included in any program using interrupts
alloc_emergency_exception_buf(100)

MOVTHRESHOLD = const(1)  # Number of vibrations in a time interval for detecting movement
MOVDELAY	 = const(10) # Time interval for detecting movement in seconds

PIN12V	= const(34) # Input pin for checking 12V connection
DEBOUNCE_COUNT = const(3) # Number of verifications por debouncing 12V pin
DEBOUNCE_INTERVAL = const(5) # Seconds delay between verifications

PINVBR	= const(33) # Input pin for checking the vibration sensor

S_TO_mS_FACTOR = const(1000)

# Events
RESTART   = const(0) # Normal microcontroller start
POWERED   = const(1) # 12V connected while leeping
TIMEOUT   = const(2) # RTC timer overflow in deep sleep
WATCHDOG  = const(3) # WDG timer reset
VIBRATION = const(4) # Vibration sensor activated while sleeping

class Controller:

    # /********* INITIALIZATION AND SETTING STORAGE ***********************/
    def __init__(
        self,
        debug: int = 0,
        log: Optional[RootLogger] = None
        )-> None:

        self._wdt = WDT(timeout=20000)  # enable it with a timeout of 20s

        if log is None and debug >= 0:
            import ulogging
            log = ulogging.getLogger("controller")
            if debug > 0:
                log.setLevel(ulogging.DEBUG)
        self._log = log

        self._pin12V = Pin(PIN12V, mode = Pin.IN)

        # Retrieve soraged data from RTC ram
        self._rtc = RTC()
        try:
            rtc_data = json.loads(self._rtc.memory())
            fsm_loaded_data = rtc_data['fsm']
            self.mode       = rtc_data['mode']
            self.moving     = rtc_data['moving']
            self._seq_num   = rtc_data['seq']
        except Exception as err:
            self.mode       = 'PWR'
            self.moving     = True #TODO moving should be set False after a determined time without vibration
            self._seq_num   = 0
            fsm_loaded_data = None
            self._log.error(err)

        if machine.wake_reason == machine.WDT_RESET:
            self._event = WATCHDOG
        elif machine.wake_reason == machine.DEEPSLEEP_RESET:
            self._handle_wakeup()
        else:
            self._event = RESTART

        self._settings = Settings(debug=1)
        self._sim = Sim7000(debug=1)

        self._fsm = FSM(self._sim, self, self._settings, self._wdt, loaded_data=fsm_loaded_data, debug=1)

        # Movement detection
        self._lastVibrationCheck = 0
        self._pinVBR = Pin(PINVBR, mode = Pin.IN)
        self._vibrationNumber = 0 # This variable is modified by sensor interrupt (global)
        self._pinVBR.irq(trigger=Pin.IRQ_RISING, handler=self._countVibrations)


    # ---- Handle Vibration sensor interrupt ---------
    def _countVibrations(self, pin: Pin):
        self._vibrationNumber += 1

    # ---------------------------------------------
    def run(self):
    # Loop forever function
        self._wdt.feed() # reset timer (feed watchdog)
        self._detectMode()
        self._detectMovement()
        self._fsm.run()

    # ---------------------------------------------
    def upload_data(self):
    # Upload data to the remote server
        msg = "{},{},{},{},{:.5f},{:.5f},{},{},{},{}".format(
            self._sim.imei,
            self._seq_num,
            {'PWR': 0, 'BATTERY': 1}[self.mode],
            self._event,
            self._sim.gps_data[0],      # latitude
            self._sim.gps_data[1],      # longitude
            int(self._sim.gps_data[2]), # speed_kph
            int(self._sim.gps_data[3]), # heading
            self._sim.gps_data[4],      # Number of sats
            self._sim.battVoltage
        )
        # gps_data = [latitude, longitude, speed_kph, heading, sats, HDOP]
        if self._sim.uploadData(msg):
            self._seq_num += 1
            return True
        else:
            return False

    # ---------------------------------------------
    def rtc_light_sleep(self, delay: int):
    # Low energy compsumption delay (in seconds)
        lightsleep(delay * S_TO_mS_FACTOR)

    # ---------------------------------------------
    def rtc_deep_sleep(self, delay: int):
    # Deep sleep delay (in seconds)

        # Compute next wake up
        self._nextWakeUp = time.time() + delay
        self._log.debug("Sleeping for: {}s...".format(delay))
        self._log.debug("Wake up if power connected.")

        if self.moving:
            # Wakeup on energy connection
            esp32.wake_on_ext1(pins = (self._pin12V, ), level = esp32.WAKEUP_ANY_HIGH)
        else:
            # Wakeup on energy connection or on vibration
            self._log.debug("Not moving, wake up also if vibration is detected")
            esp32.wake_on_ext1(pins = (self._pin12V, self._pinVBR), level = esp32.WAKEUP_ANY_HIGH)

        self._save2rtc()

        # Going to sleep
        deepsleep(delay * S_TO_mS_FACTOR)

    # ---------------------------------------------
    def _save2rtc(self):
        writedata = {
            'nextWakeUp': self._nextWakeUp,
            'moving': self.moving,
            'mode': self.mode,
            'seq': self._seq_num,
            'fsm': self._fsm.get_state_data()
        }
        self._rtc.memory(json.dumps(writedata)) # this command writes writedata into the RTC memory

    # ---------------------------------------------
    def _handle_wakeup(self):
        # Find out the reazon for waking up
        if not self._detectMode(): # Check for power connected event
            remainig_time = self._nextWakeUp - time.time()
            if remainig_time > 0: # Check RTC timer overflow
                # Wake up reason is vibration sensor
                self.moving = True
                self.rtc_deep_sleep(remainig_time)
                self._event = VIBRATION
                self._log.debug("Waking up on vibration.")
            else:
                self._event = TIMEOUT
                self._log.debug("Waking up on timeout.")
        else:
            self._event = POWERED
            self._log.debug("Waking up on power connected.")

    # ---------------------------------------------
    # Detect movement every MOVDELAY seconds
    def _detectMovement(self):
        if not self.moving:
            if time.time() - self._lastVibrationCheck > MOVDELAY:
                self._lastVibrationCheck = time.time()
                self._log.debug("Vibrations detected: {}".format(self._vibrationNumber))
                if self._vibrationNumber > MOVTHRESHOLD:
                    self.moving = True
                self._vibrationNumber = 0

    # ---------------------------------------------
    def _detectMode(self) -> bool:
    # Detect battery powered debouncing input signal
    # Returns True when a new mode is detected

        current_value = {'PWR': 1, 'BATTERY': 0}
        next_mode = {'PWR': 'BATTERY', 'BATTERY': 'PWR'}

        # Look for a change in the 12V pin, which should be
        # sostained for DEBOUNCE_COUNT verifications
        for i in range(DEBOUNCE_COUNT):
            if self._pin12V.value() == current_value[self.mode]:
                return False
            time.sleep_ms(DEBOUNCE_INTERVAL)

        if i == DEBOUNCE_COUNT - 1:
            self.mode = next_mode[self.mode]
            self._log.debug("Mode: {}".format(self.mode))
            self._fsm.modeUpdated() # Notify FSM to change state
            return True