// ============================================================================
//  _   _           _      __  __
// | \ | | ___   __| | ___\ \/ /
// |  \| |/ _ \ / _` |/ _ \\  /
// | |\  | (_) | (_| |  __//  \     ESP32 Wi-Fi Repeater
// |_| \_|\___/ \__,_|\___/_/\_\    v1.0.0
//
// ============================================================================
// NodeX — Main Firmware Entry Point
//
// Architecture:
//   Core 1 — WiFi health monitor, LED status, OTA handling
//   Core 0 — Async web server (runs on Arduino event core)
//
// FreeRTOS Tasks:
//   taskHealth  — monitors STA link, reconnects, watchdog
//   taskLED     — non-blocking status LED patterns
//   taskOTA     — handles OTA update polling
// ============================================================================

#include <Arduino.h>
#include <WiFi.h>
#include "config_manager.h"
#include "wifi_manager.h"
#include "web_dashboard.h"
#include "captive_portal.h"
#include "ota_manager.h"
#include "status_led.h"
#include "nodex_pins.h"
#include "nodex_defaults.h"

// ---- Forward declarations for FreeRTOS tasks ----
void taskHealth(void* param);
void taskLED(void* param);
void taskOTA(void* param);
void taskDNS(void* param);

// ---- Factory reset button check ----
static void checkFactoryResetButton() {
    pinMode(PIN_FACTORY_RESET, INPUT_PULLUP);
    // Hold button LOW for 3 seconds during boot → factory reset
    if (digitalRead(PIN_FACTORY_RESET) == LOW) {
        Serial.println("[Boot] Factory reset button held...");
        delay(3000);
        if (digitalRead(PIN_FACTORY_RESET) == LOW) {
            Serial.println("[Boot] FACTORY RESET triggered!");
            Config.factoryReset();
            ESP.restart();
        }
    }
}

