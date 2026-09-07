#pragma once

#include "thor_vrx.h"

class Thor78 : public ThorVRX {
public:
    void Init();
    void SendIndexCmd(uint8_t index);
};
