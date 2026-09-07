#pragma once

#include "module_base.h"
#include <Arduino.h>

class FS6075 : public ModuleBase {
public:
	void Init();
    void SendIndexCmd(uint8_t index);
private:
	void writeRegister(uint16_t reg);
	void IF_SPI_delay();
	void IF_SPI_SendByte(uint8_t cData);
};
