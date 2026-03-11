// ============================================================================
// NodeX — Status LED Controller Implementation
// ============================================================================
#include "status_led.h"
#include "nodex_pins.h"

StatusLED Led;

void StatusLED::begin() {
    ledcAttach(PIN_LED_STATUS, 5000, 8);
}

void StatusLED::setPattern(Pattern p) {
    _pattern = p;
}

void StatusLED::solid(bool on) {
    ledcWrite(PIN_LED_STATUS, on ? 255 : 0);
}

void StatusLED::off() {
    _pattern = OFF;
    ledcWrite(PIN_LED_STATUS, 0);
}

void StatusLED::update() {
    unsigned long now = millis();

    switch (_pattern) {
        case OFF:
            ledcWrite(PIN_LED_STATUS, 0);
            break;

        case SOLID_ON:
            ledcWrite(PIN_LED_STATUS, 255);
            break;

        case BLINK_SLOW:
            if (now - _lastToggle >= 1000) {
                _ledState = !_ledState;
                ledcWrite(PIN_LED_STATUS, _ledState ? 255 : 0);
                _lastToggle = now;
            }
            break;

        case BLINK_FAST:
            if (now - _lastToggle >= 200) {
                _ledState = !_ledState;
                ledcWrite(PIN_LED_STATUS, _ledState ? 255 : 0);
                _lastToggle = now;
            }
            break;

        case BREATHE:
            if (now - _lastToggle >= 15) {
                _breatheVal += _breatheDir * 3;
                if (_breatheVal >= 250) { _breatheVal = 250; _breatheDir = -1; }
                if (_breatheVal <= 5)   { _breatheVal = 5;   _breatheDir = 1;  }
                ledcWrite(PIN_LED_STATUS, _breatheVal);
                _lastToggle = now;
            }
            break;
    }
}
