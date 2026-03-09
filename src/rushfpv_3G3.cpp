#include "rushfpv_3G3.h"
#include <SPI.h>
#include "logging.h"

void RUSHFPV_3G3::Init() {
    ModuleBase::Init();
    pinMode(PIN_SDA, OUTPUT);
    pinMode(PIN_SCL, OUTPUT);
    display = new TM1650(PIN_SDA, PIN_SCL, 2);

    pinMode(PIN_MOSI, OUTPUT);
    pinMode(PIN_CLK, OUTPUT);
    pinMode(PIN_CS, OUTPUT);

    digitalWrite(PIN_MOSI, LOW);
    digitalWrite(PIN_CLK, LOW);
    digitalWrite(PIN_CS, LOW);
    delay(100);
    digitalWrite(PIN_MOSI, HIGH);
    digitalWrite(PIN_CLK, LOW);
    digitalWrite(PIN_CS, HIGH);

    DBGLN("RUSHFPV_3G3 init complete");
}

void RUSHFPV_3G3::writeRegister(uint32_t reg) {
    uint32_t periodMicroSec = 1000000 / BIT_BANG_FREQ;
    digitalWrite(PIN_CS, LOW);
    delayMicroseconds(periodMicroSec);

    for (int i = 31; i >= 0; i--) {
        digitalWrite(PIN_MOSI, (reg >> i) & 1);
        delayMicroseconds(1);
        digitalWrite(PIN_CLK, HIGH);
        delayMicroseconds(9);
        digitalWrite(PIN_CLK, LOW);
        delayMicroseconds(5);
    }
    digitalWrite(PIN_MOSI, LOW);
    digitalWrite(PIN_CLK, LOW);
    digitalWrite(PIN_CS, HIGH);
}

void RUSHFPV_3G3::SendIndexCmd(uint8_t index) {
    DBG("Setting index ");
    DBGLN("%x", index);

    if (index > TABLE_3G3_SIZE) {
        index = 0;
    }

    uint8_t oscMultiplier = 0x08;
    uint16_t f = table3G3[index];
    uint32_t fLo = (f - ifMHz) * prescaler / oscFreqMHz * oscMultiplier;
    uint16_t divA = fLo / prescaler;
    uint16_t divB = fLo % prescaler;
    uint32_t registers[6] = {
        (0x29000000u | divB),                   // R1
        0x42010000,                             // R2
        0x6375C800,                             // R3
        (0x8A200001 | (oscMultiplier << 4)),    // R4
        0xA000001A,                             // R5
        (0x05180000u | divA)                    // R0
    };
    for (int i = 0; i < 6; i++) {
        writeRegister(registers[i]);
        delayMicroseconds(10);
    }
    display->printIndex(index);
}
