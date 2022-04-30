import time

class Adafruit_FONA:
    port_CA_FONA = 0
    CID_CA_FONA = 0

    def __init__(self, rst, log = None, debug=False):

        if log is None and debug >= 0:
            import ulogging
            log = ulogging.getLogger("picoweb")
            if debug > 0:
                log.setLevel(ulogging.DEBUG)
        self.log = log

        _rstpin = rst

        # apn = "FONAnet"
        apn = F("")
        apnusername = 0
        apnpassword = 0
        self.mySerial = 0
        httpsredirect = False
        useragent = "FONA"
        ok_reply = "OK"

    def type(self):
        return _type

    def begin(self, port, FONA_PWRKEY):
        self.mySerial = port

        time.sleep_ms(1000)

        self.log.debug("Attempting to open comm with ATs")
        # give 7 seconds to reboot
        timeout = 5500

        while timeout > 0:
            while self.mySerial.available():
                self.mySerial.read()
            if (self.sendCheckReply("AT", ok_reply))
                break
            while self.mySerial.available():
                self.mySerial.read()
            if (self.sendCheckReply("AT", "AT"))
                break
            powerOn(FONA_PWRKEY) # Power on the module
            time.sleep_ms(500)
            # timeout -= 500
            timeout -= 1100
            timerWrite(timer, 0) #reset timer (feed watchdog)

        if timeout <= 0:
            self.log.debug("Timeout: No response to AT... last ditch attempt.")

            for i in range(3):
                self.sendCheckReply("AT", ok_reply)
                time.sleep_ms(100)

        # turn off Echo!
        self.sendCheckReply("ATE0", ok_reply)
        time.sleep_ms(100)

        if (not self.sendCheckReply("ATE0", ok_reply))        
            return False

        # turn on hangupitude
        if _rstpin != 99:
        self.sendCheckReply("AT+CVHU=0", ok_reply)

        time.sleep_ms(100)
        self.flushInput()

        self.log.debug("\t---> AT+GMR") # This definitely should have the module name, but ATI may not

        # self.mySerial->println("ATI")
        self.mySerial.println("AT+GMR")
        self.readline(500, True)

        self.log.debug("\t<--- %s" % replybuffer)

        if (replybuffer.find("SIM808 R14") != -1):
            _type = "SIM808_V2"
        elif (replybuffer.find("1418B03SIM808M32_BT_EAT") != -1):
            _type = "SIM808_V2" #For Boards with Bluetooth and Extended AT commands for it
        elif (replybuffer.find("SIM808 R13") != -1):
            _type = "SIM808_V1"
        elif (replybuffer.find("SIM800 R13") != -1):
            _type = "SIM800L"
        elif (replybuffer.find("SIMCOM_SIM5320A") != -1):
            _type = "SIM5320A"
        elif (replybuffer.find("SIMCOM_SIM5320E") != -1):
            _type = "SIM5320E"
        elif (replybuffer.find("SIM7000") != -1):
            _type = "SIM7000"
        elif (replybuffer.find("SIM7070") != -1):
            _type = "SIM7070"
        elif (replybuffer.find("SIM7500") != -1):
            _type = "SIM7500"
        elif (replybuffer.find("SIM7600") != -1):
            _type = "SIM7600"

        if (_type == "SIM800L"):
            # determine if L or H
            self.log.debug("\t---> AT+GMM")

            self.mySerial.println("AT+GMM")
            self.self.readline(500, True)

            self.log.debug("\t<--- %s" % replybuffer)

            if (replybuffer.find("SIM800H") != -1):
                _type = "SIM800H"

        if (FONA_PREF_SMS_STORAGE):
            self.sendCheckReply(F("AT+CPMS=" FONA_PREF_SMS_STORAGE "," FONA_PREF_SMS_STORAGE "," FONA_PREF_SMS_STORAGE), ok_reply)

        return True

    #******** Serial port *******************************************
    def setBaudrate(self, baud):
    return self.sendCheckReply("AT+IPREX=", baud, ok_reply)

    def setBaudrate(self, baud):
    return self.sendCheckReply("AT+IPR=", baud, ok_reply)

    #******** POWER, BATTERY & ADC *******************************************

    # returns value in mV () 
    def getBattVoltage(self):
    if _type == SIM5320A or _type == SIM5320E or _type == SIM7500 or _type == SIM7600:
        b = self.sendParseReplyFloat("AT+CBC", "+CBC: ", &f, ',', 0)
        *v = f * 1000
        return b
    else:
        return self.sendParseReply("AT+CBC", "+CBC: ", v, ',', 2)

    # returns value in mV () 
    Adafruit_FONA_3G::getBattVoltage(v)
    b = self.sendParseReply("AT+CBC", "+CBC: ", &f, ',', 2)
    *v = f * 1000
    return b

    # powers on the module 
    def powerOn( FONA_PWRKEY):
    pinMode(FONA_PWRKEY, OUTPUT)
    digitalWrite(FONA_PWRKEY, LOW)

    # See spec sheets for your particular module
    if _type <= SIM808_V2:
        time.sleep_ms(1050)
    elif _type == SIM5320A or _type == SIM5320E:
        time.sleep_ms(180) # For SIM5320
    elif _type == SIM7000 or _type == SIM7070:
        time.sleep_ms(1100) # At least 1s
    elif _type == SIM7500 or _type == SIM7600:
        time.sleep_ms(500)

    digitalWrite(FONA_PWRKEY, HIGH)

    # powers down the SIM module 
    def powerDown(void)
    if _type == SIM7500 or _type == SIM7600:
        if (not self.sendCheckReply("AT+CPOF", ok_reply))
        return False
    else:
        if (not self.sendCheckReply("AT+CPOWD=1", "NORMAL POWER DOWN")) # Normal power off
        return False

    return True

    # powers down the SIM5320 
    Adafruit_FONA_3G::powerDown(void)
    if (not self.sendCheckReply("AT+CPOF", ok_reply))
        return False

    return True

    # returns the percentage charge of battery as reported by sim800 
    def getBattPercent(p)
    return self.sendParseReply("AT+CBC", "+CBC: ", p, ',', 1)

    def getADCVoltage(v)
    return self.sendParseReply("AT+CADC?", "+CADC: 1,", v)

    #******** NETWORK AND WIRELESS CONNECTION SETTINGS **********************

    # Uses the AT+CFUN command to set functionality (refer to AT+CFUN in manual)
    # 0 --> Minimum functionality
    # 1 --> Full functionality
    # 4 --> Disable RF
    # 5 --> Factory test mode
    # 6 --> Restarts module
    # 7 --> Offline mode
    def setFunctionality( option)
    return self.sendCheckReply("AT+CFUN=", option, ok_reply)

    # 2  - Automatic
    # 13 - GSM only
    # 38 - LTE only
    # 51 - GSM and LTE only
    def setPreferredMode( mode)
    return self.sendCheckReply("AT+CNMP=", mode, ok_reply)

    # 1 - CAT-M
    # 2 - NB-IoT
    # 3 - CAT-M and NB-IoT
    def setPreferredLTEMode( mode)
    return self.sendCheckReply("AT+CMNB=", mode, ok_reply)

    # Useful for choosing a certain carrier only
    # For example, AT&T uses band 12 in the US for LTE CAT-M
    # whereas Verizon uses band 13
    # Mode: "CAT-M" or "NB-IOT"
    # Band: The cellular EUTRAN band number
    def setOperatingBand(self, mode,  band)

    sprintf(cmdBuff, "AT+CBANDCFG="%s",%i" % (mode, band))

    return self.sendCheckReply(cmdBuff, ok_reply)

    # Sleep mode reduces power consumption significantly while remaining registered to the network
    # NOTE: USB port must be disconnected before this will take effect
    def enableSleepMode(onoff)
    return self.sendCheckReply("AT+CSCLK=", onoff, ok_reply)

    # Set e-RX parameters
    # Mode options:
    # 0  Disable the use of eDRX
    # 1  Enable the use of eDRX
    # 2  Enable the use of eDRX and auto report
    # 3  Disable the use of eDRX(Reserved)

    # Connection type options:
    # 4 - CAT-M
    # 5 - NB-IoT

    # See AT command manual for eDRX values (options 0-15)

    # NOTE: Network must support eDRX mode
    def set_eDRX( mode,  connType, eDRX_val)
    if len(eDRX_val) > 4:
        return False


    sprintf(auxStr, "AT+CEDRXS=%i,%i,"%s"" % (mode, connType, eDRX_val))

    return self.sendCheckReply(auxStr, ok_reply)

    # NOTE: Network must support PSM and modem needs to restart before it takes effect
    def enablePSM(onoff)
    return self.sendCheckReply("AT+CPSMS=", onoff, ok_reply)
    # Set PSM with custom TAU and active time
    # For both TAU and Active time, leftmost 3 bits represent the multiplier and rightmost 5 bits represent the value in bits.

    # For TAU, left 3 bits:
    # 000 10min
    # 001 1hr
    # 010 10hr
    # 011 2s
    # 100 30s
    # 101 1min
    # For Active time, left 3 bits:
    # 000 2s
    # 001 1min
    # 010 6min
    # 111 disabled

    # Note: Network decides the final value of the TAU and active time.
    def enablePSM(onoff, TAU_val, activeTime_val)
    { # AT+CPSMS command
    if len(activeTime_val) > 8:
        return False
    if len(TAU_val) > 8:
        return False

    sprintf(auxStr, "AT+CPSMS=%i,,,"%s","%s"" % (onoff, TAU_val, activeTime_val))

    return self.sendCheckReply(auxStr, ok_reply)

    # Enable, disable, or set the blinking frequency of the network status LED
    # Default settings are the following:
    # Not connected to network --> 1,64,800
    # Connected to network     --> 2,64,3000
    # Data connection enabled  --> 3,64,300
    def setNetLED(onoff,  mode,  timer_on,  timer_off)
    if onoff:
        if (not self.sendCheckReply("AT+CNETLIGHT=1", ok_reply))
        return False

        if mode > 0:

        sprintf(auxStr, "AT+SLEDS=%i,%i,%i" % (mode, timer_on, timer_off))

        return self.sendCheckReply(auxStr, ok_reply)
        else:
        return False
    else:
        return self.sendCheckReply("AT+CNETLIGHT=0", ok_reply)

    #******** SIM **********************************************************

    def unlockSIM(pin)
    sendbuff[14] = "AT+CPIN="
    sendbuff[8] = pin[0]
    sendbuff[9] = pin[1]
    sendbuff[10] = pin[2]
    sendbuff[11] = pin[3]
    sendbuff[12] = '\0'

    return self.sendCheckReply(sendbuff, ok_reply)

    def getSIMCCID(ccid)
    self.getReply("AT+CCID")
    # up to 28 chars for reply, 20 char total ccid
    if replybuffer[0] == '+':
        # fona 3g?
        strncpy(ccid, replybuffer + 8, 20)
    else:
        # fona 800 or 800
        strncpy(ccid, replybuffer, 20)
    ccid[20] = 0

    self.readline() # eat 'OK'

    return len(ccid)

    #******** IMEI *********************************************************

    def getIMEI(imei)
    self.getReply("AT+GSN")

    # up to 15 chars
    strncpy(imei, replybuffer, 15)
    imei[15] = 0

    self.readline() # eat 'OK'

    return len(imei)

    #******** NETWORK ******************************************************

    def getNetworkStatus(void)
    status

    if _type >= SIM7000:
        if (not self.sendParseReply("AT+CGREG?", "+CGREG: ", &status, ',', 1))
        return 0
    else:
        if (not self.sendParseReply("AT+CREG?", "+CREG: ", &status, ',', 1))
        return 0

    return status

    def getRSSI(void)
    reply

    if (not self.sendParseReply("AT+CSQ", "+CSQ: ", &reply))
        return 0

    return reply

    #******** AUDIO ******************************************************

    def setAudio( a)
    # For SIM5320, 1 is headset, 3 is speaker phone, 4 is PCM interface
    if (_type == SIM5320A or _type == SIM5320E) and (a != 1 and a != 3 and a != 4):
        return False
    # For SIM7500, 1 is headset, 3 is speaker phone
    elif (_type == SIM7500 or _type == SIM7600) and (a != 1 and a != 3):
        return False
    # For SIM800, 0 is main audio channel, 1 is aux, 2 is main audio channel (hands-free), 3 is aux channel (hands-free), 4 is PCM channel
    elif a > 4:
        return False # 0 is headset, 1 is external audio

    if _type <= SIM808_V2:
        return self.sendCheckReply("AT+CHFA=", a, ok_reply)
    else:
        return self.sendCheckReply("AT+CSDVC=", a, ok_reply)

    def getVolume(void)
    reply

    if (not self.sendParseReply("AT+CLVL?", "+CLVL: ", &reply))
        return 0

    return reply

    def setVolume( i)
    return self.sendCheckReply("AT+CLVL=", i, ok_reply)

    def playDTMF(dtmf)
    str[0] = '"'
    str[1] = dtmf
    str[2] = '"'
    str[3] = 0
    return self.sendCheckReply("AT+CLDTMF=3,", str, ok_reply)

    def playToolkitTone( t,  len)
    return self.sendCheckReply("AT+STTONE=1,", t, len, ok_reply)

    Adafruit_FONA_3G::playToolkitTone( t,  len)
    if (not self.sendCheckReply("AT+CPTONE=", t, ok_reply))
        return False
    time.sleep_ms(len)
    return self.sendCheckReply("AT+CPTONE=0", ok_reply)

    def setMicVolume( a,  level)
    # For SIM800, 0 is main audio channel, 1 is aux, 2 is main audio channel (hands-free), 3 is aux channel (hands-free)
    if a > 3:
        return False

    return self.sendCheckReply("AT+CMIC=", a, level, ok_reply)

    #******** FM RADIO ******************************************************

    def FMradio( onoff,  a)
    if not onoff:
        return self.sendCheckReply("AT+FMCLOSE", ok_reply)

    # 0 is headset, 1 is external audio
    if a > 1:
        return False

    return self.sendCheckReply("AT+FMOPEN=", a, ok_reply)

    def tuneFMradio( station)
    # Fail if FM station is outside allowed range.
    if (station < 870) or (station > 1090):
        return False

    return self.sendCheckReply("AT+FMFREQ=", station, ok_reply)

    def setFMVolume( i)
    # Fail if volume is outside allowed range (0-6).
    if i > 6:
        return False
    # Send FM volume command and verify response.
    return self.sendCheckReply("AT+FMVOLUME=", i, ok_reply)

    int8_tdef getFMVolume()
    level

    if (not self.sendParseReply("AT+FMVOLUME?", "+FMVOLUME: ", &level))
        return 0

    return level

    int8_tdef getFMSignalLevel( station)
    # Fail if FM station is outside allowed range.
    if (station < 875) or (station > 1080):
        return -1

    # Send FM signal level query command.
    # Note, need to explicitly send timeout so right overload is chosen.
    self.getReply("AT+FMSIGNAL=", station, FONA_DEFAULT_TIMEOUT_MS)
    # Check response starts with expected value.
    p = prog_char_strstr(replybuffer, PSTR("+FMSIGNAL: "))
    if p == 0:
        return -1
    p += 11
    # Find second colon to get start of signal quality.
    p = strchr(p, ':')
    if p == 0:
        return -1
    p += 1
    # Parse signal quality.
    int8_t level = int(p)
    self.readline() # eat the "OK"
    return level

    #******** PWM/BUZZER *************************************************

    def setPWM( period,  duty)
    if period > 2000:
        return False
    if duty > 100:
        return False

    return self.sendCheckReply("AT+SPWM=0,", period, duty, ok_reply)

    #******** CALL PHONES *************************************************
    def callPhone(number)
    sendbuff[35] = "ATD"
    strncpy(sendbuff + 3, number, min(30, (int)len(number)))

    x = len(sendbuff)

    sendbuff[x] = ';'
    sendbuff[x + 1] = 0
    #self.log.debug(sendbuff)

    if _type == SIM7500:
        self.sendCheckReply("AT+CSDVC=3", ok_reply) # Enable speaker output

    return self.sendCheckReply(sendbuff, ok_reply)

    def getCallStatus(void)
    phoneStatus

    if (not self.sendParseReply("AT+CPAS", "+CPAS: ", &phoneStatus))
        return FONA_CALL_FAILED # 1, since 0 is actually a known, good reply

    return phoneStatus # 0 ready, 2 unknown, 3 ringing, 4 call in progress

    def hangUp(void)
    return self.sendCheckReply("ATH0", ok_reply)

    Adafruit_FONA_3G::hangUp(void)
    self.getReply("ATH")

    return (prog_char_strstr(replybuffer, (prog_char *)"VOICE CALL: END") != 0)

    def hangUp(void)
    # return self.sendCheckReply("ATH", ok_reply); # For SIM7500 this only works when AT+CVHU=0
    return self.sendCheckReply("AT+CHUP", ok_reply)

    def pickUp(void)
    return self.sendCheckReply("ATA", ok_reply)

    Adafruit_FONA_3G::pickUp(void)
    return self.sendCheckReply("ATA", "VOICE CALL: BEGIN")

    def onIncomingCall():

    self.log.debug("> ")
    self.log.debug("Incoming call...")

    def _incomingCall = True

    def _incomingCall = False

    def callerIdNotification( enable,  interrupt)
    if enable:
        attachInterrupt(interrupt, onIncomingCall, FALLING)
        return self.sendCheckReply("AT+CLIP=1", ok_reply)

    detachInterrupt(interrupt)
    return self.sendCheckReply("AT+CLIP=0", ok_reply)

    def incomingCallNumber(phonenum)
    #+CLIP: "<incoming phone number>",145,"",0,"",0
    if notdef _incomingCall:
        return False

    self.readline()
    while not prog_char_replybuffer.find((prog_char *:"RING") == -1)
    {
        self.flushInput()
        self.readline()

    self.readline() #reads incoming phone number line

    parseReply("+CLIP: "", phonenum, '"')

    self.log.debug("Phone Number: ")
    self.log.debug(replybuffer)

    def _incomingCall = false
    return true

    /********* SMS **********************************************************/

    def getSMSInterrupt(void)
    reply

    if (!self.sendParseReply("AT+CFGRI?", "+CFGRI: ", &reply))
        return 0

    return reply

    def setSMSInterrupt( i)
    return self.sendCheckReply("AT+CFGRI=", i, ok_reply)

    int8_tdef getNumSMS(void)
    numsms

    # get into text mode
    if (!self.sendCheckReply("AT+CMGF=1", ok_reply))
        return -1

    # ask how many sms are stored
    if (self.sendParseReply("AT+CPMS?", F(FONA_PREF_SMS_STORAGE ","), &numsms))
        return numsms
    if (self.sendParseReply("AT+CPMS?", ""SM",", &numsms))
        return numsms
    if (self.sendParseReply("AT+CPMS?", ""SM_P",", &numsms))
        return numsms
    return -1

    # Reading SMS's is a bit involved so we don't use helpers that may cause delays or debug
    # printouts!
    def readSMS( i, char *smsbuff,
                                    maxlen, readlen)
    # text mode
    if (!self.sendCheckReply("AT+CMGF=1", ok_reply))
        return false

    # show all text mode parameters
    if (!self.sendCheckReply("AT+CSDH=1", ok_reply))
        return false

    # parse out the SMS len
    thesmslen = 0

    self.log.debug("\t--. ")
    self.log.debug("AT+CMGR=")
    self.log.debug(i)

    #self.getReply("AT+CMGR=", i, 1000);  #  do not print debug!
    self.mySerial->print("AT+CMGR=")
    self.mySerial->println(i)
    self.readline(1000); # timeout

    #self.log.debug("Reply: "); self.log.debug(replybuffer)
    # parse it out...

    self.log.debug(replybuffer)

    if (!parseReply("+CMGR:", &thesmslen, ',', 11))
    {
        *readlen = 0
        return false

    readRaw(thesmslen)

    self.flushInput()

    thelen = min(maxlen, ()strlen(replybuffer))
    strncpy(smsbuff, replybuffer, thelen)
    smsbuff[thelen] = 0; # end the string

    self.log.debug(replybuffer)

    *readlen = thelen
    return true

    # Retrieve the sender of the specified SMS message and copy it as a string to
    # the sender buffer.  Up to senderlen characters of the sender will be copied
    # and a null terminator will be added if less than senderlen charactesr are
    # copied to the result.  Returns true if a result was successfully retrieved,
    # otherwise false.
    def getSMSSender( i, char *sender, int senderlen)
    # Ensure text mode and all text mode parameters are sent.
    if (!self.sendCheckReply("AT+CMGF=1", ok_reply))
        return false
    if (!self.sendCheckReply("AT+CSDH=1", ok_reply))
        return false

    self.log.debug("\t--. ")
    self.log.debug("AT+CMGR=")
    self.log.debug(i)

    # Send command to retrieve SMS message and parse a line of response.
    self.mySerial->print("AT+CMGR=")
    self.mySerial->println(i)
    self.readline(1000)

    self.log.debug(replybuffer)

    # Parse the second field in the response.
    result = self.parseReplyQuoted("+CMGR:", sender, senderlen, ',', 1)
    # Drop any remaining data from the response.
    self.flushInput()
    return result

    def sendSMS(self, char *smsaddr, char *smsmsg)
    if (!self.sendCheckReply("AT+CMGF=1", ok_reply))
        return false

    char sendcmd[30] = "AT+CMGS=""
    strncpy(sendcmd + 9, smsaddr, 30 - 9 - 2); # 9 bytes beginning, 2 bytes for close quote + null
    sendcmd[strlen(sendcmd)] = '"'

    if (!self.sendCheckReply(sendcmd, "> "))
        return false

    self.log.debug("\t--. ")
    self.log.debug(smsmsg)

    # no need for extra NEWLINE characters self.mySerial->println(smsmsg)
    # no need for extra NEWLINE characters self.mySerial->println()
    self.mySerial->print(smsmsg)
    self.mySerial->write(0x1A)

    # self.log.debug("^Z")

    if ((_type == SIM5320A) || (_type == SIM5320E) || (_type >= SIM7000))
    {
        # Eat two sets of CRLF
        self.readline(200)
        #self.log.debug("Line 1: "); self.log.debug(strlen(replybuffer))
        self.readline(200)
        #self.log.debug("Line 2: "); self.log.debug(strlen(replybuffer))
    self.readline(30000); # read the +CMGS reply, wait up to 30s
    #self.log.debug("Line 3: "); self.log.debug(strlen(replybuffer))
    if (strstr(replybuffer, "+CMGS") == 0)
    {
        return false
    self.readline(1000); # read OK
    #self.log.debug("* "); self.log.debug(replybuffer)

    if (strcmp(replybuffer, "OK") != 0)
    {
        return false

    return true

    def deleteSMS( i)
    if (!self.sendCheckReply("AT+CMGF=1", ok_reply))
        return false
    # delete an sms
    char sendbuff[12] = "AT+CMGD=000"
    sendbuff[8] = (i / 100) + '0'
    i %= 100
    sendbuff[9] = (i / 10) + '0'
    i %= 10
    sendbuff[10] = i + '0'

    return self.sendCheckReply(sendbuff, ok_reply, 2000)

    def deleteAllSMS()
    if (!self.sendCheckReply("A)T+CMGF=1", ok_reply))
        return false
    return self.sendCheckReply("AT+CMGD=1,4", ok_reply, 2000)

    /********* USSD *********************************************************/

    def sendUSSD(char *ussdmsg, char *ussdbuff,  maxlen, readlen)
    if (!self.sendCheckReply("AT+CUSD=1", ok_reply))
        return false

    char sendcmd[30] = "AT+CUSD=1,""
    strncpy(sendcmd + 11, ussdmsg, 30 - 11 - 2); # 11 bytes beginning, 2 bytes for close quote + null
    sendcmd[strlen(sendcmd)] = '"'

    if (!self.sendCheckReply(sendcmd, ok_reply))
    {
        *readlen = 0
        return false
    else
    {
        self.readline(10000); # read the +CUSD reply, wait up to 10 seconds!!!
        #self.log.debug("* "); self.log.debug(replybuffer)
        char *p = prog_char_strstr(replybuffer, PSTR("+CUSD: "))
        if (p == 0)
        {
        *readlen = 0
        return false
        p += 7; #+CUSD
        # Find " to get start of ussd message.
        p = strchr(p, '"')
        if p == 0:
        *readlen = 0
        return False
        p += 1 #"
        # Find " to get end of ussd message.
        strend = strchr(p; '"')

        lentocopy = min(maxlen - 1, strend - p)
        strncpy(ussdbuff, p, lentocopy + 1)
        ussdbuff[lentocopy] = 0
        *readlen = lentocopy
    return True

    #******** TIME *********************************************************

    #
    # def enableNetworkTimeSync( onoff) {
    # if (onoff) {
        # if (! self.sendCheckReply("AT+CLTS=1", ok_reply))
        # return false
    # } else {
        # if (! self.sendCheckReply("AT+CLTS=0", ok_reply))
        # return false
    # }
    # 
    # self.flushInput(); # eat any 'Unsolicted Result Code'
    # 
    # return true
    # }
    # 

    def enableNTPTimeSync( onoff,  ntpserver)
    if onoff:
        if (not self.sendCheckReply("AT+CNTPCID=1", ok_reply))
        return False

        self.mySerial.print("AT+CNTP="")
        if ntpserver != 0:
        self.mySerial.print(ntpserver)
        else:
        self.mySerial.print("pool.ntp.org")
        self.mySerial.println("",0")
        self.readline(FONA_DEFAULT_TIMEOUT_MS)
        if (strcmp(replybuffer, "OK") != 0)
        return False

        if (not self.sendCheckReply("AT+CNTP", ok_reply, 10000))
        return False

        status
        self.readline(10000)
        if (not parseReply("+CNTP:", &status))
        return False
    else:
        if (not self.sendCheckReply("AT+CNTPCID=0", ok_reply))
        return False

    return True

    def getTime(buff,  maxlen)
    self.getReply("AT+CCLK?", ()10000)
    if (strncmp(replybuffer, "+CCLK: ", 7) != 0)
        return False

    p = replybuffer + 7
    lentocopy = min(maxlen - 1, (int)len(p))
    strncpy(buff, p, lentocopy + 1)
    buff[lentocopy] = 0

    self.readline() # eat OK

    return True

    #******** Real Time Clock *******************************************

    def readRTC( *year,  *month,  *date,  *hr,  *min,  *sec, int8_t *tz)
    self.getReply("AT+CCLK?", ()10000) #Get RTC timeout 10 sec
    if (strncmp(replybuffer, "+CCLK: ", 7) != 0)
        return False

    p = replybuffer + 8 # skip +CCLK: "
    # Parse date
    reply = int(p)    # get year
    *year = ()reply # save as year
    p += 3                 # skip 3 char
    reply = int(p)
    *month = ()reply
    p += 3
    reply = int(p)
    *date = ()reply
    p += 3
    reply = int(p)
    *hr = ()reply
    p += 3
    reply = int(p)
    *min = ()reply
    p += 3
    reply = int(p)
    *sec = ()reply
    p += 3
    reply = int(p)
    *tz = reply

    self.readline() # eat OK

    return True

    def enableRTC( i)
    if (not self.sendCheckReply("AT+CLTS=", i, ok_reply))
        return False
    return self.sendCheckReply("AT&W", ok_reply)

    #******** GPS *********************************************************

    def enableGPS( onoff)
    state

    # First check if its already on or off

    if _type == SIM808_V2 or _type == SIM7000 or _type == SIM7070:
        if (not self.sendParseReply("AT+CGNSPWR?", "+CGNSPWR: ", &state))
        return False
    elif _type == SIM5320A or _type == SIM5320E or _type == SIM7500 or _type == SIM7600:
        if (notdef sendParseReply("AT+CGPS?", "+CGPS: ", &state))
        return False
    else:
        if (not self.sendParseReply("AT+CGPSPWR?", "+CGPSPWR: ", &state))
        return False

    if onoff and not state:
        if _type == SIM808_V2 or _type == SIM7000 or _type == SIM7070:
        if (not self.sendCheckReply("AT+CGNSPWR=1", ok_reply))
            return False
        elif _type == SIM5320A or _type == SIM5320E or _type == SIM7500 or _type == SIM7600:
        if (not self.sendCheckReply("AT+CGPS=1", ok_reply))
            return False
        else:
        if (not self.sendCheckReply("AT+CGPSPWR=1", ok_reply))
            return False
    elif not onoff and state:
        if _type == SIM808_V2 or _type == SIM7000 or _type == SIM7070:
        if (not self.sendCheckReply("AT+CGNSPWR=0", ok_reply))
            return False
        elif _type == SIM5320A or _type == SIM5320E or _type == SIM7500 or _type == SIM7600:
        if (not self.sendCheckReply("AT+CGPS=0", ok_reply))
            return False
        # this takes a little time
        self.readline(2000) # eat '+CGPS: 0'
        else:
        if (not self.sendCheckReply("AT+CGPSPWR=0", ok_reply))
            return False
    return True

    #
    #  Adafruit_FONA_3G::enableGPS( onoff) {
    #  state
    # 
    # / first check if its already on or off
    # if (!def sendParseReply("AT+CGPS?", "+CGPS: ", &state) )
        # return false
    # 
    # if (onoff && !state) {
        # if (! self.sendCheckReply("AT+CGPS=1", ok_reply))
        # return false
    # } else if (!onoff && state) {
        # if (! self.sendCheckReply("AT+CGPS=0", ok_reply))
        # return false
        # / this takes a little time
        # self.readline(2000); # eat '+CGPS: 0'
    # }
    # return true
    # }
    # 

    int8_tdef GPSstatus(void)
    if _type == SIM808_V2 or _type == SIM7000 or _type == SIM7070:
        # 808 V2 uses GNS commands and doesn't have an explicit 2D/3D fix status.
        # Instead just look for a fix and if found assume it's a 3D fix.
        self.getReply("AT+CGNSINF")
        p = prog_char_strstr(replybuffer, (prog_char *)"+CGNSINF: ")
        if p == 0:
        return -1
        p += 10
        self.readline() # eat 'OK'
        if p[0] == '0':
        return 0 # GPS is not even on!

        p += 2 # Skip to second value, fix status.
        #self.log.debug(p)
        # Assume if the fix status is '1' then we have a 3D fix, otherwise no fix.
        if p[0] == '1':
        return 3
        else:
        return 1
    if _type == SIM5320A or _type == SIM5320E:
        # FONA 3G doesn't have an explicit 2D/3D fix status.
        # Instead just look for a fix and if found assume it's a 3D fix.
        self.getReply("AT+CGPSINFO")
        p = prog_char_strstr(replybuffer, (prog_char *)"+CGPSINFO:")
        if p == 0:
        return -1
        if p[10] != ',':
        return 3 # if you get anything, its 3D fix
        return 0
    else:
        # 808 V1 looks for specific 2D or 3D fix state.
        self.getReply("AT+CGPSSTATUS?")
        p = prog_char_strstr(replybuffer, (prog_char *)"SSTATUS: Location ")
        if p == 0:
        return -1
        p += 18
        self.readline() # eat 'OK'
        #self.log.debug(p)
        if p[0] == 'U':
        return 0
        if p[0] == 'N':
        return 1
        if p[0] == '2':
        return 2
        if p[0] == '3':
        return 3
    # else
    return 0

    def getGPS( arg, buffer,  maxbuff)
    x = arg

    if _type == SIM5320A or _type == SIM5320E or _type == SIM7500 or _type == SIM7600:
        self.getReply("AT+CGPSINFO")
    elif _type == SIM808_V1:
        self.getReply("AT+CGPSINF=", x)
    else:
        self.getReply("AT+CGNSINF")

    p = prog_char_strstr(replybuffer, (prog_char *)"SINF")
    if p == 0:
        buffer[0] = 0
        return 0

    p += 6

    len = max(maxbuff - 1, (int)len(p))
    strncpy(buffer, p, len)
    buffer[len] = 0

    self.readline() # eat 'OK'
    return len

    def getGPS(lat, lon, speed_kph, heading, sats)
    # def getGPS(float *lat, float *lon, float *speed_kph, float *heading, float *altitude,
    #                               year,  *month,  *day,  *hour,  *min, float *sec) {


    # we need at least a 2D fix
    if _type < SIM7000: # SIM7500 doesn't support AT+CGPSSTATUS? command
        if GPSstatus() < 2:
        return False

    # grab the mode 2^5 gps csv from the sim808
    res_len = getGPS(32, gpsbuffer, 120)

    # make sure we have a response
    if res_len == 0:
        return False

    if _type == SIM5320A or _type == SIM5320E or _type == SIM7500 or _type == SIM7600:
        # # Parse 3G respose
        # # +CGPSINFO:4043.000000,N,07400.000000,W,151015,203802.1,-12.0,0.0,0
        # # skip beginning
        # char *tok

        # # grab the latitude
        # char *latp = strtok(gpsbuffer, ",")
        # if (!latp)
        #   return false

        # # grab latitude direction
        # char *latdir = strtok(NULL, ",")
        # if (!latdir)
        #   return false

        # # grab longitude
        # char *longp = strtok(NULL, ",")
        # if (!longp)
        #   return false

        # # grab longitude direction
        # char *longdir = strtok(NULL, ",")
        # if (!longdir)
        #   return false

        # # skip date & time
        # tok = strtok(NULL, ",")
        # tok = strtok(NULL, ",")

        # # only grab altitude if needed
        # if (altitude != NULL)
        # {
        #   # grab altitude
        #   char *altp = strtok(NULL, ",")
        #   if (!altp)
        #     return false
        #   *altitude = atof(altp)
        # }

        # # only grab speed if needed
        # if (speed_kph != NULL)
        # {
        #   # grab the speed in km/h
        #   char *speedp = strtok(NULL, ",")
        #   if (!speedp)
        #     return false

        #   *speed_kph = atof(speedp)
        # }

        # # only grab heading if needed
        # if (heading != NULL)
        # {

        #   # grab the speed in knots
        #   char *coursep = strtok(NULL, ",")
        #   if (!coursep)
        #     return false

        #   *heading = atof(coursep)
        # }

        # double latitude = atof(latp)
        # double longitude = atof(longp)

        # # convert latitude from minutes to decimal
        # float degrees = floor(latitude / 100)
        # double minutes = latitude - (100 * degrees)
        # minutes /= 60
        # degrees += minutes

        # # turn direction into + or -
        # if (latdir[0] == 'S')
        #   degrees *= -1

        # *lat = degrees

        # # convert longitude from minutes to decimal
        # degrees = floor(longitude / 100)
        # minutes = longitude - (100 * degrees)
        # minutes /= 60
        # degrees += minutes

        # # turn direction into + or -
        # if (longdir[0] == 'W')
        #   degrees *= -1

        # *lon = degrees

        # (void)tok
    elif _type == SIM808_V2 or _type >= SIM7000:
        # Parse 808 V2 response.  See table 2-3 from here for format:
        # http:#www.adafruit.com/datasheets/SIM800%20Series_GNSS_Application%20Note%20V1.00.pdf
        # 
    #    +CGNSINF: <GNSS run status>,<Fix status >,<UTC date &
    #    Time>,<Latitude>,<Longitude>,<MSL Altitude>,<Speed Over Ground>,
    #    <Course Over Ground>,<Fix Mode>,<Reserved1>,<HDOP>,<PDOP>,<VDOP>,<Reserved2>,
    #    <GNSS Satellites in View>,<GNSS Satellites Used>,<GLONASS Satellites Used>,
    #    <Reserved3>,<C/N0 max>,<HPA>,<VPA>
    ##

        # skip GPS run status
        tok = strtok(gpsbuffer, ",")
        if not tok:
        return False

        # skip fix status
        tok = strtok(NULL, ",")
        if not tok:
        return False

        # skip date
        # tok = strtok(NULL, ",")
        # if (! tok) return false

        # # only grab date and time if needed
        # if ((year != NULL) && (month != NULL) && (day != NULL) && (hour != NULL) && (min != NULL) && (sec != NULL)) {
        #   char *date = strtok(NULL, ",")
        #   if (! date) return false

        #   # Seconds
        #   char *ptr = date + 12
        #   *sec = atof(ptr)

        #   # Minutes
        #   ptr[0] = 0
        #   ptr = date + 10
        #   *min = atoi(ptr)

        #   # Hours
        #   ptr[0] = 0
        #   ptr = date + 8
        #   *hour = atoi(ptr)

        #   # Day
        #   ptr[0] = 0
        #   ptr = date + 6
        #   *day = atoi(ptr)

        #   # Month
        #   ptr[0] = 0
        #   ptr = date + 4
        #   *month = atoi(ptr)

        #   # Year
        #   ptr[0] = 0
        #   ptr = date
        #   *year = atoi(ptr)
        # }
        # else
        # {

        # skip date
        tok = strtok(NULL, ",")
        if not tok:
        return False
        # }

        # grab the latitude
        latp = strtok(NULL, ",")
        if not latp:
        return False

        # grab longitude
        longp = strtok(NULL, ",")
        if not longp:
        return False

        *lat = float(latp)
        *lon = float(longp)

        # # only grab altitude if needed
        # if (altitude != NULL)
        # {

        # skip altitude
        tok = strtok(NULL, ",")
        if not tok:
        return False

        #   *altitude = atof(altp)
        # }

        # only grab speed if needed
        if speed_kph != NULL:
        # grab the speed in km/h
        speedp = strtok(NULL, ",")
        if not speedp:
            return False

        *speed_kph = float(speedp)

        # only grab heading if needed
        if heading != NULL:

        # grab the speed in knots
        coursep = strtok(NULL, ",")
        if not coursep:
            return False

        *heading = int(coursep)
        # # only grab heading if needed
        # if (heading != NULL)
        # {

        #   # grab the speed in knots
        #   char *coursep = strtok(NULL, ",")
        #   if (!coursep)
        #     return false

        #   *heading = atof(coursep)
        # }

        # tok = strtok(NULL, ",")
        # tok = strtok(NULL, ",")
        # tok = strtok(NULL, ",")
        tok = strtok(NULL, ",")
        tok = strtok(NULL, ",")
        tok = strtok(NULL, ",")
        tok = strtok(NULL, ",")

        # only grab sats if needed

        Serial.println("Here")
        if sats != NULL:
        # grab the speed in knots
        nsats = strtok(NULL, ",")
        Serial.println("There")
        if not nsats:
            return False
        Serial.println(*nsats)
        *sats = (char)int(nsats)

    tok
    else:
        # # Parse 808 V1 response.

        # # skip mode
        # char *tok = strtok(gpsbuffer, ",")
        # if (!tok)
        #   return false

        # # skip date
        # tok = strtok(NULL, ",")
        # if (!tok)
        #   return false

        # # skip fix
        # tok = strtok(NULL, ",")
        # if (!tok)
        #   return false

        # # grab the latitude
        # char *latp = strtok(NULL, ",")
        # if (!latp)
        #   return false

        # # grab latitude direction
        # char *latdir = strtok(NULL, ",")
        # if (!latdir)
        #   return false

        # # grab longitude
        # char *longp = strtok(NULL, ",")
        # if (!longp)
        #   return false

        # # grab longitude direction
        # char *longdir = strtok(NULL, ",")
        # if (!longdir)
        #   return false

        # double latitude = atof(latp)
        # double longitude = atof(longp)

        # # convert latitude from minutes to decimal
        # float degrees = floor(latitude / 100)
        # double minutes = latitude - (100 * degrees)
        # minutes /= 60
        # degrees += minutes

        # # turn direction into + or -
        # if (latdir[0] == 'S')
        #   degrees *= -1

        # *lat = degrees

        # # convert longitude from minutes to decimal
        # degrees = floor(longitude / 100)
        # minutes = longitude - (100 * degrees)
        # minutes /= 60
        # degrees += minutes

        # # turn direction into + or -
        # if (longdir[0] == 'W')
        #   degrees *= -1

        # *lon = degrees

        # # only grab speed if needed
        # if (speed_kph != NULL)
        # {

        #   # grab the speed in knots
        #   char *speedp = strtok(NULL, ",")
        #   if (!speedp)
        #     return false

        #   # convert to kph
        #   *speed_kph = atof(speedp) * 1.852
        # }

        # # only grab heading if needed
        # if (heading != NULL)
        # {

        #   # grab the speed in knots
        #   char *coursep = strtok(NULL, ",")
        #   if (!coursep)
        #     return false

        #   *heading = atof(coursep)
        # }

        # # no need to continue
        # if (altitude == NULL)
        #   return true

        # # we need at least a 3D fix for altitude
        # if (GPSstatus() < 3)
        #   return false

        # # grab the mode 0 gps csv from the sim808
        # res_len = getGPS(0, gpsbuffer, 120)

        # # make sure we have a response
        # if (res_len == 0)
        #   return false

        # # skip mode
        # tok = strtok(gpsbuffer, ",")
        # if (!tok)
        #   return false

        # # skip lat
        # tok = strtok(NULL, ",")
        # if (!tok)
        #   return false

        # # skip long
        # tok = strtok(NULL, ",")
        # if (!tok)
        #   return false

        # # grab altitude
        # char *altp = strtok(NULL, ",")
        # if (!altp)
        #   return false

        # *altitude = atof(altp)

        # (void)tok

    return True

    def enableGPSNMEA( i)

    sendbuff[15] = "AT+CGPSOUT=000"
    sendbuff[11] = (i / 100) + '0'
    i %= 100
    sendbuff[12] = (i / 10) + '0'
    i %= 10
    sendbuff[13] = i + '0'

    if _type == SIM808_V2 or _type == SIM7000 or _type == SIM7070:
        if i:
        self.sendCheckReply("AT+CGNSCFG=1", ok_reply)
        self.sendCheckReply("AT+CGNSTST=1", ok_reply)
        return True
        else:
        return self.sendCheckReply("AT+CGNSTST=0", ok_reply)
    else:
        return self.sendCheckReply(sendbuff, ok_reply, 2000)

    #******** GPRS *********************************************************

    def enableGPRS( onoff)
    if _type == SIM5320A or _type == SIM5320E or _type == SIM7500 or _type == SIM7600:
        if onoff:
        # disconnect all sockets
        #self.sendCheckReply("AT+CIPSHUT", "SHUT OK", 5000)

        if (not self.sendCheckReply("AT+CGATT=1", ok_reply, 10000))
            return False

        # set bearer profile access point name
        if apn:
            # Send command AT+CGSOCKCONT=1,"IP","<apn value>" where <apn value> is the configured APN name.
            if (not self.sendCheckReplyQuoted("AT+CGSOCKCONT=1,"IP",", apn, ok_reply, 10000))
            return False

            # set username/password
            if apnusername:
            authstring[100] = "AT+CGAUTH=1,1,""
            # char authstring[100] = "AT+CSOCKAUTH=1,1,""; # For 3G
            strp = authstring + len(authstring)
            prog_char_strcpy(strp, (prog_char *)apnusername)
            strp += prog_char_strlen((prog_char *)apnusername)
            strp[0] = '"'
            strp += 1
            strp[0] = 0

            if apnpassword:
                strp[0] = ','
                strp += 1
                strp[0] = '"'
                strp += 1
                prog_char_strcpy(strp, (prog_char *)apnpassword)
                strp += prog_char_strlen((prog_char *)apnpassword)
                strp[0] = '"'
                strp += 1
                strp[0] = 0

            if not self.sendCheckReply(authstring, ok_reply, 10000):
                return False

        # connect in transparent mode
        if (not self.sendCheckReply("AT+CIPMODE=1", ok_reply, 10000))
            return False
        # open network
        if _type == SIM5320A or _type == SIM5320E:
            if (not self.sendCheckReply("AT+NETOPEN=,,1", "Network opened", 10000))
            return False
        elif _type == SIM7500 or _type == SIM7600:
            if (not self.sendCheckReply("AT+NETOPEN", ok_reply, 10000))
            return False
        self.readline() # eat 'OK'
        else:
        # close GPRS context
        if _type == SIM5320A or _type == SIM5320E:
            if (not self.sendCheckReply("AT+NETCLOSE", "Network closed", 10000))
            return False
        elif _type == SIM7500 or _type == SIM7600:
            self.getReply("AT+NETCLOSE")
            self.getReply("AT+CHTTPSSTOP")
            # self.getReply("AT+CHTTPSCLSE")

            # if (! self.sendCheckReply("AT+NETCLOSE", ok_reply, 10000))
            #   return false
            #    if (! self.sendCheckReply("AT+CHTTPSSTOP", "+CHTTPSSTOP: 0", 10000))
            #  return false
            # if (! self.sendCheckReply("AT+CHTTPSCLSE", ok_reply, 10000))
            #  return false

        self.readline() # eat 'OK'
    elif _type == SIM7070:
        # getNetworkInfo()

        if not openWirelessConnection(onoff):
        return False
        # if (! wirelessConnStatus()) return false
    else:
        if onoff:
        # if (_type < SIM7000) { # UNCOMMENT FOR LTE ONLY!
        # disconnect all sockets
        self.sendCheckReply("AT+CIPSHUT", "SHUT OK", 20000)

        if (not self.sendCheckReply("AT+CGATT=1", ok_reply, 10000))
            return False

        # set bearer profile! connection type GPRS
        if (not self.sendCheckReply("AT+SAPBR=3,1,"CONTYPE","GPRS"", ok_reply, 10000))
            return False
        # } # UNCOMMENT FOR LTE ONLY!

        time.sleep_ms(200) # This seems to help the next line run the first time

        # set bearer profile access point name
        if apn:
            # Send command AT+SAPBR=3,1,"APN","<apn value>" where <apn value> is the configured APN value.
            if (not self.sendCheckReplyQuoted("AT+SAPBR=3,1,"APN",", apn, ok_reply, 10000))
            return False

            # if (_type < SIM7000) { # UNCOMMENT FOR LTE ONLY!
            # send AT+CSTT,"apn","user","pass"
            self.flushInput()

            self.mySerial.print("AT+CSTT="")
            self.mySerial.print(apn)
            if apnusername:
            self.mySerial.print("","")
            self.mySerial.print(apnusername)
            if apnpassword:
            self.mySerial.print("","")
            self.mySerial.print(apnpassword)
            self.mySerial.println(""")

            self.log.debug("\t---> ")
            self.log.debug("AT+CSTT="")
            self.log.debug(apn)

            if apnusername:
            self.log.debug("","")
            self.log.debug(apnusername)
            if apnpassword:
            self.log.debug("","")
            self.log.debug(apnpassword)
            self.log.debug(""")

            if not self.expectReply(ok_reply):
            return False
            # } # UNCOMMENT FOR LTE ONLY!

            # set username/password
            if apnusername:
            # Send command AT+SAPBR=3,1,"USER","<user>" where <user> is the configured APN username.
            if (not self.sendCheckReplyQuoted("AT+SAPBR=3,1,"USER",", apnusername, ok_reply, 10000))
                return False
            if apnpassword:
            # Send command AT+SAPBR=3,1,"PWD","<password>" where <password> is the configured APN password.
            if (not self.sendCheckReplyQuoted("AT+SAPBR=3,1,"PWD",", apnpassword, ok_reply, 10000))
                return False

        # open bearer
        if (not self.sendCheckReply("AT+SAPBR=1,1", ok_reply, 30000))
            return False

        # if (_type < SIM7000) { # UNCOMMENT FOR LTE ONLY!
        # bring up wireless connection
        if (not self.sendCheckReply("AT+CIICR", ok_reply, 10000))
            return False
        # } # UNCOMMENT FOR LTE ONLY!

        # if (! openWirelessConnection(true)) return false
        # if (! wirelessConnStatus()) return false
        else:
        # disconnect all sockets
        if (not self.sendCheckReply("AT+CIPSHUT", "SHUT OK", 20000))
            return False

        # close bearer
        if (not self.sendCheckReply("AT+SAPBR=0,1", ok_reply, 10000))
            return False

        # if (_type < SIM7000) { # UNCOMMENT FOR LTE ONLY!
        if (not self.sendCheckReply("AT+CGATT=0", ok_reply, 10000))
            return False
        # } # UNCOMMENT FOR LTE ONLY!
    return True

    #
    #  Adafruit_FONA_3G::enableGPRS( onoff) {
    # 
    # if (onoff) {
        # / disconnect all sockets
        # /self.sendCheckReply("AT+CIPSHUT", "SHUT OK", 5000)
    # 
        # if (! self.sendCheckReply("AT+CGATT=1", ok_reply, 10000))
        # return false
    # 
    # 
        # / set bearer profile access point name
        # if (apn) {
        # / Send command AT+CGSOCKCONT=1,"IP","<apn value>" where <apn value> is the configured APN name.
        # if (! self.sendCheckReplyQuoted("AT+CGSOCKCONT=1,"IP",", apn, ok_reply, 10000))
            # return false
    # 
        # / set username/password
        # if (apnusername) {
            # char authstring[100] = "AT+CGAUTH=1,1,""
            # char *strp = authstring + strlen(authstring)
            # prog_char_strcpy(strp, (prog_char *)apnusername)
            # strp+=prog_char_strlen((prog_char *)apnusername)
            # strp[0] = '"'
            # strp++
            # strp[0] = 0
    # 
            # if (apnpassword) {
            # strp[0] = ','; strp++
            # strp[0] = '"'; strp++
            # prog_char_strcpy(strp, (prog_char *)apnpassword)
            # strp+=prog_char_strlen((prog_char *)apnpassword)
            # strp[0] = '"'
            # strp++
            # strp[0] = 0
            # }
    # 
            # if (! self.sendCheckReply(authstring, ok_reply, 10000))
            # return false
        # }
        # }
    # 
        # / connect in transparent
        # if (! self.sendCheckReply("AT+CIPMODE=1", ok_reply, 10000))
        # return false
        # / open network (?)
        # if (! self.sendCheckReply("AT+NETOPEN=,,1", "Network opened", 10000))
        # return false
    # 
        # self.readline(); # eat 'OK'
    # } else {
        # / close GPRS context
        # if (! self.sendCheckReply("AT+NETCLOSE", "Network closed", 10000))
        # return false
    # 
        # self.readline(); # eat 'OK'
    # }
    # 
    # return true
    # }
    # 

    def getNetworkInfo():
    self.getReply("AT+CPSI?")
    self.getReply("AT+COPS?")
    self.readline() # Eat 'OK'

    def getNetworkInfoLong():
    if (not self.sendCheckReply("AT+COPS=?", ok_reply, 2000))
        return False

    return True

    int8_tdef GPRSstate(void)
    state

    if (not self.sendParseReply("AT+CGATT?", "+CGATT: ", &state))
        return -1

    return state

    Adafruit_FONA::setNetworkSettings( apn,
                                            username,  password)
    this.apn = apn
    this.apnusername = username
    this.apnpassword = password

    if _type >= SIM7000:
        self.sendCheckReplyQuoted("AT+CGDCONT=1,"IP",", apn, ok_reply, 10000)

    def getGSMLoc(errorcode, buff,  maxlen)

    self.getReply("AT+CIPGSMLOC=1,1", ()10000)

    if (not parseReply("+CIPGSMLOC: ", errorcode))
        return False

    p = replybuffer + 14
    lentocopy = min(maxlen - 1, (int)len(p))
    strncpy(buff, p, lentocopy + 1)

    self.readline() # eat OK

    return True

    def getGSMLoc(lat, lon)

    returncode

    # make sure we could get a response
    if not getGSMLoc(&returncode, gpsbuffer, 120):
        return False

    # make sure we have a valid return code
    if returncode != 0:
        return False

    # +CIPGSMLOC: 0,-74.007729,40.730160,2015/10/15,19:24:55
    # tokenize the gps buffer to locate the lat & long
    longp = strtok(gpsbuffer, ",")
    if not longp:
        return False

    latp = strtok(NULL, ",")
    if not latp:
        return False

    *lat = float(latp)
    *lon = float(longp)

    return True

    # Open or close wireless data connection
    def openWirelessConnection(onoff)
    if not onoff: # Disconnect wireless
        if _type == SIM7070:
        if (not self.sendCheckReply("AT+CNACT=0,0", ok_reply))
            return False
        else:
        if (not self.sendCheckReply("AT+CNACT=0", ok_reply))
            return False

        self.readline()
        self.log.debug("\t<--- ")
        self.log.debug(replybuffer)

        if (_type == SIM7070 and strstr(replybuffer, ",DEACTIVE") == NULL)
        return False # +APP PDP: <pdpidx>,DEACTIVE
        elif (_type == SIM7000 and strstr(replybuffer, "PDP: DEACTIVE") == NULL)
        return False # +APP PDP: DEACTIVE
    else:
        if _type == SIM7070:
        if (not self.sendCheckReply("AT+CNACT=0,1", ok_reply))
            return False
        else:
        # if (! self.sendCheckReplyQuoted("AT+CNACT=1,", apn, ok_reply)) return false
        if (not self.sendCheckReply("AT+CNACT=1", ok_reply))
            return False

        self.readline()
        self.log.debug("\t<--- ")
        self.log.debug(replybuffer)

        if (_type == SIM7070 and strstr(replybuffer, ",ACTIVE") == NULL)
        return False # +APP PDP: <pdpidx>,ACTIVE
        elif (_type == SIM7000 and strstr(replybuffer, "PDP: ACTIVE") == NULL)
        return False # +APP PDP: ACTIVE

    return True

    # Query wireless connection status
    def wirelessConnStatus(void)
    self.getReply("AT+CNACT?")
    # Format of response:
    # +CNACT: <status>,<ip_addr>  (ex.SIM7000)
    # +CNACT: <pdpidx>,<status>,<ip_addr>  (ex.SIM7070)
    if _type == SIM7070:
        if (strstr(replybuffer, "+CNACT: 0,1") == NULL)
        return False
    else:
        if (strstr(replybuffer, "+CNACT: 1") == NULL)
        return False
    return True

    def postData(self, request_type, URL, body, token, u bodylen)
    # NOTE: Need to open socket/enable GPRS before using this function
    # char auxStr[64]

    # Make sure HTTP service is terminated so initialization will run
    self.sendCheckReply("AT+HTTPTERM", ok_reply, 10000)

    # Initialize HTTP service
    if (not self.sendCheckReply("AT+HTTPINIT", ok_reply, 10000))
        return False

    # Set HTTP parameters
    if (not self.sendCheckReply("AT+HTTPPARA="CID",1", ok_reply, 10000))
        return False

    # Specify URL

    sprintf(urlBuff, "AT+HTTPPARA="URL","%s"" % (URL))

    if not self.sendCheckReply(urlBuff, ok_reply, 10000):
        return False

    # Perform request based on specified request Type
    if len(body) > 0:
        bodylen = len(body)

    if String(request_type: == "GET")
    {
        if (not self.sendCheckReply("AT+HTTPACTION=0", ok_reply, 10000))
        return False
    elif String(request_type: == "POST" and bodylen > 0)
    { # POST with content body
        if (not self.sendCheckReply("AT+HTTPPARA="CONTENT","application/json"", ok_reply, 10000))
        return False

        if len(token) > 0:

        sprintf(tokenStr, "AT+HTTPPARA="USERDATA","Authorization: Bearer %s"" % (token))

        if not self.sendCheckReply(tokenStr, ok_reply, 10000):
            return False


        sprintf(dataBuff, "AT+HTTPDATA=%lu,10000" % ((unsigned int)bodylen))
        if (not self.sendCheckReply(dataBuff, "DOWNLOAD", 10000))
        return False

        time.sleep_ms(100) # Needed for fast baud rates (ex: 115200 baud with SAMD21 hardware serial)

        if not self.sendCheckReply(body, ok_reply, 10000):
        return False

        if (not self.sendCheckReply("AT+HTTPACTION=1", ok_reply, 10000))
        return False
    elif String(request_type: == "POST" and bodylen == 0)
    { # POST with query parameters
        if (not self.sendCheckReply("AT+HTTPACTION=1", ok_reply, 10000))
        return False
    elif String(request_type: == "HEAD")
    {
        if (not self.sendCheckReply("AT+HTTPACTION=2", ok_reply, 10000))
        return False

    # Parse response status and size
    status, datalen
    self.readline(10000)
    if (not parseReply("+HTTPACTION:", &status, ',', 1))
        return False
    if (not parseReply("+HTTPACTION:", &datalen, ',', 2))
        return False

    self.log.debug("HTTP status: ")
    self.log.debug(status)
    self.log.debug("Data length: ")
    self.log.debug(datalen)

    if status != 200:
        return False

    self.getReply("AT+HTTPREAD")

    self.readline(10000)
    self.log.debug("\t<--- %s" % replybuffer) # Print out server reply

    # Terminate HTTP service
    self.sendCheckReply("AT+HTTPTERM", ok_reply, 10000)

    return True

    #******************************** HTTPS FUNCTION ********************************
    def postData(self, server,  port, connType, URL, body)
    # Sample request URL for SIM5320/7500/7600:
    # "GET /dweet/for/{deviceID}?temp={temp}&batt={batt} HTTP/1.1\r\nHost: dweet.io\r\n\r\n"

    # Start HTTPS stack
    if _type == SIM7500 or _type == SIM7600:
        self.getReply("AT+CHTTPSSTART") # Don't check if true/false since it will return false if already started (not stopped before)

        # if (! self.sendCheckReply("AT+CHTTPSSTART", "+CHTTPSSTART: 0", 10000))
        #  return false
    else:
        if (not self.sendCheckReply("AT+CHTTPSSTART", ok_reply, 10000))
        return False

    self.log.debug("Waiting 1s to ensure connection...")
    time.sleep_ms(1000)

    # Construct the AT command based on function parameters
    connTypeNum = 1

    if (connType.find("HTTP") == -1)
    {
        connTypeNum = 1
    if (connType.find("HTTPS") == -1)
    {
        connTypeNum = 2

    sprintf(auxStr, "AT+CHTTPSOPSE="%s",%d,%d" % (server, port, connTypeNum))

    if _type == SIM7500 or _type == SIM7600:
        # self.sendParseReply(auxStr, "+CHTTPSOPSE: ", &reply)
        # if (reply != 0) return false

        if not self.sendCheckReply(auxStr, ok_reply, 10000):
        return False

        self.readline(10000)
        self.log.debug("\t<--- ")
        self.log.debug(replybuffer)
        # if (strcmp(replybuffer, "+CHTTPSOPSE: 0") != 0) return false
        # SIM7500E v1.1 firmware apparently doesn't have the space:
        if ((strstr(replybuffer, "+CHTTPSOPSE: 0") == NULL) or (strstr(replybuffer, "+CHTTPSOPSE:0") == NULL))
        return False
    else:
        if not self.sendCheckReply(auxStr, ok_reply, 10000):
        return False

    self.log.debug("Waiting 1s to make sure it works...")
    time.sleep_ms(1000)

    # Send data to server
    sprintf(auxStr, "AT+CHTTPSSEND=%i" % (len(URL) + len(body))) # URL and body must include \r\n as needed

    if (not self.sendCheckReply(auxStr, ">", 10000))
        return False

    if _type == SIM7500 or _type == SIM7600:
        # self.sendParseReply(URL, "+CHTTPSSEND: ", &reply)
        # if (reply != 0) return false

        # Less efficient method
        # char dataBuff[strlen(URL)+strlen(body)+1]
        # if (strlen(body) > 0) {
        #   strcpy(dataBuff, URL)
        #   strcat(dataBuff, body)
        # }

        if len(body) == 0:
        if not self.sendCheckReply(URL, ok_reply, 10000):
            return False
        else:
        self.mySerial.print(URL)
        self.log.debug("\t---> ")
        self.log.debug(URL)

        if not self.sendCheckReply(body, ok_reply, 10000):
            return False

        # if (! self.sendCheckReply(dataBuff, ok_reply, 10000))
        #   return false

        self.readline(10000)
        self.log.debug("\t<--- ")
        self.log.debug(replybuffer)
        # if (strcmp(replybuffer, "+CHTTPSSEND: 0") != 0) return false
        if (strstr(replybuffer, "+CHTTPSSEND: 0") == NULL)
        return False
    else:
        if not self.sendCheckReply(URL, ok_reply, 10000):
        return False

    time.sleep_ms(1000)

    if _type == SIM5320A or _type == SIM5320E:
        if (not self.sendCheckReply("AT+CHTTPSSEND", ok_reply, 10000))
        return False

        self.readline(10000)
        self.log.debug("\t<--- ")
        self.log.debug(replybuffer)
        # if (strcmp(replybuffer, "+CHTTPSSEND: 0") != 0) return false
        if (strstr(replybuffer, "+CHTTPSSEND: 0") == NULL)
        return False

        time.sleep_ms(1000) # Needs to be here otherwise won't get server reply properly

    # Check server response length
    replyLen
    self.sendParseReply("AT+CHTTPSRECV?", "+CHTTPSRECV: LEN,", &replyLen)

    # Get server response content
    sprintf(auxStr, "AT+CHTTPSRECV=%i" % (replyLen))
    self.getReply(auxStr, 2000)

    if replyLen > 0:
        readRaw(replyLen)
        self.flushInput()
        self.log.debug("\t<--- ")
        self.log.debug(replybuffer)

    # Close HTTP/HTTPS session
    self.sendCheckReply("AT+CHTTPSCLSE", ok_reply, 10000)
    # if (! self.sendCheckReply("AT+CHTTPSCLSE", ok_reply, 10000))
    #   return false

    self.readline(10000)
    self.log.debug("\t<--- %s" % replybuffer)

    # Stop HTTP/HTTPS stack
    if (not self.sendCheckReply("AT+CHTTPSSTOP", "+CHTTPSSTOP: 0", 10000))
        return False

    self.readline() # Eat OK
    self.log.debug("\t<--- %s" % replybuffer)

    return (replyLen > 0)

    def HTTP_connect(self, server)
    # Set up server URL

    self.sendCheckReply("AT+SHDISC", ok_reply, 10000) # Disconnect HTTP

    sprintf(urlBuff, "AT+SHCONF="URL","%s"" % (server))

    if not self.sendCheckReply(urlBuff, ok_reply, 10000):
        return False

    # Set max HTTP body length
    self.sendCheckReply("AT+SHCONF="BODYLEN",1024", ok_reply, 10000) # Max 1024 for SIM7070G

    # Set max HTTP header length
    self.sendCheckReply("AT+SHCONF="HEADERLEN",350", ok_reply, 10000) # Max 350 for SIM7070G

    # HTTP build
    self.sendCheckReply("AT+SHCONN", ok_reply, 10000)

    # Get HTTP status
    self.getReply("AT+SHSTATE?")
    self.readline()
    if (strstr(replybuffer, "+SHSTATE: 1") == NULL)
    {
        self.log.debug(replybuffer)
        #return false
    self.readline() # Eat 'OK'

    # Clear HTTP header (HTTP header is appended)
    if (not self.sendCheckReply("AT+SHCHEAD", ok_reply, 10000))
        return False

    return True

    def HTTP_GET(self, URI)
    # Use fona.HTTP_addHeader() as needed before using this function
    # Then use fona.HTTP_connect() to connect to the server first

    sprintf(cmdBuff, "AT+SHREQ="%s",1" % (URI))

    self.sendCheckReply(cmdBuff, ok_reply, 10000)

    # Parse response status and size
    # Example reply --> "+SHREQ: "GET",200,387"
    status, datalen
    self.readline(10000)
    self.log.debug("\t<--- %s" % replybuffer)

    if (not parseReply("+SHREQ: "GET"", &status, ',', 1))
        return False
    if (not parseReply("+SHREQ: "GET"", &datalen, ',', 2))
        return False

    self.log.debug("HTTP status: ")
    self.log.debug(status)
    self.log.debug("Data length: ")
    self.log.debug(datalen)

    if status != 200:
        return False

    # Read server response
    self.getReply("AT+SHREAD=0,", datalen, 10000)
    self.readline(10000, True) # read multiline
    self.log.debug("\t<--- %s" % replybuffer) # +SHREAD: <datalen>
    self.readline(10000)
    self.log.debug("\t<--- %s" % replybuffer) # Print out server reply

    self.sendCheckReply("AT+SHDISC", ok_reply, 10000) # Disconnect HTTP

    return True

    def HTTP_POST(self, URI, body,  bodylen)
    # Use fona.HTTP_addHeader() as needed before using this function
    # Then use fona.HTTP_connect() to connect to the server first
    # Make sure this is large enough for URI

    # Example 2 in HTTP(S) app note for SIM7070 POST request
    if _type == SIM7070:
        sprintf(cmdBuff, "AT+SHBOD=%i,10000" % (bodylen))
        self.getReply(cmdBuff, 10000)
        if (strstr(replybuffer, ">") == NULL)
        return False # Wait for ">" to send message
        self.sendCheckReply(body, ok_reply, 2000)

        # if (! strcmp(replybuffer, "OK") != 0) return false; # Now send the JSON body
    else: # For ex, SIM7000
        sprintf(cmdBuff, "AT+SHBOD="%s",%i" % (body, bodylen))
        if not self.sendCheckReply(cmdBuff, ok_reply, 10000):
        return False

    memset(cmdBuff, 0, sizeof(cmdBuff)) # Clear URI char array
    sprintf(cmdBuff, "AT+SHREQ="%s",3" % (URI))

    if not self.sendCheckReply(cmdBuff, ok_reply, 10000):
        return False

    # Parse response status and size
    # Example reply --> "+SHREQ: "POST",200,452"
    status, datalen
    self.readline(10000)
    self.log.debug("\t<--- %s" % replybuffer)

    if (not parseReply("+SHREQ: "POST"", &status, ',', 1))
        return False
    if (not parseReply("+SHREQ: "POST"", &datalen, ',', 2))
        return False

    self.log.debug("HTTP status: ")
    self.log.debug(status)
    self.log.debug("Data length: ")
    self.log.debug(datalen)

    if status != 200:
        return False

    # Read server response
    self.getReply("AT+SHREAD=0,", datalen, 10000)
    self.readline()
    self.log.debug("\t<--- %s" % replybuffer) # +SHREAD: <datalen>
    self.readline(10000)
    self.log.debug("\t<--- %s" % replybuffer) # Print out server reply

    self.sendCheckReply("AT+SHDISC", ok_reply, 10000) # Disconnect HTTP

    return True

    #******** FTP FUNCTIONS  ***********************************
    def FTP_Connect(self, serverIP,  port, username, password)

    # if (! self.sendCheckReply("AT+FTPCID=1", ok_reply, 10000))
    #   return false

    self.sendCheckReply("AT+FTPCID=1", ok_reply, 10000) # Don't return false in case this is a reconnect attempt

    sprintf(auxStr, "AT+FTPSERV="%s"" % (serverIP))

    if not self.sendCheckReply(auxStr, ok_reply, 10000):
        return False

    if port != 21:
        sprintf(auxStr, "AT+FTPPORT=%i" % (port))

        if not self.sendCheckReply(auxStr, ok_reply, 10000):
        return False

    if len(username) > 0:
        sprintf(auxStr, "AT+FTPUN="%s"" % (username))

        if not self.sendCheckReply(auxStr, ok_reply, 10000):
        return False

    if len(password) > 0:
        sprintf(auxStr, "AT+FTPPW="%s"" % (password))

        if not self.sendCheckReply(auxStr, ok_reply, 10000):
        return False

    return True

    def FTP_Quit()
    if (not self.sendCheckReply("AT+FTPQUIT", ok_reply, 10000))
        return False

    return True

    def FTP_Rename(self, filePath, oldName, newName)

    sprintf(auxStr, "AT+FTPGETPATH="%s"" % (filePath))

    if not self.sendCheckReply(auxStr, ok_reply, 10000):
        return False

    sprintf(auxStr, "AT+FTPGETNAME="%s"" % (oldName))

    if not self.sendCheckReply(auxStr, ok_reply, 10000):
        return False

    sprintf(auxStr, "AT+FTPPUTNAME="%s"" % (newName))

    if not self.sendCheckReply(auxStr, ok_reply, 10000):
        return False

    if (not self.sendCheckReply("AT+FTPRENAME", ok_reply, 2000))
        return False

    if (not self.expectReply("+FTPRENAME: 1,0"))
        return False

    return True

    def FTP_Delete(self, fileName, filePath)

    sprintf(auxStr, "AT+FTPGETNAME="%s"" % (fileName))

    if not self.sendCheckReply(auxStr, ok_reply, 10000):
        return False

    sprintf(auxStr, "AT+FTPGETPATH="%s"" % (filePath))

    if not self.sendCheckReply(auxStr, ok_reply, 10000):
        return False

    if (not self.sendCheckReply("AT+FTPDELE", ok_reply, 2000)) # It's NOT AT+FTPDELE=1
        return False

    if (not self.expectReply("+FTPDELE: 1,0"))
        return False

    return True

    # def FTP_MDTM(self, char* fileName, char* filePath, char & timestamp) {
    def FTP_MDTM(self, fileName, filePath, year,
                                    *month,  *day,  *hour,  *minute,  *second)

    sprintf(auxStr, "AT+FTPGETNAME="%s"" % (fileName))

    if not self.sendCheckReply(auxStr, ok_reply, 10000):
        return False

    sprintf(auxStr, "AT+FTPGETPATH="%s"" % (filePath))

    if not self.sendCheckReply(auxStr, ok_reply, 10000):
        return False

    if (not self.sendCheckReply("AT+FTPMDTM", ok_reply, 2000))
        return False

    self.readline(10000)
    self.log.debug("\t<--- %s" % replybuffer)

    if (strstr(replybuffer, "+FTPMDTM: 1,0,") == NULL)
        return False

    strcpy(timestamp, replybuffer + 14)
    # self.log.debug(timestamp); # DEBUG

    # Timestamp format for SIM7000 is YYYYMMDDHHMMSS
    memset(auxStr, 0, sizeof(auxStr)) # Clear auxStr contents
    strncpy(auxStr, timestamp, 4)
    *year = int(auxStr)

    memset(auxStr, 0, sizeof(auxStr))
    strncpy(auxStr, timestamp + 4, 2)
    *month = int(auxStr)

    strncpy(auxStr, timestamp + 6, 2)
    *day = int(auxStr)

    strncpy(auxStr, timestamp + 8, 2)
    *hour = int(auxStr)

    strncpy(auxStr, timestamp + 10, 2)
    *minute = int(auxStr)

    strncpy(auxStr, timestamp + 12, 2)
    *second = int(auxStr)

    return True

    constdef FTP_GET(self, fileName, filePath,  numBytes)
    err = "error"

    if not self.sendCheckReply('AT+FTPGETNAME="%s"' % (fileName), ok_reply, 10000):
        return err

    if not self.sendCheckReply('AT+FTPGETPATH="%s"' % (filePath), ok_reply, 10000):
        return err

    if (not self.sendCheckReply("AT+FTPGET=1", ok_reply, 10000))
        return err

    if (not self.expectReply("+FTPGET: 1,1"))
        return err

    if numBytes <= 1024:
        sprintf(auxStr, "AT+FTPGET=2,%i" % (numBytes))
        self.getReply(auxStr, 10000)
        if (strstr(replybuffer, "+FTPGET: 2,") == NULL)
        return err
    else:
        sprintf(auxStr, "AT+FTPEXTGET=2,%i,10000" % (numBytes))
        self.getReply(auxStr, 10000)
        if (replybuffer.find("+FTPEXTGET: 2,") == -1)
        return err

    self.readline(10000)
    self.log.debug("\t<--- %s" % replybuffer)

    return replybuffer

    def FTP_PUT(self, fileName, filePath, content, size_t numBytes):

    if not self.sendCheckReply('AT+FTPPUTNAME="%s"' % (fileName), ok_reply, 10000):
        return False

    if not self.sendCheckReply('AT+FTPPUTPATH="%s"' % (filePath), ok_reply, 10000):
        return False

    # Use extended PUT method if there's more than 1024 bytes to send
    if numBytes >= 1024:
        # Repeatedly PUT data until all data is sent
        u remBytes = numBytes
        offset = 0 # Data offset
        sendArray = content

        while remBytes > 0:
        if (not self.sendCheckReply("AT+FTPEXTPUT=1", ok_reply, 10000))
            return False

        if remBytes >= 300000:
            sprintf(auxStr, "AT+FTPEXTPUT=2,%i,300000,10000" % (offset)) # Extended PUT handles up to 300k
            offset = offset + 300000
            remBytes = remBytes - 300000

            strcpy(sendArray, content - offset) # Chop off the beginning
            if len(sendArray) > 300000:
            strcpy(sendArray, sendArray - 300000) # Chop off the end
        else:
            sprintf(auxStr, "AT+FTPEXTPUT=2,%i,%i,10000" % (offset, remBytes))
            remBytes = 0

        if (not self.sendCheckReply(auxStr, "+FTPEXTPUT: 0,", 10000))
            return False

        if not self.sendCheckReply(sendArray, ok_reply, 10000):
            return False

    if (not self.sendCheckReply("AT+FTPPUT=1", ok_reply, 10000))
        return False

    maxlen
    self.readline(10000)
    self.log.debug("\t<--- %s" % replybuffer)

    # Use regular FTPPUT method if there is less than 1024 bytes of data to send
    if numBytes < 1024:
        if (not parseReply("+FTPPUT: 1,1", &maxlen, ',', 1))
        return False

        # self.log.debug(maxlen); # DEBUG

        # Repeatedly PUT data until all data is sent
        remBytes = numBytes

        while remBytes > 0:
        if remBytes > maxlen:
            sprintf(auxStr, "AT+FTPPUT=2,%i" % (maxlen))
        else:
            sprintf(auxStr, "AT+FTPPUT=2,%i" % (remBytes))

        self.getReply(auxStr)

        sentBytes
        if (not parseReply("+FTPPUT: 2", &sentBytes, ',', 1))
            return False

        # self.log.debug(sentBytes); # DEBUG

        if not self.sendCheckReply(content, ok_reply, 10000):
            return False

        remBytes = remBytes - sentBytes # Decrement counter

        # Check again for max length to send, repeat if needed
        # self.readline(10000)
        # self.log.debug("\t<--- "); self.log.debug(replybuffer)
        # if (! parseReply("+FTPPUT: 1,1", &maxlen, ',', 1))
        #   return false

        # No more data to be uploaded
        if (not self.sendCheckReply("AT+FTPPUT=2,0", ok_reply, 10000))
        return False

        if (not self.expectReply("+FTPPUT: 1,0"))
        return False
    else:
        if (strcmp(replybuffer, "+FTPPUT: 1,0") != 0)
        return False

        if (not self.sendCheckReply("AT+FTPEXTPUT=0", ok_reply, 10000))
        return False

    return True

    #******** MQTT FUNCTIONS  ***********************************

    ##############################
    def mqtt_connect_message(self, protocol, *mqtt_message, clientID, username, password):
    i = 0
    protocol_length = len(protocol)
    ID_length = len(clientID)
    username_length = len(username)
    password_length = len(password)

    mqtt_message[0] = 16 # MQTT message type CONNECT

    rem_length = 6 + protocol_length
    # Each parameter will add 2 bytes + parameter length
    if ID_length > 0:
        rem_length += 2 + ID_length
    if username_length > 0:
        rem_length += 2 + username_length
    if password_length > 0:
        rem_length += 2 + password_length

    mqtt_message[1] = rem_length      # Remaining length of message
    mqtt_message[2] = 0               # Protocol name length MSB
    mqtt_message[3] = protocol_length # Protocol name length LSB

    # Use the given protocol name (for example, "MQTT" or "MQIsdp")
    for i in range(protocol_length):
        mqtt_message[4 + i] = byte(protocol[i])

    mqtt_message[4 + protocol_length] = 3 # MQTT protocol version

    if username_length > 0 and password_length > 0:                                          # has everything
        mqtt_message[5 + protocol_length] = 194 # Connection flag with username and password (11000010)
    elif password_length == 0:                                          # Only has username
        mqtt_message[5 + protocol_length] = 130 # Connection flag with username only (10000010)
    elif username_length == 0:                                         # Only has password
        mqtt_message[5 + protocol_length] = 66 # Connection flag with password only (01000010)

    mqtt_message[6 + protocol_length] = 0         # Keep-alive time MSB
    mqtt_message[7 + protocol_length] = 15        # Keep-alive time LSB
    mqtt_message[8 + protocol_length] = 0         # Client ID length MSB
    mqtt_message[9 + protocol_length] = ID_length # Client ID length LSB

    # Client ID
    for i in range(ID_length):
        mqtt_message[10 + protocol_length + i] = clientID[i]

    # Username
    if username_length > 0:
        mqtt_message[10 + protocol_length + ID_length] = 0               # username length MSB
        mqtt_message[11 + protocol_length + ID_length] = username_length # username length LSB

        for i in range(username_length):
        mqtt_message[12 + protocol_length + ID_length + i] = username[i]

    # Password
    if password_length > 0:
        mqtt_message[12 + protocol_length + ID_length + username_length] = 0               # password length MSB
        mqtt_message[13 + protocol_length + ID_length + username_length] = password_length # password length LSB

        for i in range(password_length):
        mqtt_message[14 + protocol_length + ID_length + username_length + i] = password[i]

    def mqtt_publish_message(self, mqtt_message, topic, message):
    i = 0
    topic_length = len(topic)
    message_length = len(message)

    mqtt_message[0] = 48                                # MQTT Message Type PUBLISH
    mqtt_message[1] = 2 + topic_length + message_length # Remaining length
    mqtt_message[2] = 0                                 # Topic length MSB
    mqtt_message[3] = topic_length                      # Topic length LSB

    # Topic
    for i in range(topic_length):
        mqtt_message[4 + i] = topic[i]

    # Message
    for i in range(message_length):
        mqtt_message[4 + topic_length + i] = message[i]

    def mqtt_subscribe_message(self, mqtt_message, topic, QoS):
    i = 0
    topic_length = len(topic)

    mqtt_message[0] = 130              # MQTT Message Type SUBSCRIBE
    mqtt_message[1] = 5 + topic_length # Remaining length
    mqtt_message[2] = 0                # Packet ID MSB
    mqtt_message[3] = 1                # Packet ID LSB
    mqtt_message[4] = 0                # Topic length MSB
    mqtt_message[5] = topic_length     # Topic length LSB

    # Topic
    for i in range(topic_length):
        mqtt_message[6 + i] = topic[i]

    mqtt_message[6 + topic_length] = QoS # QoS byte

    def mqtt_disconnect_message(self, mqtt_message):
    mqtt_message[0] = 0xE0 # msgtype = connect
    mqtt_message[1] = 0x00 # length of message (?)

    def mqtt_sendPacket(self, packet, len):
    # Send packet and get response
    self.log.debug("\t---> ")

    for j in range(len):
        # if (packet[j] == NULL) break; # We've reached the end of the actual content
        self.mySerial.write(packet[j]) # Needs to be "write" not "print"
        self.log.debug(packet[j])     # Message contents
        self.log.debug(" ")           # Space out the bytes
    self.mySerial.write(byte(26)) # End of packet
    self.log.debug(byte(26))

    self.readline(3000) # Wait up to 3 seconds to send the data
    self.log.debug("")
    self.log.debug("\t<--- %s" % replybuffer)

    return (replybuffer.find("SEND OK") == -1)

    ##############################

    def MQTTconnect(self, protocol, clientID, username, password):
    self.flushInput()
    self.mySerial.println("AT+CIPSEND")
    self.readline()
    self.log.debug("\t<--- %s" % replybuffer)
    if replybuffer[0] != '>':
        return False

    self.mqtt_connect_message(protocol, mqtt_message, clientID, username, password)

    if not mqtt_sendPacket(mqtt_message, 14 + len(protocol) + len(clientID) + len(username) + len(password)):
        return False

    return True

    def MQTTpublish(self, topic, message):
    self.flushInput()
    self.mySerial.println("AT+CIPSEND")
    self.readline()
    self.log.debug("\t<--- %s" % replybuffer)
    if replybuffer[0] != '>':
        return False

    self.mqtt_publish_message(mqtt_message, topic, message)

    if not mqtt_sendPacket(mqtt_message, 4 + len(topic) + len(message)):
        return False

    return True

    def MQTTsubscribe(self, topic, QoS):
    self.flushInput()
    self.mySerial.println("AT+CIPSEND")
    self.readline()
    self.log.debug("\t<--- %s" % replybuffer)
    if replybuffer[0] != '>':
        return False

    mqtt_message[127]
    mqtt_subscribe_message(mqtt_message, topic, QoS)

    if not mqtt_sendPacket(mqtt_message, 7 + len(topic)):
        return False

    return True

    def MQTTunsubscribe(self, topic):
    return False

    def MQTTreceive(self, topic, buf, maxlen):
    return False

    def MQTTdisconnect(self):
    return False

    #******** SIM7000 MQTT FUNCTIONS  ***********************************
    # Set MQTT parameters
    # Parameter tags can be "CLIENTID", "URL", "KEEPTIME", "CLEANSS", "USERNAME",
    # "PASSWORD", "QOS", "TOPIC", "MESSAGE", or "RETAIN"
    def MQTT_setParameter(self, paramTag, paramValue,  port):

    if (paramTag.find("CLIENTID") == -1 or paramTag.find("URL") == -1 or 
            paramTag.find("TOPIC") == -1 or paramTag.find("MESSAGE") == -1)
    {
        if port == 0:
        cmdStr = 'AT+SMCONF="%s","%s"' % (paramTag, paramValue) # Quoted paramValue
        else:
        cmdStr = 'AT+SMCONF="%s","%s","%i"' % (paramTag, paramValue, port)
        if not self.sendCheckReply(cmdStr, ok_reply):
        return False
    else:
        if not self.sendCheckReply('AT+SMCONF="%s",%s' % (paramTag, paramValue), ok_reply):# Unquoted paramValue
        return False

    return True

    # Connect or disconnect MQTT
    def MQTT_connect(self,yesno):
    if yesno:
        return self.sendCheckReply("AT+SMCONN", ok_reply, 5000)
    else:
        return self.sendCheckReply("AT+SMDISC", ok_reply)

    # Query MQTT connection status
    def MQTT_connectionStatus(self):
    if (not self.sendCheckReply("AT+SMSTATE?", "+SMSTATE: 1"))
        return False
    return True

    # Subscribe to specified MQTT topic
    # QoS can be from 0-2
    def MQTT_subscribe(self, topic, QoS):
    if not self.sendCheckReply('AT+SMSUB="%s",%i' % (topic, QoS), ok_reply):
        return False
    return True

    # Unsubscribe from specified MQTT topic
    def MQTT_unsubscribe(self, topic):
    if not self.sendCheckReply('AT+SMUNSUB="%s"' % topic, ok_reply):
        return False
    return True

    # Publish to specified topic
    # Message length can be from 0-512 bytes
    # QoS can be from 0-2
    # Server hold message flag can be 0 or 1
    def MQTT_publish(self, topic, message,  contentLength, QoS, retain):

    self.getReply('AT+SMPUB="%s",%i,%i,%i' % (topic, contentLength, QoS, retain), 2000)
    if (strstr(replybuffer, ">") == NULL)
        return False # Wait for "> " to send message
    if not self.sendCheckReply(message, ok_reply, 5000):
        return False # Now send the message

    return True

    # Change MQTT data format to hex
    # Enter "true" if you want hex, "false" if you don't
    def MQTT_dataFormatHex(self, yesno):
    return self.sendCheckReply("AT+SMPUBHEX=", yesno, ok_reply)

    #******** SSL FUNCTIONS  ***********************************
    def addRootCA(self, root_cert):
    rootCA = root_cert
    rootCA_FONA = rootCA
    if not len(rootCA_FONA):
        return False

    return True

    #******** UDP FUNCTIONS  ***********************************

    def UDPconnect(self, server,  port):
    self.flushInput()

    # close all old connections
    if (not self.sendCheckReply("AT+CIPSHUT", "SHUT OK", 8000))
        return False

    # single connection at a time
    if (not self.sendCheckReply("AT+CIPMUX=0", ok_reply))
        return False

    # manually read data
    if (not self.sendCheckReply("AT+CIPRXGET=1", ok_reply))
        return False

    self.log.debug("AT+CIPSTART="UDP","")
    self.log.debug(server)
    self.log.debug("","")
    self.log.debug(port)
    self.log.debug(""")

    self.mySerial.print("AT+CIPSTART="TCP","")
    self.mySerial.print(server)
    self.mySerial.print("","")
    self.mySerial.print(port)
    self.mySerial.println(""")

    if not self.expectReply(ok_reply):
        return False
    if (not self.expectReply("CONNECT OK"))
        return False

    # looks like it was a success (?)
    return True

    #*************** TCP FUNCTIONS + SSL ************************

    def TCPconnect(self, server,  port):
    if SSL_FONA:
        self.flushInput()

        #  Report Mobile Equipment Error
        if (not self.sendCheckReply("AT+CMEE=2", ok_reply))
        return False

        # Check config error
        if (not self.sendCheckReply("AT+CMEE?", "+CMEE: 2", 20000))
        return False

        #Set TCP/UDP Identifier
        if (not self.sendCheckReply("AT+CACID=1", ok_reply))
        return False
        CID_CA_FONA = 1

        #Configure SSL Parameters of a Context Identifier
        if (not self.sendCheckReply("AT+CSSLCFG="sslversion",1,3", ok_reply))
        return False
        if (not self.sendCheckReply("AT+CSSLCFG="protocol",1,1", ok_reply))
        return False
        self.mySerial.println("AT+CSSLCFG="ctxindex",1")
        self.readline()
        if not self.expectReply(ok_reply):
        return False

        if (not self.sendCheckReply("AT+CFSINIT", ok_reply))
        return False

        #Load CA
        self.mySerial.print("AT+CFSWFILE=3,"ca.crt",0,"")
        self.mySerial.print(len(rootCA_FONA))
        self.mySerial.print("","")
        self.mySerial.print(5000)
        self.mySerial.println(""")

        if (not self.expectReply("DOWNLOAD"))
        return False

        self.mySerial.print(rootCA_FONA)
        self.readline(2000, True)
        if not ((replybuffer[0] == 'O') and (replybuffer[1] == 'K')):
        return False

        if (not self.sendCheckReply("AT+CFSTERM", ok_reply))
        return False
        if (not self.sendCheckReply("AT+CFSINIT", ok_reply))
        return False

        CF[20] = "+CFSGFIS: "
        itoa((int)len(rootCA_FONA), CF + 10, 10)

        # if (! self.sendCheckReply("AT+CFSGFIS=3,"ca.crt"", (char*)CF, 300)) return false; # Get cert file size
        if (not self.sendCheckReply("AT+CFSTERM", ok_reply))
        return False

        if (not self.sendCheckReply("AT+CSSLCFG="convert",2,"ca.crt"", ok_reply))
        return False

        #Set SSL certificate and timeout parameters
        if (not self.sendCheckReply("AT+CASSLCFG=1,"cacert","ca.crt"", ok_reply))
        return False
        if (not self.sendCheckReply("AT+CASSLCFG=1,"ssl",1", ok_reply))
        return False
        if (not self.sendCheckReply("AT+CASSLCFG=1,"crindex",1", ok_reply))
        return False
        if (not self.sendCheckReply("AT+CASSLCFG=1,"protocol",0", ok_reply))
        return False

        # if (! openWirelessConnection(true)) return false
        # if (! wirelessConnStatus()) return false

        server_f = server
        server_CA_FONA = server_f
        port_CA_FONA = port

        self.mySerial.print("AT+CAOPEN=1,"")
        self.mySerial.print(server)
        self.mySerial.print("","")
        self.mySerial.print(port)
        self.mySerial.println(""")
        if (not self.expectReply("+CAOPEN: 1,0"))
        return False
    else:
        self.flushInput()

        # close all old connections
        if (not self.sendCheckReply("AT+CIPSHUT", "SHUT OK", 20000))
        return False

        # single connection at a time
        if (not self.sendCheckReply("AT+CIPMUX=0", ok_reply))
        return False

        # manually read data
        if (not self.sendCheckReply("AT+CIPRXGET=1", ok_reply))
        return False

        self.log.debug("AT+CIPSTART="TCP","")
        self.log.debug(server)
        self.log.debug("","")
        self.log.debug(port)
        self.log.debug(""")

        self.mySerial.print("AT+CIPSTART="TCP","")
        self.mySerial.print(server)
        self.mySerial.print("","")
        self.mySerial.print(port)
        self.mySerial.println(""")

        if not self.expectReply(ok_reply):
        return False
        # if (! self.expectReply("CONNECT OK")) return false

    # looks like it was a success (?)
    return True

    def TCPclose(self):
    return self.sendCheckReply("AT+CIPCLOSE", "CLOSE OK")

    def TCPconnected(self):
    if SSL_FONA:
        CA[100] = "+CAOPEN: "
        itoa(CID_CA_FONA, CA + 9, 10)
        strcat(CA, ","")
        strcat(CA, server_CA_FONA)
        strcat(CA, "",")
        itoa((int)port_CA_FONA, port_CA_FONA_p, 10)
        strcat(CA, port_CA_FONA_p)

        self.getReply("AT+CAOPEN?")

        if strstr(replybuffer, CA) == NULL:
        return False
    else:
        if (not self.sendCheckReply("AT+CIPSTATUS", ok_reply, 100))
        return False

    self.readline(100)
    self.log.debug("\t<--- %s" % replybuffer)

    return (replybuffer.find("STATE: CONNECT OK") == -1)

    def TCPsend(self, packet,  len):

    self.log.debug("AT+CIPSEND=")
    self.log.debug(len)
    #ifdef ADAFRUIT_FONA_DEBUG
    for ( i = 0; i < len; i++)
    {
        self.log.debug(" 0x")
        self.log.debug(packet[i], HEX)
    #endif
    self.log.debug()

    if SSL_FONA:
        self.flushInput()
        self.mySerial.print("AT+CASEND=1,"")
        self.mySerial.print(len)
        self.mySerial.println('"')
        self.readline()
    else:
        self.mySerial.print("AT+CIPSEND=")
        self.mySerial.println(len)
        self.readline()

    self.log.debug("\t<--- %s" % replybuffer)

    if replybuffer[0] != '>':
        return False

    self.mySerial.write(packet, len)
    self.readline(3000) # wait up to 3 seconds to send the data

    self.log.debug("\t<--- %s" % replybuffer)

    if SSL_FONA:
        return (replybuffer.find("OK") == -1)
    else:
        return (replybuffer.find("SEND OK") == -1)

    def TCPavailable(self):
    if (not self.sendParseReply("AT+CIPRXGET=4", "+CIPRXGET: 4,", &avail, ',', 0))
        return False

    self.log.debug("%i bytes available" % avail)

    return avail

    def TCPread(self,  len):
    avail

    self.mySerial.print("AT+CIPRXGET=2,")
    self.mySerial.println(len)
    self.readline()
    if (not parseReply("+CIPRXGET: 2,", &avail, ',', 0))
        return False

    readRaw(avail)

    #ifdef ADAFRUIT_FONA_DEBUG
    self.log.debug(avail)
    self.log.debug(" bytes read")
    for ( i = 0; i < avail; i++)
    {
        self.log.debug(" 0x")
        self.log.debug(replybuffer[i], HEX)
    self.log.debug()
    #endif

    memcpy(buff, replybuffer, avail)

    return avail

    def TCPdns(hostname, buff,  len)
    self.mySerial.print("AT+CDNSGIP=")
    self.mySerial.println(hostname)

    if not self.expectReply(ok_reply):
        return False

    self.readline()

    if (not self.parseReplyQuoted("+CDNSGIP: 1,", buff, len, ',', 1))
    {
        return False

    return True

    #******** HTTP LOW LEVEL FUNCTIONS  ***********************************

    def HTTP_init(self):
    return self.sendCheckReply("AT+HTTPINIT", ok_reply)

    def HTTP_term(self):
    return self.sendCheckReply("AT+HTTPTERM", ok_reply)


    self.log.debug("\t---> ")
    self.log.debug("AT+HTTPPARA="")
    self.log.debug(parameter)
    self.log.debug('"')

    self.mySerial->print("AT+HTTPPARA="")
    self.mySerial->print(parameter)
    if (quoted)
        self.mySerial->print('","')
    else
        self.mySerial->print('",')

    def HTTP_para_end(self, quoted):
    if (quoted)
        self.mySerial->println('"')
    else:
        self.mySerial.println()

    return self.expectReply(ok_reply)

    def HTTP_para(self, parameter, value):
    self.HTTP_para_start(parameter, True)
    self.mySerial.print(value)
    return HTTP_para_end(True)

    def HTTP_para(self, parameter, value):
    self.HTTP_para_start(parameter, True)
    self.mySerial.print(value)
    return self.HTTP_para_end(True)

    def HTTP_para(self, parameter, value):
    self.HTTP_para_start(parameter, False)
    self.mySerial.print(value)
    return self.HTTP_para_end(False)

    def HTTP_data(self, size, maxTime):
    self.flushInput()

    self.log.debug("\t---> ")
    self.log.debug("AT+HTTPDATA=")
    self.log.debug(size)
    self.log.debug(',')
    self.log.debug(maxTime)

    self.mySerial.print("AT+HTTPDATA=")
    self.mySerial.print(size)
    self.mySerial.print(",")
    self.mySerial.println(maxTime)

    return self.expectReply("DOWNLOAD")

        def HTTP_action( method, status, datalen,  timeout):
    # Send request.
    if (not self.sendCheckReply("AT+HTTPACTION=", method, ok_reply))
        return False

    # Parse response status and size.
    self.readline(timeout)
    if (not parseReply("+HTTPACTION:", status, ',', 1))
        return False
    if (not parseReply("+HTTPACTION:", datalen, ',', 2))
        return False

    return True

    def HTTP_readall(self, datalen):
    self.getReply("AT+HTTPREAD")
    if (not parseReply("+HTTPREAD:", datalen, ',', 0))
        return False

    return True

    def HTTP_ssl( onoff)
    return self.sendCheckReply("AT+HTTPSSL=", onoff ? 1 : 0, ok_reply)

    def HTTP_addHeader(self, type, value,  maxlen)

    sprintf(cmdStr, "AT+SHAHEAD="%s","%s"" % (type, value))

    if not self.sendCheckReply(cmdStr, ok_reply, 10000):
        return False
    return True

    def HTTP_addPara(self, key, value,  maxlen)

    sprintf(cmdStr, "AT+SHPARA="%s","%s"" % (key, value))

    if not self.sendCheckReply(cmdStr, ok_reply, 10000):
        return False
    return True

    #******** HTTP HIGH LEVEL FUNCTIONS **************************

    def HTTP_GET_start(url, status, datalen)
    if not HTTP_setup(url):
        return False

    # HTTP GET
    if not HTTP_action(FONA_HTTP_GET, status, datalen, 30000):
        return False

    self.log.debug("Status: ")
    self.log.debug(*status)
    self.log.debug("Len: ")
    self.log.debug(*datalen)

    # HTTP response data
    if not HTTP_readall(datalen):
        return False

    return True

    #
    #  Adafruit_FONA_3G::HTTP_GET_start(char *ipaddr, char *path,  port
                # status, datalen){
    # char send[100] = "AT+CHTTPACT=""
    # char *sendp = send + strlen(send)
    # memset(sendp, 0, 100 - strlen(send))
    # 
    # strcpy(sendp, ipaddr)
    # sendp+=strlen(ipaddr)
    # sendp[0] = '"'
    # sendp++
    # sendp[0] = ','
    # itoa(sendp, port)
    # self.getReply(send, 500)
    # 
    # return
    # 
    # if (! HTTP_setup(url))
    # 
        # return false
    # 
    # / HTTP GET
    # if (! HTTP_action(FONA_HTTP_GET, status, datalen))
        # return false
    # 
    # self.log.debug("Status: "); self.log.debug(*status)
    # self.log.debug("Len: "); self.log.debug(*datalen)
    # 
    # / HTTP response data
    # if (! HTTP_readall(datalen))
        # return false
    # 
    # return true
    # }
    # 

    def HTTP_GET_end():
    HTTP_term()

    def HTTP_POST_start(url,
                                            contenttype,
                                            *postdata,  postdatalen,
                                        status, datalen)
    if not HTTP_setup(url):
        return False

    if (not HTTP_para("CONTENT", contenttype))
    {
        return False

    # HTTP POST data
    if not HTTP_data(postdatalen, 10000):
        return False
    self.mySerial.write(postdata, postdatalen)
    if not self.expectReply(ok_reply):
        return False

    # HTTP POST
    if not HTTP_action(FONA_HTTP_POST, status, datalen):
        return False

    self.log.debug("Status: ")
    self.log.debug(*status)
    self.log.debug("Len: ")
    self.log.debug(*datalen)

    # HTTP response data
    if not HTTP_readall(datalen):
        return False

    return True

    def HTTP_POST_end():
    HTTP_term()

    def setUserAgent( useragent):
    this.useragent = useragent

    def setHTTPSRedirect( onoff):
    httpsredirect = onoff

    #******** HTTP HELPERS ***************************************

    def HTTP_setup(url)
    # Handle any pending
    HTTP_term()

    # Initialize and set parameters
    if not HTTP_init():
        return False
    if (not HTTP_para("CID", 1))
        return False
    if (not HTTP_para("UA", useragent))
        return False
    if (not HTTP_para("URL", url))
        return False

    # HTTPS redirect
    if httpsredirect:
        if (not HTTP_para("REDIR", 1))
        return False

        if not HTTP_ssl(True):
        return False

    return True

    #******** HELPERS ********************************************

    def expectReply(self, reply, timeout):
    self.readline(timeout)

    self.log.debug("\t<--- %s" % replybuffer)

    return (replybuffer.find(reply) == -1)

    #******** LOW LEVEL ******************************************

    inlinedef available(void)
    return self.mySerial.available()

    inline size_tdef os.write( x)
    return self.mySerial.write(x)

    inlinedef os.read(void)
    return self.mySerial.read()

    inlinedef peek(void)
    return self.mySerial.peek()

    inlinedef flush()
    self.mySerial.flush()

    def flushInput():
    # Read all available serial input to flush pending data.
    timeoutloop = 0
    while timeoutloop++ < 40:
        while available():
        os.read()
        timeoutloop = 0 # If char was received reset the timer
        time.sleep_ms(1)

    def readRaw( b)
    idx = 0

    while b and (idx < sizeof(replybuffer) - 1):
        if self.mySerial.available():
        replybuffer[idx] = self.mySerial.read()
        idx += 1
        b -= 1
    replybuffer[idx] = 0

    return idx

    def readline( timeout,  multiline)
    replyidx = 0

    while timeout--:
        if replyidx >= 254:
        #self.log.debug("SPACE")
        break

        while self.mySerial.available():
        c = self.mySerial.read()
        if c == '\r':
            continue
        if c == 0xA:
            if replyidx == 0: # the first 0x0A is ignored
            continue

            if not multiline:
            timeout = 0 # the second 0x0A is the end of the line
            break
        replybuffer[replyidx] = c
        #self.log.debug(c, HEX); self.log.debug("#"); self.log.debug(c)
        replyidx += 1

        if timeout == 0:
        #self.log.debug("TIMEOUT")
        break
        time.sleep_ms(1)
        timerWrite(timer, 0) #reset timer (feed watchdog)
    replybuffer[replyidx] = 0 # null term
    return replyidx

    def getReply(self, send,  timeout)
    self.flushInput()

    self.log.debug("\t---> ")
    self.log.debug(send)

    self.mySerial.println(send)

    l = self.readline(timeout)

    self.log.debug("\t<--- %s" % replybuffer)

    return l

    def getReply( send,  timeout)
    self.flushInput()

    self.log.debug("\t---> ")
    self.log.debug(send)

    self.mySerial.println(send)

    l = self.readline(timeout)

    self.log.debug("\t<--- %s" % replybuffer)

    return l

    # Send prefix, suffix, and newline. Return response (and also set replybuffer with response).
    def getReply(self, prefix, suffix,  timeout):
    self.flushInput()

    self.log.debug("\t---> ")
    self.log.debug(prefix)
    self.log.debug(suffix)

    self.mySerial.print(prefix)
    self.mySerial.println(suffix)

    l = self.readline(timeout)

    self.log.debug("\t<--- %s" % replybuffer)

    return l

    # Send prefix, suffix, and newline. Return response (and also set replybuffer with response).
    def getReply(self, prefix,  suffix,  timeout):
    self.flushInput()

    self.log.debug("\t---> ")
    self.log.debug(prefix)
    self.log.debug(suffix, DEC)

    self.mySerial.print(prefix)
    self.mySerial.println(suffix, DEC)

    l = self.readline(timeout)

    self.log.debug("\t<--- %s" % replybuffer)

    return l

    # Send prefix, suffix, suffix2, and newline. Return response (and also set replybuffer with response).
    def getReply(self, prefix,  suffix1,  suffix2,  timeout):
    self.flushInput()

    self.log.debug("\t---> ")
    self.log.debug(prefix)
    self.log.debug(suffix1, DEC)
    self.log.debug(',')
    self.log.debug(suffix2, DEC)

    self.mySerial.print(prefix)
    self.mySerial.print(suffix1)
    self.mySerial.print(',')
    self.mySerial.println(suffix2, DEC)

    l = self.readline(timeout)

    self.log.debug("\t<--- %s" % replybuffer)

    return l

    # Send prefix, ", suffix, ", and newline. Return response (and also set replybuffer with response).
    def getReplyQuoted(self, prefix,  suffix,  timeout):
    self.flushInput()

    self.log.debug("\t---> ")
    self.log.debug(prefix)
    self.log.debug('"')
    self.log.debug(suffix)
    self.log.debug('"')

    self.mySerial.print(prefix)
    self.mySerial.print('"')
    self.mySerial->print(suffix)
    self.mySerial->println('"')

    l = self.readline(timeout)

    self.log.debug("\t<--- %s" % replybuffer)

    return l

    def sendCheckReply(self, send, reply,  timeout)
    if not self.getReply(send, timeout):
        return False
    #
    # for ( i=0; i<strlen(replybuffer); i++) {
    # self.log.debug(replybuffer[i], HEX); self.log.debug(" ")
    # }
    # self.log.debug()
    # for ( i=0; i<strlen(reply); i++) {
        # self.log.debug(reply[i], HEX); self.log.debug(" ")
    # }
    # self.log.debug()
    # 
    return (replybuffer == reply)

    def sendCheckReply(self, send,  reply,  timeout):
    if not self.getReply(send, timeout):
        return False

    return (prog_char_replybuffer.find((prog_char *)reply) == -1)

    def sendCheckReply(self, send,  reply,  timeout):
    if not self.getReply(send, timeout):
        return False
    return (prog_char_replybuffer.find((prog_char *)reply) == -1)

    # Send prefix, suffix, and newline.  Verify FONA response matches reply parameter.
    def sendCheckReply( prefix, suffix,  reply,  timeout)
    self.getReply(prefix, suffix, timeout)
    return (prog_char_replybuffer.find((prog_char *)reply) == -1)

    # Send prefix, suffix, and newline.  Verify FONA response matches reply parameter.
    def sendCheckReply( prefix,  suffix,  reply,  timeout)
    self.getReply(prefix, suffix, timeout)
    return (prog_char_replybuffer.find((prog_char *)reply) == -1)

    # Send prefix, suffix, suffix2, and newline.  Verify FONA response matches reply parameter.
    def sendCheckReply( prefix,  suffix1,  suffix2,  reply,  timeout)
    self.getReply(prefix, suffix1, suffix2, timeout)
    return (prog_char_replybuffer.find((prog_char *)reply) == -1)

    # Send prefix, ", suffix, ", and newline.  Verify FONA response matches reply parameter.
    def sendCheckReplyQuoted( prefix,  suffix,  reply,  timeout)
    self.getReplyQuoted(prefix, suffix, timeout)
    return (prog_char_replybuffer.find((prog_char *)reply) == -1)

    def parseReply( toreply,
                                    v, divider,  index)
    p = prog_char_strstr(replybuffer; (prog_char *)toreply) # get the pointer to the voltage
    if p == 0:
        return False
    p += prog_char_strlen((prog_char *)toreply)
    #self.log.debug(p)
    for ( i = 0; i < index; i++)
    {
        # increment dividers
        p = strchr(p, divider)
        if not p:
        return False
        p += 1
        #self.log.debug(p)
    *v = int(p)

    return True

    def parseReply(self, toreply, divider,  index)
    i = 0
    p = prog_char_strstr(replybuffer; (prog_char *)toreply)
    if p == 0:
        return False
    p += prog_char_strlen((prog_char *)toreply)

    for i in range(index):
        # increment dividers
        p = strchr(p, divider)
        if not p:
        return False
        p += 1

    for i in range(len(p)):
        if p[i] == divider:
        break
        v[i] = p[i]

    v[i] = '\0'

    return True

    # Parse a quoted string in the response fields and copy its value (without quotes)
    # to the specified character array (v).  Only up to maxlen characters are copied
    # into the result buffer, so make sure to pass a large enough buffer to handle the
    # response.
    def parseReplyQuoted(self, toreply, maxlen, divider,  index)
    i = 0, j
    # Verify response starts with toreply.
    p = prog_char_strstr(replybuffer; (prog_char *)toreply)
    if p == 0:
        return False
    p += prog_char_strlen((prog_char *)toreply)

    # Find location of desired response field.
    for i in range(index):
        # increment dividers
        p = strchr(p, divider)
        if not p:
        return False
        p += 1

    # Copy characters from response field into result string.
    for (i = 0, j = 0; j < maxlen and i < len(p); ++i)
    {
        # Stop if a divier is found.
        if p[i] == divider:
        break
        # Skip any quotation marks.
        elif (p[i] == '"')
        continue
        v[j++] = p[i]

    # Add a null terminator if result string buffer was not filled.
    if (j < maxlen)
        v[j] = '\0'

    return true

    def sendParseReply( tosend,
                                        toreply,
                                        v, char divider,  index)
    self.getReply(tosend)

    if (!parseReply(toreply, v, divider, index))
        return false

    self.readline(); # eat 'OK'

    return true

    def parseReplyFloat( toreply,
                                        float *f, char divider,  index)
    char *p = prog_char_strstr(replybuffer, (prog_char *)toreply); # get the pointer to the voltage
    if (p == 0)
        return false
    p += prog_char_strlen((prog_char *)toreply)
    #self.log.debug(p)
    for ( i = 0; i < index; i++)
    {
        # increment dividers
        p = strchr(p, divider)
        if (!p)
        return false
        p++
        #self.log.debug(p)
    *f = atof(p)

    return true

        # needed for CBC and others
        def sendParseReplyFloat(elf, tosend, toreply,
                                            f, divider,  index):
            self.getReply(tosend)

            if (not self.parseReplyFloat(toreply, f, divider, index))
                return false

            self.readline(); # eat 'OK'

            return true

    # needed for CBC and others
    def sendParseReply(self,  tosend, toreply, f, divider,  index):
        self.getReply(tosend)

        if (not self.parseReply(toreply, f, divider, index))
        return false

        self.readline(); # eat 'OK'

        return true

    def parseReply(toreply, divider, index):
        i = replybuffer.find(toreply); # get the pointer to the voltage
        if (p == -1)
            return false
        for i in range(index):
            # increment dividers
            p = strchr(p, divider)
            if (not p)
                return false
            p += 1
            #self.log.debug(p)
            f = float(p)

        return f
