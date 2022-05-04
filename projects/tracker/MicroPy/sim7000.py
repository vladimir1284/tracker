
# pylint: disable=import-outside-toplevel
# pylint: disable=consider-using-f-string
 
import time
from adafruit_fona import FONA
# pylint: disable=wrong-import-order
# pylint: disable=import-error
from machine import Pin, UART
try:
    from typing import Optional
    from ulogging import RootLogger
except ImportError:
    pass

SERVER   = "http://trailerrental.pythonanywhere.com"
ADDR	 = "/towit/tracker_data"
APN_NAME = "hologram"
FONA_TX  = 19
FONA_RX  = 18
PWRKEY   = 4
SIMPWR   = 25

class Sim7000:

    def __init__(
        self, 
        debug: int = 0,
        log: Optional[RootLogger] = None
        )-> None:

        self._simpwr = Pin(SIMPWR, Pin.OUT)

        if log is None and debug >= 0:
            import ulogging
            log = ulogging.getLogger("sim7000")
            if debug > 0:
                log.setLevel(ulogging.DEBUG)
        self._log = log

        self.gps_data = []
        
        # Reset the module for a clean startup
        self.reset()

        self._log.debug("Starting configuration...")
        # Hardware serial:
        uart = UART(1, baudrate=9600, tx=FONA_TX, rx=FONA_RX) # TODO baudrate=115200
        
        pk = Pin(PWRKEY, Pin.OUT)

        self._fona = FONA(uart, pwrkey = pk, debug = debug)#, log = self._log)

        self._log.debug("Module IMEI: {}".format(self._fona.imei))

        # Turn off modem
        self._fona.setFunctionality(self._fona.RADIO_OFF)
        
        # Configure
        self._fona.setNetworkSettings(APN_NAME)
        self._fona.setPreferredMode(self._fona.LTE_ONLY) # Use LTE only, not 2G
        self._fona.setPreferredLTEMode(self._fona.CAT_M) # Use LTE CAT-M only, not NB-IoT

    #--------------------------------------------------------------------
    def prepareMessage(self) -> bool:
        gps_list = self._fona.gps
        self._log.debug(gps_list)
        if int(gps_list[0]) != 1:
            return False
        else:
            # Parse GPS response
            # +CGNSINF: <GNSS run status>,<Fix status>,<UTC date & Time>,<Latitude>,<Longitude>,
            # <MSL Altitude>,<Speed Over Ground>,<Course Over Ground>,<Fix Mode>,<Reserved1>,
            # <HDOP>,<PDOP>,<VDOP>,<Reserved2>,<GNSS Satellites in View>,<GNSS Satellites Used>,
            # <GLONASS Satellites Used>,<Reserved3>,<C/N0 max>,<HPA>,<VPA>
            # ['1', '1', '20020918165724.000', '29.729532', '-95.649097', '33.600', '0.00', '78.4', '1', '', '3.2', '3.3', '1.0', '', '11', '4', '', '', '33', '', '']
            try:
                HDOP = float(gps_list[10])
                if HDOP < 20:
                    latitude  = float(gps_list[3])
                    longitude = float(gps_list[4])
                    speed_kph = float(gps_list[6])
                    heading   = float(gps_list[7])
                    sats      = int(gps_list[15])
                else:
                    return False
            except Exception as err:
                self._log.debug(err)
                return False

            # pending = True

            # # TODO handle events
            # msg = "%s,%i,%i,%i,%.5f,%.5f,%i,%i,%i,%i" % (self._fona.imei, seq_num, mode, 0, latitude, longitude, int(speed_kph), heading, 0, self._fona.battVoltage)
            # seq_num += 1

            self._log.debug("\n Latitude: {:.5f}\n Longitude: {:.5f}\n Speed: {:.1f}km/h\n Heading: {} deg\n N sats: {}\n HDOP: {}".
                            format(latitude, longitude, speed_kph, heading, sats, HDOP))
            self.gps_data = [latitude, longitude, speed_kph, heading, sats, HDOP]
            # self._log.debug(msg)

            return True

    #--------------------------------------------------------------------
    def checkSMS(self)->str:
        # Returns the body of the first sms if any and deletes it
        # Returns an empty string "" if there are problems

        self._fona.setFunctionality(self._fona.RADIO_ON) # AT+CFUN=1
        time.sleep(0.1)

        numSMS = self._fona.num_sms()
        self._log.debug("Number of SMS available: {}".format(numSMS))
        
        if numSMS > 0:
            # Retrieve SMS value.
            for i in range(numSMS):
                sms = self._fona.read_sms(i)[1]
                self._log.debug("Read SMS in slot {}:\n{}".format(i, sms))

                # Delete the original message after it is processed.
                if self._fona.delete_sms(i):
                    self._log.debug("SMS in slot {} have been deleted!".format(i))
                    return sms
                else:
                    self._log.error("Couldn't delete SMS in slot {}!".format(i))
        else:
            self._log.debug("There are no SMS available!")

        return ""


    #--------------------------------------------------------------------
    def setGPS(self, gps_on: bool):
        self._fona.gps = gps_on

    #--------------------------------------------------------------------
    def turnOFF(self):
        self._simpwr.value(0)

    #--------------------------------------------------------------------
    def turnON(self):
        self._simpwr.value(1)
        time.sleep(3) # SIM7000 takes about 3s to turn on


    #--------------------------------------------------------------------
    def reset(self):
        self.turnOFF()
        time.sleep(0.1) # Short pause to let the capacitors discharge
        self.turnON()

    #--------------------------------------------------------------------
    def uploadData(self):
        # Open wireless connection if not already activated
        if not self._fona.wirelessConnStatus():
            if not self._fona.openWirelessConnection(True):
                self._log.debug("Failed to enable connection, retrying...")
                return False
            self._log.debug("Data enabled!")
        else:
            self._log.debug("Data already enabled!")

        # HTTP send data
        if not self._fona.HTTP_connect(SERVER):
            self._log.debug("Failed to connect to server!")
            return False

        # Upload data
        if not self._fona.HTTP_POST(ADDR, self.prepareMessage()):
            self._log.debug("Failed to upload!") # Send GPS location
            return False
