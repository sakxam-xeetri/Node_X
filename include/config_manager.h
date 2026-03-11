// ============================================================================
// NodeX — Configuration Persistence (NVS / Preferences)
// ============================================================================
#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include <vector>
#include <freertos/semphr.h>
#include "nodex_defaults.h"

struct SavedNetwork {
    String ssid;
    String pass;
};

struct NodeXConfig {
    // Station
    String staSSID;
    String staPass;
    // Access Point
    String apSSID;
    String apPass;
    String adminCode;
    uint8_t apChannel;
    uint8_t apMaxClients;
    bool    apHidden;
    // Web auth
    String webUser;
    String webPass;
    // OTA
    String otaPass;
    // Saved networks (up to MAX_SAVED_NETWORKS entries)
    std::vector<SavedNetwork> savedNetworks;
};

class ConfigManager {
public:
    void begin();
    void load();
    void save();
    void factoryReset();

    // Thread-safe lock for dual-core access to config fields
    void lock()   { if (_mutex) xSemaphoreTakeRecursive(_mutex, portMAX_DELAY); }
    void unlock() { if (_mutex) xSemaphoreGiveRecursive(_mutex); }

    NodeXConfig& get();

private:
    Preferences _prefs;
    NodeXConfig _cfg;
    SemaphoreHandle_t _mutex = nullptr;
    static const char* NVS_NAMESPACE;
};

extern ConfigManager Config;
