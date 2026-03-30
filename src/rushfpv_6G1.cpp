#include "rushfpv_6G1.h"
#include "logging.h"

void RUSHFPV_6G1::Init() {
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
    DBGLN("RUSHFPV_6G1 init complete");
}

void RUSHFPV_6G1::writeWord(uint64_t word) {
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

void RUSHFPV_6G1::writeChnl(uint8_t index) {
    uint16_t f = table5G8[index];
    uint16_t fDif = f - ifMHz;
    double fLo = fDif / 5.0;
    double fLo2 = fDif / 2.5;
    uint64_t word1, word2, word3, val2;
    uint16_t val1 = fLo2;
    uint16_t val3;


    if (f % 5) {
        fLo = fDif % 100;
        val2 = fLo;
        val3 = fDif / 100;

        word1 = 0x03A0000000 | val1 << 12;
        
        if (fLo2 - val1 >= 0.5f) {
            word1 |= 0xC00;
        } else {
            word1 |= 0x400;
        }

        switch(val2) {
            case 2:
            case 9:
            case 13:
            case 39:
            case 49:
            case 52:
            case 53:
            case 56:
            case 59:
            case 67:
            case 78:
            case 82:
            case 84:
            case 89:
            case 99:
                break;
            default:
                fLo -= 0.5;
        }
        val2 = fLo * multMHz;
        val2 -= 2;
        word2 = 0x100000000 | val2;
        word3 = 0x20000C601 | val3 << 16;
        
        for (int8_t i = 0; i < seqLen; i++) {
            writeWord(restSeq[i]);
            delayMicroseconds(dlyWord);
        }
    } else {
        val3 = fLo;
        word1 = 0x0350000400 | val1 << 12;
        word2 = 0x0100000000;
        word3 = 0x020000C601 | val3 << 16;

        for (int8_t i = 0; i < seqLen; i++) {
            writeWord(div5seq[i]);
            delayMicroseconds(dlyWord);
        }
    }
    delayMicroseconds(dlySeq2);
    writeWord(word1);
    delayMicroseconds(dlySeq3);
    writeWord(word2);
    delayMicroseconds(dlyWord);
    writeWord(word3);
}

void RUSHFPV_6G1::SendIndexCmd(uint8_t index) {
    DBG("Setting index ");
    DBGLN("%x", index);

    if (index > TABLE_5G8_SIZE) {
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
    display->print(bands[index / 8], index % 8 + 1);
    prevChnl = index;
}
