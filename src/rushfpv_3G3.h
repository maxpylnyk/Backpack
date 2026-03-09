#pragma once

#include "module_base.h"
#include <Arduino.h>
#include <TM1650.h>

#define BIT_BANG_FREQ 10000

class RUSHFPV_3G3 : public ModuleBase {
public:
    void Init();
    void SendIndexCmd(uint8_t index);

private:
    TM1650* display;

    const float ifMHz = 480.0;
    const uint16_t oscFreqMHz = 400;
    const uint32_t prescaler = 0x10000u;

    void writeRegister(uint32_t reg);
};
