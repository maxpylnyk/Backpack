#include "rushfpv_7G2.h"
#include "logging.h"

void RUSHFPV_7G2::Init() {
    ModuleBase::Init();
    pinMode(PIN_SDA, OUTPUT);
    pinMode(PIN_SCL, OUTPUT);
    pinMode(PIN_MOSI, OUTPUT);
    pinMode(PIN_CLK, OUTPUT);
    pinMode(PIN_CS, OUTPUT);
    display = new TM1650(PIN_SDA, PIN_SCL, 2);

    digitalWrite(PIN_MOSI, HIGH);
    digitalWrite(PIN_CLK, HIGH);
    digitalWrite(PIN_CS, HIGH);
    delay(122);
    digitalWrite(PIN_CLK, LOW);
    digitalWrite(PIN_MOSI, LOW);
    digitalWrite(PIN_CS, LOW);
    delayMicroseconds(dly);
    digitalWrite(PIN_CLK, HIGH);
    digitalWrite(PIN_MOSI, HIGH);
    digitalWrite(PIN_CS, HIGH);
    delay(102);

    for (int8_t i = 0, j = 0; j < initPartCount; j++) {
        for (; i < initSeqIdx[j]; i++) {
            writeWord(initSeq[i]);
            delayMicroseconds(dlyWord);
        }
        delayMicroseconds(dlyInit);
    }
    delay(6);
    DBGLN("RUSHFPV_7G2 init complete");
}

void RUSHFPV_7G2::writeWord(uint64_t word) {
    digitalWrite(PIN_CS, LOW);
    delayMicroseconds(dly);
    digitalWrite(PIN_CLK, LOW);
    delayMicroseconds(dly);

    for (int8_t i = wordLen-1; i >= 0; i--) {
        digitalWrite(PIN_MOSI, (word >> i) & 1);
        delayMicroseconds(dly);
        digitalWrite(PIN_CLK, HIGH);
        delayMicroseconds(dly);
        digitalWrite(PIN_CLK, LOW);
        delayMicroseconds(dly);
    }
    digitalWrite(PIN_CS, HIGH);
}

void RUSHFPV_7G2::writeChnl(uint8_t index) {
    uint16_t f = table7G2[index];
    uint16_t fLo = (f - ifMHz) / 5;
    uint64_t word1 = 0x0350000400 | 2 * fLo << 12;
    uint64_t word2 = 0x0100000000;
    uint64_t word3 = 0x020000C401 | fLo << 16;

    writeWord(word1);
    delayMicroseconds(dlyWord);
    writeWord(word2);
    delayMicroseconds(dlyWord);
    writeWord(word3);
}

void RUSHFPV_7G2::SendIndexCmd(uint8_t index) {
    DBG("Setting index ");
    DBGLN("%x", index);

    if (index > 64u) {
        writeChnl(0u);
        prevChnl = 0u;
        return;
    }
    if (firstChange) {
        writeChnl(prevChnl);
        delay(144);
        firstChange = false;
    }
    writeChnl(index);
    display->printIndex(index);
    prevChnl = index;
}