// ============================================================================
// setup() — runs once on Core 1
// ============================================================================
void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println();
    Serial.println("========================================");
    Serial.println("  NodeX — ESP32 Wi-Fi Repeater v1.0.0");
    Serial.println("========================================");

    // 1. Status LED
    Led.begin();
    Led.setPattern(StatusLED::BLINK_SLOW);

    // 2. Configuration from NVS
    Config.begin();
    Serial.println("[Boot] Config loaded from NVS");

    // 3. Factory reset button
    checkFactoryResetButton();

    // 4. Wi-Fi: start AP + STA dual mode
    WifiMgr.begin();
    WifiMgr.startAP();

    // 5. Captive portal DNS (must start after AP is up)
    Portal.begin(WiFi.softAPIP(), IPAddress(8, 8, 8, 8));
    Portal.setSessionTimeoutMs((unsigned long)Config.get().sessionTimeoutMin * 60000UL);

    // 6. Connect to upstream Wi-Fi
    if (WifiMgr.connectSTA()) {
        // 7. Enable NAT forwarding
        WifiMgr.enableNAT();
        Led.setPattern(StatusLED::BREATHE);
    } else {
        Serial.println("[Boot] STA not connected — AP-only mode");
        Led.setPattern(StatusLED::BLINK_FAST);
    }

    // 8. OTA
    OTA.begin();

    // 9. Web dashboard (ESPAsyncWebServer manages its own task on Core 0)
    Dashboard.begin();

    // 10. Spawn FreeRTOS tasks
    xTaskCreatePinnedToCore(taskHealth, "health",  TASK_STACK_HEALTH, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(taskLED,    "led",     TASK_STACK_LED,    NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(taskOTA,    "ota",     TASK_STACK_WEB,    NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore(taskDNS,    "dns",     TASK_STACK_DNS,    NULL, 2, NULL, 0);

    Serial.println("[Boot] All systems go ✓");
    Serial.printf("[Boot] Free heap: %u bytes\n", ESP.getFreeHeap());
}

// ============================================================================
// loop() — intentionally empty; work is done in FreeRTOS tasks
// ============================================================================
void loop() {
    vTaskDelay(portMAX_DELAY);  // yield forever — tasks handle everything
}

// ============================================================================
// FreeRTOS Task: Health Monitor
// - Monitors STA connection, auto-reconnects
// - Re-enables NAT after reconnection
// ============================================================================
void taskHealth(void* param) {
    TickType_t lastCheck = 0;
    // Start lastReconnect at "now" — setup() already tried once, apply full cooldown
    TickType_t lastReconnect = xTaskGetTickCount();
    uint8_t lastClientCount = WifiMgr.getAPClientCount();
    bool lastSTAState = WifiMgr.isSTAConnected();

    for (;;) {
        TickType_t now = xTaskGetTickCount();

        // Track client count and STA state
        uint8_t clientCount = WifiMgr.getAPClientCount();
        bool staState = WifiMgr.isSTAConnected();

        if (clientCount > lastClientCount || (staState && !lastSTAState)) {
            Serial.printf("[Health] Connection event (clients: %d, STA: %s)\n",
                          clientCount, staState ? "UP" : "DOWN");
        }

        // LED: continuously blink while any client is connected; otherwise
        // breathe (STA up) or fast-blink (STA down / error).
        // Don't override BLINK_NOTIFY mid-sequence or BLINK_SLOW mid-reconnect.
        {
            StatusLED::Pattern cur = Led.getPattern();
            if (clientCount > 0) {
                if (cur != StatusLED::BLINK_SLOW && cur != StatusLED::BLINK_NOTIFY) {
                    Led.setPattern(StatusLED::BLINK_SLOW);
                }
            } else {
                if (cur == StatusLED::BLINK_SLOW) {
                    // No clients — restore based on STA state
                    Led.setPattern(staState ? StatusLED::BREATHE : StatusLED::BLINK_FAST);
                }
            }
        }

        lastClientCount = clientCount;
        lastSTAState = staState;

        if ((now - lastCheck) >= pdMS_TO_TICKS(HEALTH_CHECK_INTERVAL_MS)) {
            lastCheck = now;

            Config.lock();
            bool hasSSID = !Config.get().staSSID.isEmpty();
            Config.unlock();

            if (!WifiMgr.isSTAConnected() && hasSSID &&
                (now - lastReconnect) >= pdMS_TO_TICKS(STA_RECONNECT_INTERVAL_MS)) {
                lastReconnect = now;
                Serial.println("[Health] STA disconnected — attempting reconnect...");
                if (clientCount == 0) Led.setPattern(StatusLED::BLINK_SLOW);

                if (WifiMgr.connectSTA()) {
                    WifiMgr.enableNAT();
                    if (clientCount == 0) Led.setPattern(StatusLED::BREATHE);
                    Serial.println("[Health] Reconnected successfully");
                } else {
                    if (clientCount == 0) Led.setPattern(StatusLED::BLINK_FAST);
                    Serial.println("[Health] Reconnect failed — will retry");
                }
            }

            // Log periodic health info
            Serial.printf("[Health] Heap: %u B | STA: %s (%d dBm) | AP clients: %d\n",
                          ESP.getFreeHeap(),
                          WifiMgr.isSTAConnected() ? "UP" : "DOWN",
                          WifiMgr.getSTARSSI(),
                          WifiMgr.getAPClientCount());
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// ============================================================================
// FreeRTOS Task: LED Status Indicator
// ============================================================================
void taskLED(void* param) {
    for (;;) {
        Led.update();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

// ============================================================================
// FreeRTOS Task: OTA Update Handler
// ============================================================================
void taskOTA(void* param) {
    for (;;) {
        OTA.handle();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// ============================================================================
// FreeRTOS Task: Captive Portal DNS Server
// ============================================================================
void taskDNS(void* param) {
    Portal.dnsLoop();  // blocks until Portal.stop()
}
