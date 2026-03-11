// ============================================================================
// NodeX — Configuration Persistence Implementation
// ============================================================================
#include "config_manager.h"
#include <ArduinoJson.h>

const char* ConfigManager::NVS_NAMESPACE = "nodex";

ConfigManager Config;

void ConfigManager::begin() {
    _mutex = xSemaphoreCreateRecursiveMutex();
    _prefs.begin(NVS_NAMESPACE, false);
    load();
}

void ConfigManager::load() {
    _cfg.staSSID      = _prefs.getString("staSSID",   DEFAULT_STA_SSID);
    _cfg.staPass      = _prefs.getString("staPass",   DEFAULT_STA_PASS);
    _cfg.apSSID       = _prefs.getString("apSSID",    DEFAULT_AP_SSID);
    // One-time migration: rename old default SSID to new name
    if (_cfg.apSSID == "NodeX_Repeater") {
        _cfg.apSSID = DEFAULT_AP_SSID;
        _prefs.putString("apSSID", _cfg.apSSID);
    }
    _cfg.apPass       = _prefs.getString("apPass",    DEFAULT_AP_PASS);    _cfg.adminCode    = _prefs.getString("admCode",  DEFAULT_ADMIN_CODE);    _cfg.apChannel    = _prefs.getUChar ("apChan",    DEFAULT_AP_CHANNEL);
    _cfg.apMaxClients = _prefs.getUChar ("apMaxCli",  DEFAULT_AP_MAX_CLIENTS);
    _cfg.apHidden     = _prefs.getBool  ("apHidden",  DEFAULT_AP_HIDDEN);
    // Safety: if adminCode is empty or collides with apPass, restore the default
    if (_cfg.adminCode.isEmpty() || _cfg.adminCode == _cfg.apPass) {
        _cfg.adminCode = DEFAULT_ADMIN_CODE;
        _prefs.putString("admCode", _cfg.adminCode);
    }
    _cfg.webUser      = _prefs.getString("webUser",   WEB_USERNAME);
    _cfg.webPass      = _prefs.getString("webPass",   WEB_PASSWORD);
    _cfg.otaPass      = _prefs.getString("otaPass",   OTA_PASSWORD);
    _cfg.sessionTimeoutMin = _prefs.getUShort("sessTime", DEFAULT_SESSION_TIMEOUT_MIN);
    // Load saved networks (JSON array in NVS)
    {
        String sn = _prefs.getString("savedNets", "[]");
        JsonDocument jdoc;
        _cfg.savedNetworks.clear();
        if (deserializeJson(jdoc, sn) == DeserializationError::Ok && jdoc.is<JsonArray>()) {
            for (JsonObject obj : jdoc.as<JsonArray>()) {
                if (_cfg.savedNetworks.size() >= MAX_SAVED_NETWORKS) break;
                String s = obj["s"] | "";
                String p = obj["p"] | "";
                if (!s.isEmpty()) _cfg.savedNetworks.push_back({s, p});
            }
        }
    }
}

void ConfigManager::save() {
    lock();
    _prefs.putString("staSSID",   _cfg.staSSID);
    _prefs.putString("staPass",   _cfg.staPass);
    _prefs.putString("apSSID",    _cfg.apSSID);
    _prefs.putString("apPass",    _cfg.apPass);    _prefs.putString("admCode",  _cfg.adminCode);    _prefs.putUChar ("apChan",    _cfg.apChannel);
    _prefs.putUChar ("apMaxCli",  _cfg.apMaxClients);
    _prefs.putBool  ("apHidden",  _cfg.apHidden);
    _prefs.putString("webUser",   _cfg.webUser);
    _prefs.putString("webPass",   _cfg.webPass);
    _prefs.putString("otaPass",   _cfg.otaPass);
    _prefs.putUShort("sessTime", _cfg.sessionTimeoutMin);
    // Save networks array
    {
        JsonDocument jdoc;
        JsonArray arr = jdoc.to<JsonArray>();
        for (auto& n : _cfg.savedNetworks) {
            JsonObject obj = arr.add<JsonObject>();
            obj["s"] = n.ssid;
            obj["p"] = n.pass;
        }
        String sn;
        serializeJson(jdoc, sn);
        _prefs.putString("savedNets", sn);
    }
    unlock();
}

void ConfigManager::factoryReset() {
    lock();
    _prefs.clear();
    load();  // reload defaults
    unlock();
}

NodeXConfig& ConfigManager::get() {
    return _cfg;
}
