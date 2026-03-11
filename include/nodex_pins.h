// ============================================================================
// NodeX — Hardware Pin Definitions
// ============================================================================
#pragma once

// On-board LED (active-LOW on most ESP32 dev boards)
#define PIN_LED_STATUS      2

// Optional external status LED
#define PIN_LED_ACTIVITY    4

// Optional factory-reset button (pull LOW to trigger)
// GPIO26: general-purpose I/O, supports INPUT_PULLUP, not a strapping pin
// (GPIO0 = BOOT button — causes accidental resets during flash)
#define PIN_FACTORY_RESET   26
