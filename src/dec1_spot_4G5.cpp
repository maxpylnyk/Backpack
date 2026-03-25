#include "dec1_spot_4G5.h"
#include "logging.h"

void
Dec1Spot4G5::Init()
{
    ModuleBase::Init();
    uart.begin(VRX_UART_BAUD, SERIAL_8N1, RX_PIN, TX_PIN);
    DBGLN("Dec1 Spot 4G5 backpack init complete");
}

uint16_t
Dec1Spot4G5::crc16_ccitt_false(const uint8_t *data, uint8_t length) {
    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;

        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc = crc << 1;
            }
        }
    }
    return crc;
}

void
Dec1Spot4G5::SendIndexCmd(uint8_t index)
{
    DBG("Setting index ");
    DBGLN("%x", index);

    uint16_t f = tablePeak35[index];

    uint8_t len = 9;
    uint8_t buf[len] = {0xAA, 0x55, 0x00, 0x00, 0x01, 0x03, 0x02, 0x00, 0x00};
    buf[7] = f & 0xFF;
    buf[8] = f >> 8;
    uint16_t crc = crc16_ccitt_false(&buf[4], 5);
    buf[2] = crc & 0xFF;
    buf[3] = crc >> 8;

    for (uint8_t i = 0; i < len; i++) {
        uart.write(buf[i]);
    }
}