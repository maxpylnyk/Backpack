#include "thor67.h"
#include <SPI.h>
#include "logging.h"

void
Thor67::Init() {
    ModuleBase::Init();

    pinMode(PIN_MOSI, OUTPUT);
    pinMode(PIN_CLK, OUTPUT);
    digitalWrite(PIN_MOSI, LOW);
    digitalWrite(PIN_CLK, LOW);
    pinMode(PIN_CS, OUTPUT);
    digitalWrite(PIN_CS, HIGH);
    updateFpfd();

    writeRegister(makeR5());
    writeRegister(makeR4(0));
    writeRegister(makeR3());
    writeRegister(makeR2());
    writeRegister(makeR1());

    DBGLN("Thor67 init complete");
}

void Thor67::updateFpfd() {
    double mul = refDoubler ? 2.0 : 1.0;
    double div = rDiv * (refDiv2 ? 2.0 : 1.0);
    fpfdHz = (refHz * mul) / div;
}

uint32_t Thor67::makeR0(uint16_t INT, uint16_t FRAC) const {
  return ((uint32_t) INT << 15) | ((uint32_t) FRAC << 3) | 0x0;
}

uint32_t Thor67::makeR1() const {
    uint32_t r = ((uint32_t) mod << 3);
    if (prescaler) {
        r |= (1UL << 27);
    }
    r |= 0x1;
    return r;
}

uint32_t Thor67::makeR2() const {
    uint32_t r = 0;

    r |= ((uint32_t) rDiv << 14);
    r |= (7UL << 9) | (1UL << 6);

    if (refDoubler) {
        r |= (1UL << 25);
    }
    if (refDiv2) {
        r |= (1UL << 24);
    }
    r |= 0x2;
    return r;
}

uint32_t Thor67::makeR3() const { 
    return 0x3;
}

uint32_t Thor67::makeR4(uint8_t rfDivSel) const {
    uint32_t r = ((uint32_t) rfDivSel << 20);
    if (rfEnable) {
        r |= (1UL << 5);
    }
    r |= ((uint32_t) pwrLevel << 3) | 0x4;
    return r;
}

uint32_t Thor67::makeR5() const {
    return 0x00580005;
}

void Thor67::writeRegister(uint32_t reg){
    digitalWrite(PIN_CS, LOW);
    spiTransfer32(reg);
    digitalWrite(PIN_CS, HIGH);
    delayMicroseconds(10);
    DBGLN("[Thor67] WR 0x%08lX\n", (unsigned long) reg);
}

void Thor67::spiTransfer32(uint32_t v) {
    for (int8_t i = 31; i >= 0; --i) {
        digitalWrite(PIN_MOSI, (v >> i) & 1U);
        delayMicroseconds(1);
        digitalWrite(PIN_CLK, HIGH);
        delayMicroseconds(1);
        digitalWrite(PIN_CLK, LOW);
    }
}

uint8_t Thor67::chooseRfDivider(double rfMHz, double& vcoMHz) {
    uint8_t sel = 0;
    vcoMHz = rfMHz;

    while (vcoMHz < 2200.0 && sel < 6) {
        vcoMHz *= 2;
        sel += 1;
    }
    while (vcoMHz > 4400.0 && sel > 0) {
        vcoMHz /= 2;
        sel -= 1;
    }
    return sel;
}

bool Thor67::calcIntFrac(double vcoMHz, uint16_t&INT, uint16_t&FRAC) {
    if (fpfdHz <= 0) {
        return false;
    }
    double N = (vcoMHz * 1e6) / fpfdHz;
    prescaler = (N >= 75) ? 1:0;
    INT = (uint16_t) floor(N);
    double frac = N - INT;
    FRAC = (uint16_t) round(frac * mod);

    if (FRAC >= mod) {
        FRAC = 0;
        INT += 1;
    }
    return true;
}

void Thor67::SendIndexCmd(uint8_t index) {
    DBG("Setting index ");
    DBGLN("%x", index);
    float rf_mhz = table7G2[index] - diff_mhz;
    float rfDiv = rf_mhz / 2.0f;
    double vcoMHz;
    uint8_t divSel;
    uint16_t INT, FRAC;

    divSel = chooseRfDivider(rfDiv, vcoMHz);

    if (!calcIntFrac(vcoMHz, INT, FRAC)) {
        return;
    }
    DBGLN("[Thor67] Setting frequency: RF=%.3f MHz, VCO=%.3f MHz, INT=%u, FRAC=%u\n",
            rf_mhz, vcoMHz, INT, FRAC);

    writeRegister(makeR4(divSel));
    writeRegister(makeR0(INT, FRAC));
    delay(100);
    writeRegister(makeR4(divSel));
    writeRegister(makeR0(INT, FRAC));
    delay(100);
    writeRegister(makeR4(divSel));
    writeRegister(makeR0(INT, FRAC));
    delay(5);
    DBGLN("[Thor67] Frequency set to %.3f MHz\n", rf_mhz);
}
