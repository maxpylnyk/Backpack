#pragma once

#include "module_base.h"
#include <Arduino.h>

#define BIT_BANG_FREQ 10000

class Thor67 : public ModuleBase
{
public:
    void Init();
    void SendIndexCmd(uint8_t index);

private:
    void writeRegister(uint32_t reg);
    void updateFpfd();
    void spiTransfer32(uint32_t v);
    uint32_t makeR0(uint16_t INT, uint16_t FRAC) const;
    uint32_t makeR1() const;
    uint32_t makeR2() const;
    uint32_t makeR3() const;
    uint32_t makeR4(uint8_t rfDivSel) const;
    uint32_t makeR5() const;
    uint8_t chooseRfDivider(double rfMHz, double& vcoMHz);
    bool calcIntFrac(double vcoMHz, uint16_t&INT, uint16_t&FRAC);

    bool rfEnable = true;
    bool refDoubler = false;
    bool refDiv2 = false;
    uint8_t prescaler = 1;
    uint8_t pwrLevel = 3;
    uint16_t rDiv = 26;
    uint16_t mod = 1000;
    uint16_t diff_mhz = 479;
    double refHz = 26e6;
    double fpfdHz = 0.0;
};