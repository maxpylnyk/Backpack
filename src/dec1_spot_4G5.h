#pragma once

#include "module_base.h"
#include "common.h"
#include "crc.h"
#include "crsf_protocol.h"
#include <Arduino.h>
#include <HardwareSerial.h>

#define VRX_UART_BAUD   115200
#define F_MIN 505
#define F_MAX 6480

class Dec1Spot4G5 : public ModuleBase
{
public:
    void Init();
    void SendIndexCmd(uint8_t index);

private:
    HardwareSerial uart = HardwareSerial(1);    

    uint16_t crc16_ccitt_false(const uint8_t *data, uint8_t length);
};
