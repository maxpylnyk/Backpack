#include "thor67.h"

void Thor67::Init() {
    ThorVRX::Init();
}

void Thor67::SendIndexCmd(uint8_t index) {
    setFreq(table7G2[index]);
}
