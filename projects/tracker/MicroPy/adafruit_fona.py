# SPDX-FileCopyrightText: Limor Fried/Ladyada for Adafruit Industries
# SPDX-FileCopyrightText: 2020 Brent Rubell for Adafruit Industries
#
# SPDX-License-Identifier: MIT

# pylint: disable=too-many-lines
# pylint: disable=import-outside-toplevel
# pylint: disable=consider-using-f-string

"""
`adafruit_fona`
================================================================================

CircuitPython library for the Adafruit FONA cellular module

* Author(s): ladyada, Brent Rubell

Implementation Notes
--------------------

**Software and Dependencies:**

* Adafruit CircuitPython firmware for the supported boards:
  https://github.com/adafruit/circuitpython/releases

"""
import time
from micropython import const

try:
    from typing import Optional, Tuple, Union, Literal
    from ulogging import RootLogger
    from machine import Pin, UART
except ImportError:
    pass

__version__ = "0.0.0-auto.0"
__repo__ = "https://github.com/adafruit/Adafruit_CircuitPython_FONA.git"

FONA_DEFAULT_TIMEOUT_MS = const(500)  # TODO: Check this against arduino...

# Commands
CMD_AT = b"AT"
# Replies
REPLY_OK = b"OK"
REPLY_AT = b"AT"

# Maximum number of fona800 and fona808 sockets
FONA_MAX_SOCKETS = const(6)

# FONA Versions
FONA_800_L = const(0x01)
FONA_800_H = const(0x6)
FONA_808_V1 = const(0x2)
FONA_808_V2 = const(0x3)
FONA_3G_A = const(0x4)
FONA_3G_E = const(0x5)
FONA_7000_A = const(0x7)

# FONA preferred SMS storage
FONA_SMS_STORAGE_SIM = b'"SM"'  # Storage on the SIM
FONA_SMS_STORAGE_INTERNAL = b'"ME"'  # Internal storage on the FONA


