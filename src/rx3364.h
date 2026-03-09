#pragma once

#include "module_base.h"
#include <Arduino.h>

#define BIT_BANG_FREQ 10000

class RX3364 : public ModuleBase
{
public:
    void Init();
    void SendIndexCmd(uint8_t index);

private:
    void writeRegister(uint32_t reg);

    const float ifMHz = 480.0;
    const uint8_t oscFreqMHz = 160;
    const uint32_t prescaler = 0x10000u;
    const uint8_t addressShift = 0x00;
};