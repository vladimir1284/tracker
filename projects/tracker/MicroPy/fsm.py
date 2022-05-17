
# pylint: disable=import-outside-toplevel
# pylint: disable=consider-using-f-string
# pylint: disable=attribute-defined-outside-init

import time
from micropython import const
from sim7000 import Sim7000
try:
    from typing import Optional
    from settings import Settings
    from ulogging import RootLogger
    from machine import WDT
    from controller import Controller
except ImportError:
    pass

MIN_TO_S_FACTOR = const(60)

RETRIES = const(10) # Repetitions of unsuccessful tasks

class FSM:

    # /********* INITIALIZATION AND SETTING STORAGE ***********************/
    def __init__(
        self,
        sim: Sim7000,
        ctrl: Controller,
        settings: Settings,
        wdt: WDT,
        loaded_data: dict = None,
        debug: int = 0,
        log: Optional[RootLogger] = None
        )-> None:

        if log is None and debug >= 0:
            import ulogging
            log = ulogging.getLogger("fsm")
            if debug > 0:
                log.setLevel(ulogging.DEBUG)
        self._log = log

        self._stateChange = time.time()

        # Load initial status data, maybe from RTC memory
        self._init_loaded_data(loaded_data)

        self._sim_device = sim
        self._ctrl = ctrl
        self._settings = settings
        self._wdt = wdt


        self._bat_states = {
            'IDLE': self._bat_idle,
            'READ_GPS': self._bat_gps,
            'SEND_DATA': self._bat_lte,
            'ERROR': self._error,
        }

        self._pwr_states = {
            'IDLE': self._pwr_idle,
            'READ_GPS': self._pwr_gps,
            'SEND_DATA': self._pwr_lte,
            'ERROR': self._error,
        }

        self._switch_state = {
            'PWR': self._pwr_states,
            'BATTERY': self._bat_states
        }

    #--------------------------------------------------------------------
    def _init_loaded_data(self, loaded_data):
        if loaded_data is not None:
            try:
                self._state         = loaded_data['state']
                self._gpsErrors     = loaded_data['gpsErrors']
                self._pending       = loaded_data['pending']
                self._gsmErrors     = loaded_data['gsmErrors']
                self._lastInterval  = loaded_data['lastInterval']
                self._lastMsgSent   = loaded_data['lastMsgSent']
                return True
            except Exception as err:
                self._log.debug(err)

        # Use default values
        self._state         = 'READ_GPS'
        self._pending       = False
        self._gsmErrors     = 0
        self._gpsErrors     = 0
        self._lastInterval  = 0
        self._lastMsgSent   = 0
            

    #--------------------------------------------------------------------
    def get_state_data(self):
        return {
            'state': self._state,
            'gpsErrors': self._gpsErrors,
            'pending': self._pending,
            'gsmErrors': self._gsmErrors,
            'lastInterval': self._lastInterval,
            'lastMsgSent': self._lastMsgSent,
        }

    # /********* GENERIC FUNCTIONS ***********************/

    #--------------------------------------------------------------------
    def run(self):
        self._wdt.feed() # reset timer (feed watchdog)
        self._switch_state[self._ctrl.mode][self._state]()

    #--------------------------------------------------------------------
    def modeUpdated(self):
        if (time.time() - self._lastMsgSent >
            self._settings.settings['Tint'] * MIN_TO_S_FACTOR / 2):
            # Force update on new mode detected if the time past from
            # the last server upload, exceeds the half of Tint
            self._state = 'READ_GPS'

    # ------------------------------------------------------------------
    def _check_sms(self):
        sms = self._sim_device.checkSMS()
        if sms != "":
            self._settings.updateSetting(sms)

    # ------------------------------------------
    def _gps(self, pwr: bool):
        # Generic handling of GPS
        interval = self._settings.settings[{True:  'TGPS',
                                            False: 'TGPSB'}[pwr]]
        if time.time() - self._stateChange > (interval * MIN_TO_S_FACTOR): 
            # No GPS data in the time window allowed
            self._gpsErrors += 1

            if not pwr: # Turn of in lack of power
                self._sim_device.setGPS(False)

            if self._pending: # Old data that haven't been sent
                self._stateChange = time.time()
                self._state = 'SEND_DATA'
                self._log.debug("GPS allowed window overdue. Sending pending data!")
                self._log.debug("{}-> State: SEND_DATA".format(time.time()))
            else: # No data to be sent
                self._state = 'ERROR'
                self._log.debug("{}-> State: ERROR".format(time.time()))

        # Enable GPS
        self._sim_device.setGPS(True)

        if not self._sim_device.prepareMessage():
            self._log.debug("Failed to get GPS location, retrying...")
            self._wdt.feed() # reset timer (feed watchdog)
            self._ctrl.rtc_light_sleep(2)  # Retry every 2s

        else: # Message ready
            # GPS data ready
            self._pending = True
            self._sim_device.readBattVoltage()
            self._sim_device.setGPS(False)
            self._stateChange = time.time()
            self._state = 'SEND_DATA'
            self._log.debug("{}-> State: SEND_DATA".format(time.time()))

    # ------------------------------------------
    def _lte(self, pwr: bool):
        # Generic handling of LTE
        interval = self._settings.settings[{True:  'Tsend',
                                            False: 'TsendB'}[pwr]]
        # Turn LTE on
        self._sim_device.setLTE(True)

        if time.time() - self._stateChange > (interval * MIN_TO_S_FACTOR):
            # Data upload not achieved
            self._check_sms()
            self._gsmErrors += 1
            self._state = 'ERROR'
            self._log.debug("{}-> State: ERROR".format(time.time()))

        for i in range(RETRIES):
            if not self._ctrl.upload_data():
                self._log.debug("Attempt {}. Failed to connect to cell network, retrying...".format(i))
                self._wdt.feed() # reset timer (feed watchdog)
                self._ctrl.rtc_light_sleep(2) # Retry every 2s
            else:
                # Data sent
                self._state = 'IDLE'
                self._pending = False
                self._lastMsgSent = time.time()
                self._check_sms()
                self._log.debug("{}-> State: IDLE".format(time.time()))
                break

    # ------------------------------------------
    def _error(self):
        if self._gsmErrors > self._settings.settings['MAX_ERRORS'] or self._gpsErrors > self._settings.settings['MAX_ERRORS'] :
            self._gpsErrors = 0
            self._gsmErrors = 0
            self._log.debug("Reseting the SIM module...")
            self._sim_device.reset()
        self._state = 'IDLE'
        self._log.debug("GSM errors: {}\nGPS errors: {}".format(self._gsmErrors, self._gpsErrors))
        self._log.debug("{}-> State: IDLE".format(time.time()))

    # /********* FSM STATES WHILE DEPENDING ON BATTERY ONLY ***********************/
    
    # ------------------------------------------
    def _bat_idle(self):
        self._state = 'READ_GPS'
        self._log.debug("{}-> State: READ_GPS".format(time.time()))
        self._sim_device.turnOFF()
        self._ctrl.rtc_deep_sleep(self._settings.settings['TintB'] * MIN_TO_S_FACTOR)

    # ------------------------------------------
    def _bat_gps(self):
        if self._ctrl.moving:
            self._gps(False)
        else: # Not moving
            # GPS data ready
            self._stateChange = time.time()
            self._state = 'SEND_DATA'
            self._log.debug("Not moving! Avoiding GPS activation!")
            self._log.debug("{}-> State: SEND_DATA".format(time.time()))

    # ------------------------------------------
    def _bat_lte(self):
        self._lte(False)

    # /********* FSM STATES WHILE POWERED WITH 12V ***********************/
    
    # ------------------------------------------
    def _pwr_idle(self):
        if time.time() - self._stateChange > (self._settings.settings['Tint'] * MIN_TO_S_FACTOR):
            # Beging location update
            self._lastInterval = time.time()
            self._stateChange = self._lastInterval
            self._state = 'READ_GPS'
            self._log.debug("{}-> State: READ_GPS".format(time.time()))
    
    # ------------------------------------------
    def _pwr_gps(self):
        self._gps(True)

    # ------------------------------------------
    def _pwr_lte(self):
        self._lte(True)
