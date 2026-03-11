// ============================================================================
// NodeX — OTA (Over-The-Air) Update Manager
// ============================================================================
#pragma once

#include <Arduino.h>

class OTAManager {
public:
    void begin();
    void handle();
    bool isUpdating();
private:
    bool _updating = false;
};

extern OTAManager OTA;
