#include "fs6075.h"

void FS6075::Init() {
    ModuleBase::Init();

    pinMode(PIN_MOSI, OUTPUT);
    pinMode(PIN_CLK, OUTPUT);
    pinMode(PIN_CS, OUTPUT);

    digitalWrite(PIN_MOSI, HIGH);
    digitalWrite(PIN_CLK, LOW);
    digitalWrite(PIN_CS, HIGH);
}

void FS6075::SendIndexCmd(uint8_t index) {
    if (index >= TABLE_7G2_SIZE) {
        index = 0;
    }
    writeRegister(table7G2[index]);
}

void FS6075::writeRegister(uint16_t reg) {
    digitalWrite(PIN_CLK, LOW);
	//digitalWrite(PIN_MOSI, HIGH);
	IF_SPI_delay();
	digitalWrite(PIN_CS, LOW);
	IF_SPI_delay();
    IF_SPI_SendByte(1);
    IF_SPI_SendByte((uint8_t)(reg>>8));
    IF_SPI_SendByte((uint8_t)reg);
	digitalWrite(PIN_CLK, LOW);
	//digitalWrite(PIN_MOSI, HIGH);

    IF_SPI_delay();
	digitalWrite(PIN_CS, HIGH);
	IF_SPI_delay();
	digitalWrite(PIN_CS, LOW);
	IF_SPI_delay();
}

void FS6075::IF_SPI_delay(void) {
    uint32_t i = 100ul;
	while (i--);
}

void FS6075::IF_SPI_SendByte(uint8_t cData) {
	uint8_t i=8;

	while (i--) {
		if(cData & 0x80) {
			digitalWrite(PIN_MOSI, HIGH);
		} else {
			digitalWrite(PIN_MOSI, LOW);
		}
		cData <<= 1;

		IF_SPI_delay();
		digitalWrite(PIN_CLK, HIGH);
		IF_SPI_delay();
		digitalWrite(PIN_CLK, LOW);
		//IF_SPI_delay();
	}
}
