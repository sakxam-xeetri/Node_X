// ============================================================================
// NodeX — OTA Update Manager Implementation
// ============================================================================
#include "ota_manager.h"
#include "config_manager.h"
#include "nodex_defaults.h"

#include <ArduinoOTA.h>

OTAManager OTA;

void OTAManager::begin() {
    NodeXConfig& cfg = Config.get();

    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPort(OTA_PORT);
    ArduinoOTA.setPassword(cfg.otaPass.c_str());

    ArduinoOTA.onStart([this]() {
        _updating = true;
        String type = (ArduinoOTA.getCommand() == U_FLASH) ? "firmware" : "filesystem";
        Serial.printf("[OTA] Start updating %s\n", type.c_str());
    });

    ArduinoOTA.onEnd([this]() {
        _updating = false;
        Serial.println("\n[OTA] Update complete — rebooting");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("[OTA] Progress: %u%%\r", (progress * 100) / total);
    });

    ArduinoOTA.onError([this](ota_error_t error) {
        _updating = false;
        Serial.printf("[OTA] Error[%u]: ", error);
        switch (error) {
            case OTA_AUTH_ERROR:    Serial.println("Auth Failed");    break;
            case OTA_BEGIN_ERROR:   Serial.println("Begin Failed");   break;
            case OTA_CONNECT_ERROR: Serial.println("Connect Failed"); break;
            case OTA_RECEIVE_ERROR: Serial.println("Receive Failed"); break;
            case OTA_END_ERROR:     Serial.println("End Failed");     break;
        }
    });

    ArduinoOTA.begin();
    Serial.printf("[OTA] Ready — hostname: %s  port: %d\n", OTA_HOSTNAME, OTA_PORT);
}

void OTAManager::handle() {
    ArduinoOTA.handle();
}

bool OTAManager::isUpdating() {
    return _updating;
}
