#pragma once

#include "module_base.h"
#include <Arduino.h>
#include <TM1650.h>

const int8_t initPartCount = 4;
const int8_t initSeqIdx[initPartCount] = {18, 21, 23, 25};
const uint64_t initSeq[25] = {
    0x1840C92C, 0x4D0202800, 0x580900044,
    0x698BFC6C4, 0x7000064A9, 0x8A7FC0002,
    0x9045888E0, 0xA920904A0, 0xB84AD0BD8,
    0xC84500601, 0xD00000000, 0xE2440000A,
    0xF836300A2, 0x10CFA1137B, 0x111C812001,
    0x12A7D0878C, 0x138444084C, 0x1481200000,
    0x350960400, 0x100000000, 0x204B0C601,
    0x89FFFFFFFF, 0x9045888E0,
    0x89FFFFFFFF, 0x9045088E0
};

class RUSHFPV_7G2 : public ModuleBase {
public:
    void Init();
    void SendIndexCmd(uint8_t index);

private:
    TM1650* display;
    bool firstChange = true;
    uint8_t prevChnl = UINT8_MAX;
    const int8_t wordLen = 40;
    const uint16_t ifMHz = 480;
    const uint32_t dly = 23;
    const uint32_t dlyWord = dly * 2;
    const uint32_t dlyInit = 256 - dly;

    void writeWord(uint64_t word);
    void writeChnl(uint8_t index);
};
