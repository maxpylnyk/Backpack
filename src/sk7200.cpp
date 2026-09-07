#include "sk7200.h"
/*
void SK7200::Init() {
    ModuleBase::Init();

    pinMode(PIN_CS, OUTPUT);
    pinMode(PIN_CLK, OUTPUT);
    pinMode(PIN_MOSI, OUTPUT);

    digitalWrite(PIN_CS, HIGH);
    digitalWrite(PIN_MOSI, HIGH);
    digitalWrite(PIN_CLK, LOW);
    Spi_Nop(100);
    Init_80B();
}

void SK7200::S_SPI_DATA(uint16_t n) {
    if (n) {
        digitalWrite(PIN_MOSI, HIGH);
    } else {
        digitalWrite(PIN_MOSI, LOW);
    }
}

void SK7200::S_SPI_CLK(uint16_t n) {
    if (n) {
        digitalWrite(PIN_CLK, HIGH);
    } else {
        digitalWrite(PIN_CLK, LOW);
    }
}

void SK7200::S_SPI_LE(uint16_t n) {
    if (n) {
        digitalWrite(PIN_CS, HIGH);
    } else {
        digitalWrite(PIN_CS, LOW);
    }
}

void SK7200::Spi_Nop(uint8_t uT) {
//delayMicroseconds(10*uT);
}

uint32_t SK7200::convert(uint32_t *tab) {
	uint32_t ret = 0;
	uint32_t carry = 0, end_index = MAX_DIGITS - 1, i;

	while (end_index >= 2 && tab[end_index] == 0) {
		end_index--;
	}

	if (tab[1] >= 5) {
		carry = 1;
	}

	for (i = end_index; i > 1; i--) {
		ret = ret * 10ul + tab[i];
	}
	ret += carry;
	return ret;
}

void SK7200::multiply_to_array(int *num, int factor, uint32_t *result) {
	int carry = 0;
	int i = 0;
	for (i = 0; i < MAX_DIGITS; i++) {
		int digit_value = num[i] * factor + carry;
		result[i] = digit_value % 10;
		carry = digit_value / 10;
	}

	if (carry > 0 && MAX_DIGITS > 0) {
		for (i = MAX_DIGITS - 1; i >= 0; i--) {
			if (result[i] == 0) {
				result[i] = carry % 10;
				carry /= 10;
				if (carry == 0)
					break;
			}
		}
	}
}

void SK7200::Init_80B() {
	SPI_WriteReg(0, 0x1840C92C);
	SPI_WriteReg(4, 0xD0202800);
	SPI_WriteReg(5, 0x80900044);
	SPI_WriteReg(6, 0x98BFC6C4);
	SPI_WriteReg(7, 0x000064A9);
	SPI_WriteReg(8, 0xA7FC0002);
	SPI_WriteReg(9, 0x045888E0);
	SPI_WriteReg(10, 0x920904A0);
	SPI_WriteReg(11, 0x84AD0BD8);
	SPI_WriteReg(12, 0x84500601);
	SPI_WriteReg(13, 0x00000000);
	SPI_WriteReg(14, 0x2440000A);
	SPI_WriteReg(15, 0x836300A2);
	SPI_WriteReg(16, 0xCFA1137B);
	SPI_WriteReg(17, 0x1C812001);
	SPI_WriteReg(18, 0xA7D0878C);
	SPI_WriteReg(19, 0x8444084C);
	SPI_WriteReg(20, 0x81200000);

	Spi_Nop(10);
	PllReg3 = 0x50960400; // 5M
	PllReg2 = 0x04B0C601;
	PllReg1 = 0x00000000;
	SPI_WriteReg(3, PllReg3);
	SPI_WriteReg(1, PllReg1);
	SPI_WriteReg(2, PllReg2);
	Spi_Nop(10);
}

void SK7200::setFreq(uint32_t Freq) {
	uint32_t regmask, freqDIV = 0, REG9 = 0;
	double fFreq = 0, Fphase = 5, Ffeedbk = 0, fRes = 0;
	uint32_t B_VCOCAL_N2 = 0, N_int = 0, FRAC = 0, dignum = 0, multi = 1;

    Freq-=480;

	if (Freq < 2000) {
		freqDIV = 0x0D00000;
		multi = 4;
	} else if ((Freq >= 2000) && (Freq < 4000)) {
		freqDIV = 0x0900000;
		multi = 2;
	} else if ((Freq >= 4000) && (Freq <= 8000)) {
		freqDIV = 0x0500000;
		multi = 1;
	}
	Freq = Freq * multi;

	dignum = Freq % 10;

	if ((dignum == 0) || (dignum == 5)) {
		Fphase = 5;
		PllReg3 = 0x50960400; // 5M
		PllReg2 = 0x04B0C601;
		PllReg1 = 0x00000000;

		SPI_WriteReg(4, 0xD0202800);
		SPI_WriteReg(5, 0x80900044);
		SPI_WriteReg(7, 0x000064A9);
		SPI_WriteReg(8, 0xA7FC0002);
		REG9 = 0x040888E0 | freqDIV;
		SPI_WriteReg(9, REG9);
		SPI_WriteReg(12, 0x84500601);
		SPI_WriteReg(15, 0x836300A2);
		SPI_WriteReg(16, 0xCFA1137B);
	} else {
		Fphase = 100;
		PllReg3 = 0xA075BC00;
		PllReg2 = 0x002FC601;

		SPI_WriteReg(4, 0xD1000800);
		SPI_WriteReg(5, 0x8088E872);
		SPI_WriteReg(7, 0x410064AD);
		SPI_WriteReg(8, 0xA7FC0003);
		REG9 = 0x0408C8E0 | freqDIV;
		SPI_WriteReg(9, 0x0458C8E0);
		SPI_WriteReg(12, 0x84500600);
		SPI_WriteReg(15, 0xA76300A2);
		SPI_WriteReg(16, 0xCFA0337B);
	}

	fFreq = Freq;
	fFreq = fFreq / 50;
	fFreq = fFreq * 40;
	B_VCOCAL_N2 = (int)fFreq;

	// REG3
	regmask = ~((1UL << 11) - 1);
	regmask &= ((1UL << 23) - 1) | (1UL << 23);
	PllReg3 &= ~regmask;
	PllReg3 |= (B_VCOCAL_N2 << 11) & regmask;
	SPI_WriteReg(3, PllReg3);

	fFreq = Freq;
	Ffeedbk = fFreq / Fphase;
	N_int = (unsigned long)(Ffeedbk);

	if (Fphase == 100) {
		int num[MAX_DIGITS] = {8, 4, 6, 3, 8, 4, 7, 4, 1, 2};
		uint32_t result[MAX_DIGITS] = {0};
		uint32_t s_ret = 0;
		int by_mul = 0;
		fRes = Ffeedbk - N_int;
		fRes = fRes * 2;
		by_mul = (int)(fRes * 100);

		multiply_to_array(num, by_mul, result);
		s_ret = convert(result);

		SPI_WriteReg(1, s_ret); // 2,147,483,648   4294967296 *
	} else {
		SPI_WriteReg(1, 0);
	}
	// REG2
	regmask = ~((1UL << 16) - 1);
	regmask &= ((1UL << 31) - 1) | (1UL << 31);
	PllReg2 &= ~regmask;
	PllReg2 |= (N_int << 16) & regmask;
	SPI_WriteReg(2, PllReg2);
}

uint32_t SK7200::SPI_WriteReg(uint8_t Addr, uint32_t value) {
	int i = 0;
	uint32_t ret = 0;

	S_SPI_LE(0);
	Spi_Nop(1);
	S_SPI_CLK(0);
	Spi_Nop(1);

	Addr = Addr & 0x7F;
	S_SPI_DATA(0);
	for (i = 0; i < 8; i++) {
		if (Addr & 0x80) {
			S_SPI_DATA(1);
		} else {
			S_SPI_DATA(0);
		}
		Spi_Nop(1);

		S_SPI_CLK(1);
		Spi_Nop(1);
		S_SPI_CLK(0);
		Spi_Nop(1);
		Addr <<= 1;
	}

	for (i = 0; i < 32; i++) {
		if (value & 0x80000000) {
			S_SPI_DATA(1);
		} else {
			S_SPI_DATA(0);
		}
		Spi_Nop(1);

		S_SPI_CLK(1);
		Spi_Nop(1);
		S_SPI_CLK(0);
		Spi_Nop(1);
		value <<= 1;
	}
	S_SPI_LE(1);
	Spi_Nop(2);
	return ret;
}

void SK7200::SendIndexCmd(uint8_t index) {
    if (index >= TABLE_7G2_SIZE) {
        index = 0;
    }
    setFreq(table7G2[index]);
}

uint32_t SK7200::SPI_ReadReg(uint8_t Addr) {
	int i = 0;
	uint32_t ret = 0;

	S_SPI_LE(0);
	Spi_Nop(2);
	S_SPI_CLK(0);
	Spi_Nop(2);
	Addr = Addr | 0x80;
	S_SPI_DATA(1);
	for (i = 0; i < 8; i++) {
		if (Addr & 0x80) {
			S_SPI_DATA(1);
		} else {
			S_SPI_DATA(0);
		}
		Spi_Nop(1);

		S_SPI_CLK(1);
		Spi_Nop(2);
		S_SPI_CLK(0);
		Spi_Nop(1);
		Addr <<= 1;
	}

	for (i = 0; i < 32; i++) {
		ret <<= 1;
		ret |= digitalRead(PIN_MOSI);
		Spi_Nop(1);
		S_SPI_CLK(1);
		Spi_Nop(2);
		S_SPI_CLK(0);
		Spi_Nop(1);
	}
	Spi_Nop(1);
	S_SPI_LE(1);
	Spi_Nop(2);
	return ret;
}

void SK7200::write_reg_bit(uint32_t addr, uint32_t endBit, uint32_t startBit, uint32_t value) {
	uint32_t uiRegValue = 0;
	uint32_t regmask = 0;

	uiRegValue = SPI_ReadReg(addr);
	regmask = ~((1UL << startBit) - 1);
	regmask &= ((1UL << endBit) - 1) | (1UL << endBit);
	uiRegValue &= ~regmask;
	uiRegValue |= (value << startBit) & regmask;
	SPI_WriteReg(addr, uiRegValue);
}

uint32_t SK7200::read_reg_bit(uint32_t addr, uint32_t endbit, uint32_t startbit) {
	uint32_t regdata = 0;
	uint32_t regmask = 0;

	regdata = SPI_ReadReg(addr);

	regmask = ~((1UL << startbit) - 1);
	regmask &= ((1UL << endbit) - 1) | (1UL << endbit);
	regdata &= regmask;
	regdata >>= startbit;
	return regdata;
}
*/

void SK7200::Init() {
    ModuleBase::Init();
	pinMode(PIN_6G, INPUT_PULLUP);
	pinMode(PIN_8G, INPUT_PULLUP);
    pinMode(PIN_MOSI, OUTPUT);
    pinMode(PIN_CLK, OUTPUT);
    pinMode(PIN_CS, OUTPUT);

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
}

void SK7200::writeWord(uint64_t word) {
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

void SK7200::writeChnl(uint8_t index) {
    uint16_t f = table7G2[index];
	
    if (digitalRead(PIN_6G) == LOW) {
        f = table6G[index];
    }
    if (digitalRead(PIN_8G) == LOW) {
        f = table8G[index];
    }
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

void SK7200::SendIndexCmd(uint8_t index) {
	size = TABLE_7G2_SIZE;
	
    if (digitalRead(PIN_6G) == LOW) {
        size = TABLE_6G_SIZE;
    }
	if (digitalRead(PIN_8G) == LOW) {
        size = TABLE_8G_SIZE;
    }
    if (index >= size) {
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

    prevChnl = index;
}


