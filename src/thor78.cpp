#include "thor78.h"

void Thor78::Init() {
    ThorVRX::Init();
}

void Thor78::SendIndexCmd(uint8_t index) {
    setFreq(table8G[index]);
}
