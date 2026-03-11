// ============================================================================
// NodeX — Status LED Controller
// ============================================================================
#pragma once

#include <Arduino.h>

class StatusLED {
public:
    void begin();

    // LED patterns
    void solid(bool on);
    void blinkSlow();   // STA connecting
    void blinkFast();   // Error / no STA
    void breathe();     // Normal operation
    void off();

    // Call from FreeRTOS task loop
    void update();

    enum Pattern { OFF, SOLID_ON, BLINK_SLOW, BLINK_FAST, BREATHE, BLINK_NOTIFY };
    void setPattern(Pattern p);
    void notify();  // quick triple-blink, then resume previous pattern

private:
    Pattern _pattern = OFF;
    Pattern _prevPattern = OFF;
    unsigned long _lastToggle = 0;
    bool _ledState = false;
    uint8_t _breatheVal = 0;
    int8_t  _breatheDir = 1;
    uint8_t _notifyCount = 0;
};

extern StatusLED Led;
