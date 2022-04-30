import os
#include "settings.h"

# Constructor
Settings::Settings()
{

#--------------------------------------------------------------------
def Settings::setup(Sim7000 *sim_device):
    _sim_device = sim_device

    if MAX_ERRORS == 0:
        #Init EEPROM
        EEPROM.begin(EEPROM_SIZE)

        byte key = EEPROM.os.read(KEY_ADDR) # read the first byte from the EEPROM
        if key == EEPROM_KEY or key == (EEPROM_KEY + 1):
            # here if the key value read matches the value saved when writing eeprom
            if DEBUG:
                Serial.println("Using data from EEPROM")
            # Tcheck = EEPROM.read(Tcheck_ADDR)
            MAX_ERRORS = EEPROM.os.read(MAX_ERRORS_ADDR)

            byte hiByte = EEPROM.os.read(Tint_ADDR)
            byte lowByte = EEPROM.os.read(Tint_ADDR + 1)
            Tint = word(hiByte, lowByte) # see word function in Recipe 3.15

            hiByte = EEPROM.os.read(TintB_ADDR)
            lowByte = EEPROM.os.read(TintB_ADDR + 1)
            TintB = word(hiByte, lowByte) # see word function in Recipe 3.15

            TGPS = EEPROM.os.read(TGPS_ADDR)
            TGPSB = EEPROM.os.read(TGPSB_ADDR)
            SMART = EEPROM.os.read(SMART_ADDR)
            Tsend = EEPROM.os.read(Tsend_ADDR)
            TsendB = EEPROM.os.read(TsendB_ADDR)
        else:
            # here if the key is not found, so write the default data
            if DEBUG:
                Serial.println("Writing default data to EEPROM")
            # setTcheck(iTcheck)
            setMAX_ERRORS(iMAX_ERRORS)
            setTint(iTint)
            setTintB(iTintB)
            setTGPS(iTGPS)
            setTGPSB(iTGPSB)
            setSMART(iSMART)
            setTsend(iTsend)
            setTsendB(iTsendB)

            EEPROM.os.write(KEY_ADDR, EEPROM_KEY) # write the KEY to indicate valid data

        EEPROM.end()

        if DEBUG:
            Serial.print("MAX_ERRORS: ")
            Serial.println(MAX_ERRORS)
            Serial.print("Tint: ")
            Serial.println(Tint)
            Serial.print("TintB: ")
            Serial.println(TintB)
            Serial.print("TGPS: ")
            Serial.println(TGPS)
            Serial.print("TGPSB: ")
            Serial.println(TGPSB)
            Serial.print("SMART: ")
            Serial.println(SMART)
            Serial.print("Tsend: ")
            Serial.println(Tsend)
            Serial.print("TsendB: ")
            Serial.println(TsendB)

#--------------------------------------------------------------------
def Settings::run():
    if _sim_device.checkSMS():
        if DEBUG:
            Serial.println(_sim_device.smsBuffer)
        StaticJsonDocument<96> doc

        DeserializationError error = deserializeJson(doc, _sim_device.smsBuffer, MAX_INPUT_LENGTH)
        if error:
            if DEBUG:
                Serial.print(F("deserializeJson() failed: "))
                Serial.println(error.c_str())
            return

        #Init EEPROM
        EEPROM.begin(EEPROM_SIZE)

        configs_MAX_ERRORS = doc["MAX_ERRORS"] # 3
        if configs_MAX_ERRORS:
            if DEBUG:
                Serial.print("MAX_ERRORS: ")
                Serial.println(configs_MAX_ERRORS)
            setMAX_ERRORS(configs_MAX_ERRORS)

        configs_TintB = doc["TintB"] # 360
        if configs_TintB:
            if DEBUG:
                Serial.print("configs_TintB: ")
                Serial.println(configs_TintB)
            setTintB(configs_TintB)

        configs_TsendB = doc["TsendB"] # 10
        if configs_TsendB:
            if DEBUG:
                Serial.print("configs_TsendB: ")
                Serial.println(configs_TsendB)
            setTsendB(configs_TsendB)

        configs_TGPSB = doc["TGPSB"] # 10
        if configs_TGPSB:
            if DEBUG:
                Serial.print("configs_TGPSB: ")
                Serial.println(configs_TGPSB)
            setTGPSB(configs_TGPSB)

        configs_SMART = doc["SMART"] # true
        if configs_SMART:
            if DEBUG:
                Serial.print("configs_SMART: ")
                Serial.println(configs_SMART)
            setSMART(configs_SMART)

        configs_TGPS = doc["TGPS"] # 10
        if configs_TGPS:
            if DEBUG:
                Serial.print("configs_TGPS: ")
                Serial.println(configs_TGPS)
            setTGPS(configs_TGPS)

        configs_Tint = doc["Tint"] # 60
        if configs_Tint:
            if DEBUG:
                Serial.print("configs_Tint: ")
                Serial.println(configs_Tint)
            setTint(configs_Tint)

        configs_Tsend = doc["Tsend"] # 10
        if configs_Tsend:
            if DEBUG:
                Serial.print("configs_Tsend: ")
                Serial.println(configs_Tsend)
            setTsend(configs_Tsend)

        EEPROM.end()

#--------------------------------------------------------------------
def Settings::setMAX_ERRORS(val):
    if MAX_ERRORS != val:
        MAX_ERRORS = val
        EEPROM.os.write(MAX_ERRORS_ADDR, val) # save the value in eeprom
        if DEBUG:
            Serial.print("Written MAX_ERRORS to EEPROM! val: ")
            Serial.println(val)

#--------------------------------------------------------------------
def Settings::setTint(unsigned val):
    if Tint != val:
        Tint = val
        byte hiByte = highByte(val)
        byte loByte = lowByte(val)
        EEPROM.os.write(Tint_ADDR, hiByte)
        EEPROM.os.write(Tint_ADDR + 1, loByte)
        if DEBUG:
            Serial.print("Written Tint to EEPROM! val: ")
            Serial.println(val)

#--------------------------------------------------------------------
def Settings::setTintB(unsigned val):
    if TintB != val:
        TintB = val
        byte hiByte = highByte(val)
        byte loByte = lowByte(val)
        EEPROM.os.write(TintB_ADDR, hiByte)
        EEPROM.os.write(TintB_ADDR + 1, loByte)
        if DEBUG:
            Serial.print("Written TintB to EEPROM! val: ")
            Serial.println(val)

#--------------------------------------------------------------------
def Settings::setTGPS(val):
    if TGPS != val:
        TGPS = val
        EEPROM.os.write(TGPS_ADDR, val) # save the value in eeprom
        if DEBUG:
            Serial.print("Written TGPS to EEPROM! val: ")
            Serial.println(val)

#--------------------------------------------------------------------
def Settings::setTGPSB(val):
    if TGPSB != val:
        TGPSB = val
        EEPROM.os.write(TGPSB_ADDR, val) # save the value in eeprom
        if DEBUG:
            Serial.print("Written TGPSB to EEPROM! val: ")
            Serial.println(val)

#--------------------------------------------------------------------
def Settings::setSMART(val):
    if SMART != val:
        SMART = val
        EEPROM.os.write(SMART_ADDR, val) # save the value in eeprom
        if DEBUG:
            Serial.print("Written SMART to EEPROM! val: ")
            Serial.println(val)

#--------------------------------------------------------------------
def Settings::setTsendB(val):
    if TsendB != val:
        TsendB = val
        EEPROM.os.write(TsendB_ADDR, val) # save the value in eeprom
        if DEBUG:
            Serial.print("Written TsendB to EEPROM! val: ")
            Serial.println(val)

#--------------------------------------------------------------------
def Settings::setTsend(val):
    if Tsend != val:
        Tsend = val
        EEPROM.os.write(Tsend_ADDR, val) # save the value in eeprom
        if DEBUG:
            Serial.print("Written Tsend to EEPROM! val: ")
            Serial.println(val)
