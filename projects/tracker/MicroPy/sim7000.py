import time

class Sim7000:
    def __init__(self, log):
        self.log = log

    #--------------------------------------------------------------------
    def prepareMessage(self):
        if not fona.getGPS([latitude, longitude, speed_kph, heading, sats]):
            return False
        else:
            # Read the module's power supply voltage
            vbat = fona.getBattVoltage()

            pending = True

            # TODO handle events
            msg = "%s,%i,%i,%i,%.5f,%.5f,%i,%i,%i,%i" % (imei, seq_num, mode, 0, latitude, longitude, int(speed_kph), heading, 0, vbat)
            seq_num += 1

            self.log.debug("Latitude: %.5f" % latitude)
            self.log.debug("Longitude: %.5f" % longitude)
            self.log.debug("Speed: %.1f km/h" % speed_kph)
            self.log.debug("Heading: %i deg" % heading)
            self.log.debug("N sats: %i" % sats)
            self.log.debug(msg)

            return True

    #--------------------------------------------------------------------
    def checkSMS(self):
        fona.setFunctionality(1) # AT+CFUN=1
        time.sleep_ms(100)

        numSMS = fona.getNumSMS()
        self.log.debug("Number of SMS available: %i" % numSMS)
        
        if numSMS > 0:
            # Retrieve SMS value.
            for i in range(numSMS):
                if fona.readSMS(i, smsBuffer, MAX_INPUT_LENGTH, smslen): # pass in buffer and max len!
                    self.log.debug("Read SMS in slot %i" % i)

                # Delete the original message after it is processed.
                if fona.deleteSMS(i):
                    self.log.debug("SMS in slot %i have been deleted!" % i)
                    return True
                else:
                    self.log.debug("Couldn't delete SMS in slot %i!" % i)
        else:
            self.log.debug("There are no SMS available!")

        return False

    #--------------------------------------------------------------------
    def turnOF(self):
        digitalWrite(SIM_PWR, LOW)

    #--------------------------------------------------------------------
    def turnON(self):
        digitalWrite(SIM_PWR, HIGH)
        for i in range(3):
            if configure():
                break
            turnOF)
            time.sleep_ms(100) # Short pause to let the capacitors discharge
            digitalWrite(SIM_PWR, HIGH)

    #--------------------------------------------------------------------
    def reset(self):
        self.turnOF()
        time.sleep_ms(100) # Short pause to let the capacitors discharge
        self.turnON()

    #--------------------------------------------------------------------
    def configure(self):
        # SIM7000 takes about 3s to turn on
        # Press Arduino reset button if the module is still turning on and the board doesn't find it.
        # When the module is on it should communicate right after pressing reset

        self.log.debug("Starting configuration...")
        # Software serial:
        fonaSS.begin(115200, SERIAL_8N1, FONA_TX, FONA_RX) # baud rate, protocol, ESP32 RX pin, ESP32 TX pin

        if not fona.begin(fonaSS, PWRKEY):
            self.log.error("Couldn't find FONA")
            return False
            # while (1)
            #     ; // Don't proceed if it couldn't find the device
        if DEBUG:
            Serial.println("FONA is OK")

        # Turn off modem
        fona.setFunctionality(0) # AT+CFUN=0

        # Configure
        fona.setNetworkSettings(APN_NAME) # For Hologram SIM card
        fona.setPreferredMode(38)            # Use LTE only, not 2G
        fona.setPreferredLTEMode(1)          # Use LTE CAT-M only, not NB-IoT
        return True

    #--------------------------------------------------------------------
    def setup(self):
        pinMode(SIM_PWR, OUTPUT)

        # Turn on the module
        turnON()

        # Print module IMEI number.
        imei_len = fona.getIMEI(imei)
        if imei_len > 0:
            self.log.debug("Module IMEI: %i" % imei)

    #--------------------------------------------------------------------
    def netStatus(self):
        n = fona.getNetworkStatus()
        self.log.debug("Network status %i:" % n)
        if n == 0:
            self.log.debug("Not registered")
        if n == 1:
            self.log.debug("Registered (home)")
        if n == 2:
            self.log.debug("Not registered (searching)")
        if n == 3:
            self.log.debug("Denied")
        if n == 4:
            self.log.debug("Unknown")
        if n == 5:
            self.log.debug("Registered roaming")
        if not (n == 1 or n == 5):
            return False
        else:
            return True

    #--------------------------------------------------------------------
    def uploadData(self):
        # Open wireless connection if not already activated
        if not fona.wirelessConnStatus():
            if not fona.openWirelessConnection(True):
                self.log.debug("Failed to enable connection, retrying...")
                return False
            self.log.debug("Data enabled!")
        else:
            self.log.debug("Data already enabled!")

        # HTTP send data
        if not fona.HTTP_connect(SERVER):
            self.log.debug("Failed to connect to server!")
            return False

        # Upload data
        if not fona.HTTP_POST(ADDR, msg, len(msg)):
            self.log.debug("Failed to upload!") # Send GPS location
            return False