# pylint: disable=too-many-instance-attributes, too-many-public-methods
class FONA:
    """CircuitPython FONA module interface.

    :param ~busio.UART uart: FONA UART connection.
    :param ~digitalio.DigitalInOut rdt: FONA RST pin.
    :param ~digitalio.DigitalInOut ri: Optional FONA Ring Interrupt (RI) pin.
    :param bool debug: Enable debugging output.
    """

    TCP_MODE = const(0)  # TCP socket
    UDP_MODE = const(1)  # UDP socket

    AUTO     = const( 2)  # 2  - Automatic
    GSM_ONLY = const(13)  # 13 - GSM only
    LTE_ONLY = const(38)  # 38 - LTE only
    GSM_LTE  = const(51)  # 51 - GSM and LTE only

    CAT_M       = const(1)  # 1 - CAT-M
    NB_IoT      = const(2)  # 2 - NB-IoT
    BOTH_MOODES = const(3)  # 3 - CAT-M and NB-IoT

    RADIO_OFF = const(0)  # 0 --> Minimum functionality
    RADIO_ON  = const(1)  # 1 --> Full functionality

    # pylint: disable=too-many-arguments
    def __init__(
        self,
        uart: UART,
        rst: Optional[Pin] = None,
        ri: Optional[Pin] = None,
        debug: int = 0,
        log: Optional[RootLogger] = None
    ) -> None:
        self._buf = b""  # shared buffer
        self._fona_type = 0
        self._debug = debug

        if  log is None and debug >= 0:
            import ulogging
            log = ulogging.getLogger("fona")
            if debug > 0:
                log.setLevel(ulogging.DEBUG)
        self._log = log

        self._uart = uart
        self._rst = rst
        self._ri = ri
        if self._ri is not None:
            self._ri.switch_to_input()
        if not self.init_fona():
            raise RuntimeError("Unable to find FONA. Please check connections.")

    # pylint: disable=too-many-branches, too-many-statements
    def init_fona(self) -> bool:
        """Initializes FONA module."""
        if self._rst is not None:
            self.reset()

        timeout = 7000
        while timeout > 0:
            if self._send_check_reply(CMD_AT, reply=REPLY_OK):
                break
            if self._send_check_reply(CMD_AT, reply=REPLY_AT):
                break
            time.sleep(0.5)
            timeout -= 500

        if timeout <= 0:  # no response to AT, last ditch attempt
            self._send_check_reply(CMD_AT, reply=REPLY_OK)
            time.sleep(0.1)
            self._send_check_reply(CMD_AT, reply=REPLY_OK)
            time.sleep(0.1)
            self._send_check_reply(CMD_AT, reply=REPLY_OK)
            time.sleep(0.1)

        # turn off echo
        self._send_check_reply(b"ATE0", reply=REPLY_OK)
        time.sleep(0.1)

        self._read_line()
        if not self._send_check_reply(b"ATE0", reply=REPLY_OK):
            return False
        
        # # turn on hangupitude
        # self._send_check_reply(b"AT+CVHU=0", reply=REPLY_OK)
        # time.sleep(0.1)

        self._buf = b""
        self._uart.read()

        # self._uart_write(b"ATI\r\n")
        self._uart_write(b"AT+GMR\r\n")# This definitely should have the module name, but ATI may not
        self._read_line(multiline=True)

        if self._buf.find(b"SIM7000") != -1:
            self._fona_type = FONA_7000_A
        elif self._buf.find(b"SIM808 R14") != -1:
            self._fona_type = FONA_808_V2
        elif self._buf.find(b"SIM808 R13") != -1:
            self._fona_type = FONA_808_V1
        elif self._buf.find(b"SIMCOM_SIM5320A") != -1:
            self._fona_type = FONA_3G_A
        elif self._buf.find(b"SIMCOM_SIM5320E") != -1:
            self._fona_type = FONA_3G_E
        elif self._buf.find(b"SIM800 R14") != -1:
            self._fona_type = FONA_800_L

        if self._fona_type == FONA_800_L:
            # determine if SIM800H
            self._uart_write(b"AT+GMM\r\n")
            self._read_line(multiline=True)

            if self._buf.find(b"SIM800H") != -1:
                self._fona_type = FONA_800_H
        self._log.debug("FONA type: {}".format(self._fona_type))
        return True

    def factory_reset(self) -> bool:
        """Resets modem to factory configuration."""
        self._uart_write(b"ATZ\r\n")

        if not self._expect_reply(REPLY_OK):
            return False
        return True

    def reset(self) -> None:
        """Performs a hardware reset on the modem."""
        self._log.info("* Reset FONA")
        #self._rst.switch_to_output()
        self._rst.value(1)
        time.sleep(0.01)
        self._rst.value(0)
        time.sleep(0.1)
        self._rst.value(1)


    ##/********* NETWORK AND WIRELESS CONNECTION SETTINGS ***********************/

    # Uses the AT+CFUN command to set functionality (refer to AT+CFUN in manual)
    # 0 --> Minimum functionality
    # 1 --> Full functionality
    # 4 --> Disable RF
    # 5 --> Factory test mode
    # 6 --> Restarts module
    # 7 --> Offline mode
    def setFunctionality(self, option: int):
        return self._send_check_reply("AT+CFUN={}".format(option).encode(), reply=REPLY_OK)

    def setNetworkSettings(self, apn):
        return self._send_check_reply('AT+CGDCONT=1,"IP","{}"'.format(apn).encode(), reply=REPLY_OK, timeout=10000)

    # 2  - Automatic
    # 13 - GSM only
    # 38 - LTE only
    # 51 - GSM and LTE only
    def setPreferredMode(self, mode):
        return self._send_check_reply("AT+CNMP={}".format(mode).encode(), reply=REPLY_OK)

    # 1 - CAT-M
    # 2 - NB-IoT
    # 3 - CAT-M and NB-IoT
    def setPreferredLTEMode(self, mode):
        return self._send_check_reply('AT+CMNB={}'.format(mode).encode(), reply=REPLY_OK)

    def wirelessConnStatus(self):
        return self._send_parse_reply(b'AT+CNACT?', b'+CNACT: 1')

    # Open or close wireless data connection
    def openWirelessConnection(self, onoff: bool):
        if not self._send_check_reply('AT+CNACT={}'.format(int(onoff)).encode(), reply=REPLY_OK):
            return False
        self._read_line()
        reply = {True: b'PDP: ACTIVE', False: b'PDP: DEACTIVE'}[onoff]
        if self._buf.find(reply) == -1:
            return False
        return True


    def HTTP_connect(self, server: str):
        # Disconnect HTTP
        self._send_check_reply(b'AT+SHDISC', reply=REPLY_OK, timeout=10000)


        # Set up server URL
        if not self._send_check_reply('AT+SHCONF="URL","{}"'.format(server).encode(), reply=REPLY_OK, timeout=10000):
            return False

        # Set max HTTP body length
        self._send_check_reply(b'AT+SHCONF="BODYLEN",1024', reply=REPLY_OK, timeout=10000)

        # Set max HTTP header length        
        self._send_check_reply(b'AT+SHCONF="HEADERLEN",350', reply=REPLY_OK, timeout=10000)

        # HTTP build
        self._send_check_reply(b'AT+SHCONN', reply=REPLY_OK, timeout=10000)

        # Get HTTP status
        if not self._send_parse_reply(b'AT+SHSTATE?', b'+SHSTATE: 1'):
            self._log.debug(self._buf)

        self._read_line()  # eat the 'ok'

        # Clear HTTP header (HTTP header is appended)
        if not self._send_check_reply(b'AT+SHCHEAD', reply=REPLY_OK, timeout=10000):
            return False

        return True

    def HTTP_POST(self, URI, body):
        # Use fona.HTTP_addHeader() as needed before using this function
        # Then use fona.HTTP_connect() to connect to the server first
        # Make sure this is large enough for URI
        if not self._send_check_reply('AT+SHBOD="{}",{}'.format(body, len(body)).encode(), reply=REPLY_OK, timeout=10000):
            return False

        if not self._send_check_reply('AT+SHREQ="{}",3'.format(URI).encode(), reply=REPLY_OK, timeout=10000):
            return False

        # Parse response status and size
        # Example reply --> "+SHREQ: "POST",200,452"
        self._read_line(timeout=10000) #TODO some times there is an empty line before the expected message
        reply = b'+SHREQ: "POST"'
        parsed_reply = self._buf.find(reply)
        if parsed_reply == -1:
            self._log.debug(self._buf)
            return False
        else:
            parsed_reply = self._buf[parsed_reply:]

            parsed_reply = self._buf[len(reply) :]
            parsed_reply = parsed_reply.decode("utf-8")

            parsed_reply = parsed_reply.split(',')
            status  = parsed_reply[1]
            datalen = parsed_reply[2]
            self._log.debug("HTTP status: {}".format(status))
            self._log.debug("Data length: {}".format(datalen))
            try:
                status = int(status)
                datalen = int(datalen)
            except ValueError:
                self._log.error("Error parsing response status and size!")
                return False

            if status != 200:
                return False

            # Read server response
            self._get_reply('AT+SHREAD=0,{}'.format(datalen).encode(), timeout=10000)
            self._read_line() # +SHREAD: <datalen>
            self._read_line(timeout=10000)  # Print out server reply
            self._send_check_reply(b'AT+SHDISC', reply=REPLY_OK, timeout=10000) # Disconnect HTTP

            return True
        
    @property
    # returns value in mV 
    def battVoltage(self):
        # Response +CBC: <bcs>,<bcl>,<voltage>
        # <voltage> Battery voltage(mV)
        if self._send_parse_reply(b'AT+CBC', b'+CBC: ', ',', 2):
            return self._buf


    @property
    # pylint: disable=too-many-return-statements
    def version(self) -> int:
        """The version of the FONA module. Can be FONA_800_L,
        FONA_800_H, FONA_808_V1, FONA_808_V2, FONA_3G_A, FONA3G_E.
        """
        return self._fona_type

    @property
    def imei(self) -> str:
        """FONA Module's IMEI (International Mobile Equipment Identity) number."""
        self._log.debug("FONA IMEI")
        self._uart.read()

        self._uart_write(b"AT+GSN\r\n")
        self._read_line(multiline=True)
        imei = self._buf[0:15]
        return imei.decode("utf-8")

    @property
    def local_ip(self) -> Optional[str]:
        """Module's local IP address, None if not set."""
        self._uart_write(b"AT+CIFSR\r\n")
        self._read_line()
        try:
            ip_addr = self.pretty_ip(self._buf)
        except ValueError:
            return None
        return ip_addr

    @property
    def iccid(self) -> str:
        """SIM Card's unique ICCID (Integrated Circuit Card Identifier)."""
        self._log.debug("ICCID")
        self._uart_write(b"AT+CCID\r\n")
        self._read_line(timeout=2000)  # 6.2.23, 2sec max. response time
        iccid = self._buf.decode()
        return iccid

    @property
    def gprs(self) -> bool:
        """GPRS (General Packet Radio Services) power status."""
        if not self._send_parse_reply(b"AT+CGATT?", b"+CGATT: ", ":"):
            return False
        if not self._buf:
            return False
        return True

    # pylint: disable=too-many-return-statements
    def set_gprs(
        self,
        apn: Optional[Tuple[str, Optional[str], Optional[str]]] = None,
        enable: bool = True,
    ) -> bool:
        """Configures and brings up GPRS.

        :param tuple apn: The APN information
        :param bool enable: Enables or disables GPRS.
        """
        if enable:
            apn_name, apn_user, apn_pass = apn

            # enable multi connection mode (3,1)
            if not self._send_check_reply(b"AT+CIPMUX=1", reply=REPLY_OK):
                return False
            self._read_line()

            # enable receive data manually (7,2)
            if not self._send_check_reply(b"AT+CIPRXGET=1", reply=REPLY_OK):
                return False

            # disconnect all sockets
            if not self._send_check_reply(
                b"AT+CIPSHUT", reply=b"SHUT OK", timeout=20000
            ):
                return False

            if not self._send_check_reply(b"AT+CGATT=1", reply=REPLY_OK, timeout=10000):
                return False

            # set bearer profile (APN)
            if not self._send_check_reply(
                b'AT+SAPBR=3,1,"CONTYPE","GPRS"', reply=REPLY_OK, timeout=10000
            ):
                return False

            # Send command AT+SAPBR=3,1,"APN","<apn value>"
            # where <apn value> is the configured APN value.
            self._send_check_reply_quoted(
                b'AT+SAPBR=3,1,"APN",', apn_name.encode(), REPLY_OK, 10000
            )

            # send AT+CSTT,"apn","user","pass"
            self._uart.read()

            self._uart_write(b'AT+CSTT="' + apn_name.encode())

            if apn_user is not None:
                self._uart_write(b'","' + apn_user.encode())

            if apn_pass is not None:
                self._uart_write(b'","' + apn_pass.encode())
            self._uart_write(b'"\r\n')

            if not self._get_reply(REPLY_OK):
                return False

            # Set username
            if not self._send_check_reply_quoted(
                b'AT+SAPBR=3,1,"USER",', apn_user.encode(), REPLY_OK, 10000
            ):
                return False

            # Set password
            if not self._send_check_reply_quoted(
                b'AT+SAPBR=3,1,"PWD",', apn_pass.encode(), REPLY_OK, 100000
            ):
                return False

            # Open GPRS context
            if not self._send_check_reply(
                b"AT+SAPBR=1,1", reply=REPLY_OK, timeout=1850
            ):
                return False

            # Bring up wireless connection
            if not self._send_check_reply(b"AT+CIICR", reply=REPLY_OK, timeout=10000):
                return False

            if not self.local_ip:
                return False
        else:
            # reset PDP state
            if not self._send_check_reply(
                b"AT+CIPSHUT", reply=b"SHUT OK", timeout=20000
            ):
                return False

        return True

    @property
    def network_status(self) -> int:
        """The status of the cellular network."""
        self._read_line()
        self._log.debug("Network status")
        if not self._send_parse_reply(b"AT+CREG?", b"+CREG: ", idx=1):
            return False
        status = self._buf
        if not 0 <= self._buf <= 5:
            status = -1
        return status

    @property
    def rssi(self) -> float:
        """The received signal strength indicator for the cellular network
        we are connected to.
        """
        self._log.debug("RSSI")
        if not self._send_parse_reply(b"AT+CSQ", b"+CSQ: "):
            return False

        reply_num = self._buf
        rssi = 0
        if reply_num == 0:
            rssi = -115
        elif reply_num == 1:
            rssi = -111
        elif reply_num == 31:
            rssi = -52

        if 2 <= reply_num <= 30:
            # rssi = map_range(reply_num, 2, 30, -110, -54)
            rssi = -110 + (reply_num-2) * (-54+110)/(30-2)

        self._read_line()  # eat the 'ok'
        return rssi

    @property
    def gps(self) -> Tuple:
        # Module's GPS status. 
        # The run status is returned in the first element of the output Tuple
        # Response
        # +CGNSINF: <GNSS run status>,<Fix status>,<UTC date & Time>,<Latitude>,<Longitude>,
        # <MSL Altitude>,<Speed Over Ground>,<Course Over Ground>,<Fix Mode>,<Reserved1>,
        # <HDOP>,<PDOP>,<VDOP>,<Reserved2>,<GNSS Satellites in View>,<GNSS Satellites Used>,
        # <GLONASS Satellites Used>,<Reserved3>,<C/N0 max>,<HPA>,<VPA>
        self._log.debug("GPS Fix")
        if self._fona_type in (FONA_808_V2, FONA_7000_A):
            # 808 V2 uses GNS commands and doesn't have an explicit 2D/3D fix status.
            # Instead just look for a fix and if found assume it's a 3D fix.
            self._get_reply(b"AT+CGNSINF")
            reply = b'+CGNSINF: '
            parsed_reply = self._buf.find(reply)
            if parsed_reply == -1:
                self._log.error(self._buf)
                raise RuntimeError("'+CGNSINF: ' not found in the response!")
            else:
                parsed_reply = self._buf[parsed_reply:]

                parsed_reply = self._buf[len(reply) :]
                parsed_reply = parsed_reply.decode("utf-8")

                status = parsed_reply.split(',')
            # status = int(self._buf[10:11].decode("utf-8"))
            # if status == 1:
            #     status = 3  # assume 3D fix
            self._read_line()
        else:
            raise NotImplementedError(
                "FONA 808 v1 not currently supported by this library."
            )
        return status

    @gps.setter
    def gps(self, gps_on: bool = False) -> bool:
        if self._fona_type not in (FONA_3G_A, FONA_3G_E, FONA_808_V1, FONA_808_V2, FONA_7000_A):
            raise TypeError("GPS unsupported for this FONA module.")

        # check if already enabled or disabled
        if self._fona_type == FONA_808_V2:
            if not self._send_parse_reply(b"AT+CGPSPWR?", b"+CGPSPWR: ", ":"):
                return False
        self._read_line()
        if not self._send_parse_reply(b"AT+CGNSPWR?", b"+CGNSPWR: ", ":"):
            return False

        state = self._buf

        if gps_on:
            if not state:
                self._read_line()
                if self._fona_type in (FONA_808_V2, FONA_7000_A):  # try GNS
                    if not self._send_check_reply(b"AT+CGNSPWR=1", reply=REPLY_OK):
                        return False
                else:
                    if not self._send_parse_reply(b"AT+CGPSPWR=1", reply_data=REPLY_OK):
                        return False
        else:
            if self._fona_type in (FONA_808_V2, FONA_7000_A):  # try GNS
                if not self._send_check_reply(b"AT+CGNSPWR=0", reply=REPLY_OK):
                    return False
                if not self._send_check_reply(b"AT+CGPSPWR=0", reply=REPLY_OK):
                    return False

        return True

    def pretty_ip(  # pylint: disable=no-self-use, invalid-name
        self, ip
    ) -> str:
        """Converts a bytearray IP address to a dotted-quad string for printing"""
        return "%d.%d.%d.%d" % (ip[0], ip[1], ip[2], ip[3])

    def unpretty_ip(  # pylint: disable=no-self-use, invalid-name
        self, ip: str
    ) -> bytes:
        """Converts a dotted-quad string to a bytearray IP address"""
        octets = [int(x) for x in ip.split(".")]
        return bytes(octets)

    ### SMS ###

    @property
    def enable_sms_notification(self) -> bool:
        """Checks if SMS notifications are enabled."""
        if not self._send_parse_reply(b"AT+CNMI?\r\n", b"+CNMI:", idx=1):
            return False
        return self._buf

    @enable_sms_notification.setter
    def enable_sms_notification(self, enable: bool = True) -> bool:
        if enable:
            if not self._send_check_reply(b"AT+CNMI=2,1\r\n", reply=REPLY_OK):
                return False
        else:
            if not self._send_check_reply(b"AT+CNMI=2,0\r\n", reply=REPLY_OK):
                return False
        return True

    def receive_sms(self) -> Tuple[str, str]:
        """Checks for a message notification from the FONA module,
        replies back with the a tuple containing (sender, message).

        :note: This method needs to be polled consistently due to the lack
               of hw-based interrupts in CircuitPython.
        """
        if self._ri is not None:  # poll the RI pin
            if self._ri.value():
                return False, False
        if not self._uart.any():  # otherwise, poll the UART
            return False, False

        self._read_line()  # parse the rcv'd URC
        if not self._parse_reply(b"+CMTI: ", idx=1):
            return False, False
        slot = self._buf
        sender, message = self.read_sms(slot)

        if not self.delete_sms(slot):  # delete sms from module memory
            return False, False

        return sender, message.strip()

    def send_sms(self, phone_number: int, message: str) -> bool:
        """Sends a message SMS to a phone number.

        :param int phone_number: Destination phone number.
        :param str message: Message to send to the phone number.
        """
        if not hasattr(phone_number, "to_bytes"):
            raise TypeError("Phone number must be integer")

        # select SMS message format, text mode (4.2.2)
        if not self._send_check_reply(b"AT+CMGF=1", reply=REPLY_OK):
            return False

        self._uart_write(b'AT+CMGS="+' + str(phone_number).encode() + b'"' + b"\r")
        self._read_line()

        if self._buf[0] != 62:  # expect '>'
            # promoting mark ('>') not found
            return False
        self._read_line()

        # write out message and ^z
        self._uart_write((message + chr(26)).encode())

        if self._fona_type in (FONA_3G_A, FONA_3G_E):
            self._read_line(200)  # eat first 'CRLF'
            self._read_line(200)  # eat second 'CRLF'

        # read +CMGS, wait ~10sec.
        self._read_line(10000)
        if not "+CMGS" in self._buf:
            return False

        if not self._expect_reply(REPLY_OK):
            return False
        return True

    def num_sms(self, sim_storage: bool = True) -> int:
        """Returns the number of SMS messages stored in memory.

        :param bool sim_storage: SMS storage on the SIM, otherwise internal storage on FONA chip.
        """
        if not self._send_check_reply(b"AT+CMGF=1", reply=REPLY_OK):
            raise RuntimeError("Operating mode not supported by FONA module.")

        if sim_storage:  # ask how many SMS are stored
            if self._send_parse_reply(b"AT+CPMS?", FONA_SMS_STORAGE_SIM + b",", idx=1):
                return self._buf
        else:
            if self._send_parse_reply(
                b"AT+CPMS?", FONA_SMS_STORAGE_INTERNAL + b",", idx=1
            ):
                return self._buf

        self._read_line()  # eat OK
        if self._send_parse_reply(b"AT+CPMS?", b'"SM",', idx=1):
            return self._buf

        self._read_line()  # eat OK
        if self._send_parse_reply(b"AT+CPMS?", b'"SM_P",', idx=1):
            return self._buf
        return 0

    def delete_sms(self, sms_slot: int) -> bool:
        """Deletes a SMS message from a storage (internal or sim) slot

        :param int sms_slot: SMS SIM or FONA memory slot number.
        """
        if not self._send_check_reply(b"AT+CMGF=1", reply=REPLY_OK):
            return False

        if not self._send_check_reply(
            b"AT+CMGD=" + str(sms_slot).encode(), reply=REPLY_OK
        ):
            return False

        return True

    def delete_all_sms(self) -> bool:
        """Deletes all SMS messages on the FONA SIM."""
        self._read_line()
        if not self._send_check_reply(b"AT+CMGF=1", reply=REPLY_OK):
            return False

        if self._fona_type in (FONA_3G_A, FONA_3G_E):
            num_sms = self.num_sms()
            for slot in range(0, num_sms):
                if not self.delete_sms(slot):
                    return False
        else:  # DEL ALL on 808
            if not self._send_check_reply(
                b'AT+CMGDA="DEL ALL"', reply=REPLY_OK, timeout=25000
            ):
                return False
        return True

    def read_sms(self, sms_slot: int) -> Tuple[str, str]:
        """Reads and parses SMS messages from FONA device. Returns the SMS
        sender's phone number and the message contents as a tuple.

        :param int sms_slot: SMS SIM or FONA memory slot number.
        """
        if not self._send_check_reply(b"AT+CMGF=1", reply=REPLY_OK):
            return False
        if not self._send_check_reply(b"AT+CSDH=1", reply=REPLY_OK):
            return False

        self._uart_write(b"AT+CMGR=" + str(sms_slot).encode() + b"\r\n")
        self._read_line(1000)
        resp = self._buf

        # get sender
        if not self._parse_reply(b"+CMGR:", idx=1):
            return False
        sender = self._buf.strip('"')

        # get sms length
        self._buf = resp
        if not self._parse_reply(b"+CMGR:", idx=11):
            return False
        sms_len = self._buf

        self._buf = bytearray(sms_len)
        self._uart.readinto(self._buf)
        message = bytes(self._buf).decode()
        self._uart.read()
        self._read_line()  # eat 'OK'

        return sender, message

    ### Socket API (TCP, UDP) ###

    def get_host_by_name(self, hostname: str) -> Union[str, Literal[False]]:
        """Converts a hostname to a packed 4-byte IP address.

        :param str hostname: Destination server.
        """
        self._read_line()
        self._log.debug("*** Get host by name")
        if isinstance(hostname, str):
            hostname = bytes(hostname, "utf-8")

        if not self._send_check_reply(
            b'AT+CDNSGIP="' + hostname + b'"\r\n', reply=REPLY_OK
        ):
            return False

        self._read_line()
        while not self._parse_reply(b"+CDNSGIP:", idx=2):
            self._read_line()
        return self._buf

    def get_socket(self) -> int:
        """Obtains a socket, if available."""
        self._log.debug("*** Get socket")

        self._uart_write(b"AT+CIPSTATUS\r\n")
        self._read_line(100)  # OK
        self._read_line(100)  # table header

        allocated_socket = 0
        for sock in range(0, FONA_MAX_SOCKETS):  # check if INITIAL state
            self._read_line(100)
            self._parse_reply(b"C:", idx=5)
            if self._buf.strip('"') == "INITIAL" or self._buf.strip('"') == "CLOSED":
                allocated_socket = sock
                break
        # read out the rest of the responses
        for _ in range(allocated_socket, FONA_MAX_SOCKETS):
            self._read_line(100)
        if self._debug:
            self._log.debug("Allocated socket #{}".format(allocated_socket))
        return allocated_socket

    def remote_ip(self, sock_num: int) -> str:
        """Returns the IP address of the remote server.

        :param int sock_num: Desired socket.
        """
        assert (
            sock_num < FONA_MAX_SOCKETS
        ), "Provided socket exceeds the maximum number of \
                                             sockets for the FONA module."
        self._uart_write(b"AT+CIPSTATUS=" + str(sock_num).encode() + b"\r\n")
        self._read_line(100)

        self._parse_reply(b"+CIPSTATUS:", idx=3)
        return self._buf

    def socket_status(self, sock_num: int) -> bool:
        """Returns the socket connection status, False if not connected.

        :param int sock_num: Desired socket number.
        """
        assert (
            sock_num < FONA_MAX_SOCKETS
        ), "Provided socket exceeds the maximum number of \
                                             sockets for the FONA module."
        if not self._send_check_reply(b"AT+CIPSTATUS", reply=REPLY_OK, timeout=100):
            return False
        self._read_line()

        for state in range(0, sock_num + 1):  # read "C: <n>" for each active connection
            self._read_line()
            if state == sock_num:
                break
        self._parse_reply(b"C:", idx=5)

        state = self._buf

        # eat the rest of the sockets
        for _ in range(sock_num, FONA_MAX_SOCKETS):
            self._read_line()

        if not "CONNECTED" in state:
            return False

        return True

    def socket_available(self, sock_num: int) -> int:
        """Returns the amount of bytes available to be read from the socket.

        :param int sock_num: Desired socket to return bytes available from.
        """
        assert (
            sock_num < FONA_MAX_SOCKETS
        ), "Provided socket exceeds the maximum number of \
                                             sockets for the FONA module."
        if not self._send_parse_reply(
            b"AT+CIPRXGET=4," + str(sock_num).encode(),
            b"+CIPRXGET: 4," + str(sock_num).encode() + b",",
        ):
            return False
        data = self._buf
        self._log.debug("\t {} bytes available.".format(self._buf))

        self._read_line()
        self._read_line()

        return data

    def socket_connect(
        self, sock_num: int, dest: str, port: int, conn_mode: int = TCP_MODE
    ) -> bool:
        """Connects to a destination IP address or hostname.
        By default, we use conn_mode TCP_MODE but we may also use UDP_MODE.

        :param int sock_num: Desired socket number
        :param str dest: Destination dest address.
        :param int port: Destination dest port.
        :param int conn_mode: Connection mode (TCP/UDP)
        """
        self._log.debug(
                "*** Socket connect, protocol={}, port={}, ip={}".format(
                    conn_mode, port, dest
            )
        )

        self._uart.read()
        assert (
            sock_num < FONA_MAX_SOCKETS
        ), "Provided socket exceeds the maximum number of \
                                             sockets for the FONA module."

        # Query local IP Address
        self._uart_write(b"AT+CIFSR\r\n")
        self._read_line()

        # Start connection
        self._uart_write(b"AT+CIPSTART=" + str(sock_num).encode())
        if conn_mode == 0:
            self._uart_write(b',"TCP","')
        else:
            self._uart_write(b',"UDP","')
        self._uart_write(dest.encode() + b'","' + str(port).encode() + b'"\r\n')

        if not self._expect_reply(REPLY_OK):
            return False

        if not self._expect_reply(b"CONNECT OK"):
            return False
        return True

    def socket_close(self, sock_num: int) -> bool:
        """Close TCP or UDP connection

        :param int sock_num: Desired socket number.
        """
        self._log.debug("*** Closing socket #{}".format(sock_num))
        assert (
            sock_num < FONA_MAX_SOCKETS
        ), "Provided socket exceeds the maximum number of \
                                             sockets for the FONA module."

        self._uart_write(b"AT+CIPCLOSE=" + str(sock_num).encode() + b"\r\n")
        self._read_line(3000)

        if self._fona_type in (FONA_3G_A, FONA_3G_E):
            if not self._expect_reply(REPLY_OK):
                return False
        else:
            if not self._expect_reply(b"CLOSE OK"):
                return False
        return True

    def socket_read(self, sock_num: int, length: int) -> bytearray:
        """Read data from the network into a buffer.
        Returns bytes read.

        :param int sock_num: Desired socket to read from.
        :param int length: Desired length to read.
        """
        self._read_line()
        assert (
            sock_num < FONA_MAX_SOCKETS
        ), "Provided socket exceeds the maximum number of \
                                             sockets for the FONA module."
        self._log.debug("* socket read")

        self._uart_write(b"AT+CIPRXGET=2," + str(sock_num).encode() + b",")
        self._uart_write(str(length).encode() + b"\r\n")
        self._read_line()

        if not self._parse_reply(b"+CIPRXGET:"):
            return False

        return self._uart.read(length)

    def socket_write(self, sock_num: int, buffer: bytes, timeout: int = 3000) -> bool:
        """Writes bytes to the socket.

        :param int sock_num: Desired socket number to write to.
        :param bytes buffer: Bytes to write to socket.
        :param int timeout: Socket write timeout, in milliseconds.
        """
        self._read_line()
        assert (
            sock_num < FONA_MAX_SOCKETS
        ), "Provided socket exceeds the maximum number of \
                                             sockets for the FONA module."

        self._uart.read()
        self._uart_write(b"AT+CIPSEND=" + str(sock_num).encode())
        self._uart_write(b"," + str(len(buffer)).encode() + b"\r\n")
        self._read_line()

        if self._buf[0] != 62:
            # promoting mark ('>') not found
            return False

        self._uart_write(buffer + b"\r\n")
        self._read_line(timeout)

        if "SEND OK" not in self._buf.decode():
            return False

        return True

    ### UART Reply/Response Helpers ###

    def _uart_write(self, buffer: bytes) -> None:
        """UART ``write`` with optional debug that prints
        the buffer before sending.

        :param bytes buffer: Buffer of bytes to send to the bus.
        """
        self._log.debug("\t---> {}".format(buffer.decode()))
        self._uart.write(buffer)

    def _send_parse_reply(
        self, send_data: bytes, reply_data: bytes, divider: str = ",", idx: int = 0
    ) -> bool:
        """Sends data to FONA module, parses reply data returned.

        :param bytes send_data: Data to send to the module.
        :param bytes send_data: Data received by the FONA module.
        :param str divider: Separator
        """
        self._read_line()
        self._get_reply(send_data)

        if not self._parse_reply(reply_data, divider, idx):
            return False
        return True

    def _get_reply(
        self,
        data: Optional[bytes] = None,
        prefix: Optional[bytes] = None,
        suffix: Optional[bytes] = None,
        timeout: int = FONA_DEFAULT_TIMEOUT_MS,
    ) -> Tuple[int, bytes]:
        """Send data to FONA, read response into buffer.

        :param bytes data: Data to send to FONA module.
        :param bytes prefix: Data to write if ``data`` is not provided
        :param bytes suffix: Data to write following ``prefix`` if ``data is not provided
        :param int timeout: Time to wait for UART response.
        """
        self._uart.read()

        if data is not None:
            self._uart_write(data + b"\r\n")
        else:
            self._uart_write(prefix + suffix + b"\r\n")

        return self._read_line(timeout)

    def _parse_reply(self, reply: bytes, divider: str = ",", idx: int = 0) -> bool:
        """Attempts to find reply in UART buffer, reads up to divider.

        :param bytes reply: Expected response from FONA module.
        :param str divider: Divider character.
        """
        parsed_reply = self._buf.find(reply)
        if parsed_reply == -1:
            return False
        parsed_reply = self._buf[parsed_reply:]

        parsed_reply = self._buf[len(reply) :]
        parsed_reply = parsed_reply.decode("utf-8")

        parsed_reply = parsed_reply.split(divider)
        parsed_reply = parsed_reply[idx]

        try:
            self._buf = int(parsed_reply)
        except ValueError:
            self._buf = parsed_reply

        return True

    def _read_line(
        self, timeout: int = FONA_DEFAULT_TIMEOUT_MS, multiline: bool = False
    ) -> Tuple[int, bytes]:
        """Reads one or multiple lines into the buffer. Optionally prints the buffer
        after reading.

        :param int timeout: Time to wait for UART serial to reply, in seconds.
        :param bool multiline: Read multiple lines.
        """
        self._buf = b""
        reply_idx = 0
        while timeout:
            if reply_idx >= 254:
                break

            while self._uart.any():
                char = self._uart.read(1)
                if char == b"\r":
                    continue
                if char == b"\n":
                    if reply_idx == 0:  # ignore first '\n'
                        continue
                    if not multiline:  # second '\n' is EOL
                        timeout = 0
                        break
                self._buf += char
                reply_idx += 1

            if timeout == 0:
                break
            timeout -= 1
            time.sleep(0.001)
        try:
            # if self._buf != b"":
            self._log.debug("\t<--- {}".format(self._buf.decode()))
            # else:
            #     self._log.debug("_read_line timeout!")
        except Exception as exc:
            self._log.debug(exc)

        return reply_idx, self._buf

    def _send_check_reply(
        self,
        send: Optional[bytes] = None,
        prefix: Optional[bytes] = None,
        suffix: Optional[bytes] = None,
        reply: Optional[bytes] = None,
        timeout=FONA_DEFAULT_TIMEOUT_MS,
    ) -> bool:
        """Sends data to FONA, validates response.

        :param bytes send: Command.
        :param bytes prefix: Data to send if ``send`` not provided
        :param bytes suffix: Data to send after ``prefix`` if ``send`` not provided
        :param bytes reply: Expected response from module.
        :param int timeout: Time to wait for UART serial to reply, in seconds.
        """
        self._read_line()
        if send is None:
            if not self._get_reply(prefix=prefix, suffix=suffix, timeout=timeout):
                return False
        else:
            if not self._get_reply(send, timeout=timeout):
                return False

        if not self._buf == reply:
            return False

        return True

    def _send_check_reply_quoted(
        self,
        prefix: bytes,
        suffix: bytes,
        reply: bytes,
        timeout: int = FONA_DEFAULT_TIMEOUT_MS,
    ) -> bool:
        """Send prefix, ", suffix, ", and a newline. Verify response against reply.

        :param bytes prefix: Command prefix.
        :param bytes prefix: Command ", suffix, ".
        :param bytes reply: Expected response from module.
        :param int timeout: Time to expect reply back from FONA, in milliseconds.
        """
        self._buf = b""

        self._get_reply_quoted(prefix, suffix, timeout)

        if reply not in self._buf:
            return False
        return True

    def _get_reply_quoted(
        self, prefix: bytes, suffix: bytes, timeout: int
    ) -> Tuple[int, bytes]:
        """Send prefix, ", suffix, ", and newline.
        Returns: Response (and also fills buffer with response).

        :param bytes prefix: Command prefix.
        :param bytes prefix: Command ", suffix, ".
        :param int timeout: Time to expect reply back from FONA, in milliseconds.
        """
        self._uart.read()

        self._uart_write(prefix + b'"' + suffix + b'"\r\n')

        return self._read_line(timeout)

    def _expect_reply(self, reply: bytes, timeout: int = 10000) -> bool:
        """Reads line from FONA module and compares to reply from FONA module.

        :param bytes reply: Expected reply from module.
        :param int timeout: Time to wait for UART serial to reply, in seconds.
        """
        self._read_line(timeout)
        if reply not in self._buf:
            return False
        return True
