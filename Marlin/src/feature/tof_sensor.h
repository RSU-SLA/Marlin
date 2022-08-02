#pragma once

#include "../inc/MarlinConfigPre.h"

#include "Wire.h"

class TofSensor {
private:
    static float range;

public:
    static float curRange;
    static void init();
    static void read();
};

extern TofSensor tofSensor;