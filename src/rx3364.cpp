#include "rx3364.h"
#include <SPI.h>
#include "logging.h"

void
RX3364::Init() {
    ModuleBase::Init();

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

    DBGLN("RX3364 init complete");
}

void RX3364::SendIndexCmd(uint8_t index) {
    DBG("Setting index ");
    DBGLN("%x", index);

    if (index >= TABLE_3G3_SIZE) {
        index = 0;
    }
    uint8_t oscMultiplier;
    uint16_t f = table3G3[index];

    if (f >= 3175) {
      oscMultiplier = 0x20;
    } else if (f <= 2860) {
      oscMultiplier = 0x10;
    } else {
      oscMultiplier = 0x08;
    }

    uint32_t fLo = (f - ifMHz) * prescaler / oscFreqMHz * oscMultiplier;
    uint16_t divA = fLo / prescaler;
    uint16_t divB = fLo % prescaler;
    uint32_t r3Value = (f <= 2860) ? 0x60C00800 : 0x61C00800;
    uint32_t registers[6] = {
      (0x29000000u | divB),                                 // R1
      0x42010000,                                           // R2
      r3Value,                                              // R3
      (0x8A000001 | (oscMultiplier << 4)) << addressShift,  // R4
      (0xA000000A << addressShift),                         // R5
      ((0x0D800000u | divA) << addressShift)                // R0
    };

    for (int i = 0; i < 6; i++) {
      writeRegister(registers[i]);
      delayMicroseconds(10);
    }
}

void RX3364::writeRegister(uint32_t reg) {
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
