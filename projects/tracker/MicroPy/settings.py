
# pylint: disable=import-outside-toplevel
# pylint: disable=consider-using-f-string

import json
try:
    from typing import Optional
    from ulogging import RootLogger
except ImportError:
    pass

class Settings:    
    def __init__(
        self,
        debug: int = 0,
        log: Optional[RootLogger] = None
        )-> None:

        if log is None and debug >= 0:
            import ulogging
            log = ulogging.getLogger("settings")
            if debug > 0:
                log.setLevel(ulogging.DEBUG)
        self._log = log

        # Load system settings
        try:
            with open("settings.json",'r', encoding='utf-8') as f:
                self.settings = json.load(f)
        except Exception as err:
            self._log.error(err)
            # Set to default values
            self.settings = {
                'MAX_ERRORS': 3,
                'Tint': 60, # min
                'TintB': 720, # min
                'TsendB': 10, # min
                'SMART': True,
                'GPS':  10, # min
                'TGPSB': 10, # min
                'Tsend': 3,  # min
            }
            self.saveSettings()

    # --------------------------------------------------------
    def saveSettings(self):
        with open("settings.json",'w', encoding='utf-8') as f:
            json.dump(self.settings, f)

    # --------------------------------------------------------
    def updateSetting(self, msg: str):
        # Modified settings in json sms string
        # ex: '{"MAX_ERRORS": 3, "Tint": 60, "TintB": 720, "TsendB": 10}'
        try:
            new_configs = json.loads(msg)
            new_data = False
            for k in new_configs.keys():
                if self.settings[k] != new_configs[k]:
                    new_data = True
                    self.settings[k] = new_configs[k]
            if new_data:
                self.saveSettings()
        except Exception as err:
            self._log.error(err)


