// ============================================================================
// NodeX — Web Dashboard Implementation
// ============================================================================
#include "web_dashboard.h"
#include "web_ui.h"
#include "config_manager.h"
#include "wifi_manager.h"
#include "captive_portal.h"
#include "nodex_defaults.h"

#include <ArduinoJson.h>
#include <WiFi.h>

WebDashboard Dashboard;

// Timer callback — runs in esp_timer task, safe for ESP.restart()
void WebDashboard::_restartTimerCb(void*) {
    ESP.restart();
}

// ---- Admin session helpers ----

String WebDashboard::_createAdminSession() {
    _cleanExpiredSessions();
    char token[33];
    for (int i = 0; i < 32; i++) token[i] = "0123456789abcdef"[esp_random() & 0xF];
    token[32] = 0;

    xSemaphoreTake(_adminMutex, portMAX_DELAY);
    if (_adminSessions.size() >= MAX_ADMIN_SESSIONS) _adminSessions.erase(_adminSessions.begin());
    _adminSessions.push_back({String(token), millis()});
    xSemaphoreGive(_adminMutex);
    return String(token);
}

void WebDashboard::_cleanExpiredSessions() {
    xSemaphoreTake(_adminMutex, portMAX_DELAY);
    unsigned long now = millis();
    _adminSessions.erase(
        std::remove_if(_adminSessions.begin(), _adminSessions.end(),
            [now](const AdminSession& s) { return (now - s.created) >= ADMIN_SESSION_TIMEOUT_MS; }),
        _adminSessions.end());
    xSemaphoreGive(_adminMutex);
}

bool WebDashboard::_isAdmin(AsyncWebServerRequest* req) {
    // 1. Check cookie
    if (req->hasHeader("Cookie")) {
        String cookies = req->header("Cookie");
        int idx = cookies.indexOf("nodex_admin=");
        if (idx >= 0) {
            String token = cookies.substring(idx + 12);
            int semi = token.indexOf(';');
            if (semi > 0) token = token.substring(0, semi);
            token.trim();

            xSemaphoreTake(_adminMutex, portMAX_DELAY);
            for (auto& s : _adminSessions) {
                if (s.token == token && (millis() - s.created) < ADMIN_SESSION_TIMEOUT_MS) {
                    xSemaphoreGive(_adminMutex);
                    return true;
                }
            }
            xSemaphoreGive(_adminMutex);
        }
    }
    // 2. Fallback: HTTP Basic Auth (for API clients / curl)
    Config.lock();
    String user = Config.get().webUser;
    String pass = Config.get().webPass;
    Config.unlock();
    return req->authenticate(user.c_str(), pass.c_str());
}

bool WebDashboard::_requireAdmin(AsyncWebServerRequest* req) {
    if (_isAdmin(req)) return true;
    req->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
    return false;
}

// ---- Lifecycle ----

void WebDashboard::begin() {
    _adminMutex = xSemaphoreCreateMutex();

    esp_timer_create_args_t timerArgs = {};
    timerArgs.callback = _restartTimerCb;
    timerArgs.name = "nodex_restart";
    esp_timer_create(&timerArgs, &_restartTimer);

    _setupRoutes();
    _server.addHandler(&_events);
    _server.begin();
    Serial.printf("[Web] Dashboard live at http://%s:%d/admin\n",
                  WifiMgr.getAPIP().c_str(), WEB_PORT);
}

// ---- Route table ----

void WebDashboard::_setupRoutes() {
    // OS captive portal detection probes
    // generate_204 / gen_204: Android (GET + POST) and Chrome use these
    _server.on("/generate_204",              HTTP_ANY, [this](AsyncWebServerRequest* req) { _handleCaptiveDetect(req); });
    _server.on("/gen_204",                   HTTP_ANY, [this](AsyncWebServerRequest* req) { _handleCaptiveDetect(req); });
    // Apple / macOS
    _server.on("/hotspot-detect.html",       HTTP_GET, [this](AsyncWebServerRequest* req) { _handleCaptiveDetect(req); });
    _server.on("/library/test/success.html", HTTP_GET, [this](AsyncWebServerRequest* req) { _handleCaptiveDetect(req); });
    // Windows NCSI
    _server.on("/connecttest.txt",           HTTP_GET, [this](AsyncWebServerRequest* req) { _handleCaptiveDetect(req); });
    _server.on("/ncsi.txt",                  HTTP_GET, [this](AsyncWebServerRequest* req) { _handleCaptiveDetect(req); });
    // Windows / generic
    _server.on("/redirect",                  HTTP_ANY, [this](AsyncWebServerRequest* req) { _handleCaptiveDetect(req); });
    _server.on("/success.txt",               HTTP_GET, [this](AsyncWebServerRequest* req) { _handleCaptiveDetect(req); });
    // Additional Android / Chrome probes
    _server.on("/api/generate_204",          HTTP_ANY, [this](AsyncWebServerRequest* req) { _handleCaptiveDetect(req); });
    _server.on("/chat.png",                  HTTP_GET, [this](AsyncWebServerRequest* req) { _handleCaptiveDetect(req); });
    _server.on("/mobile/status.php",         HTTP_GET, [this](AsyncWebServerRequest* req) { _handleCaptiveDetect(req); });

    // Captive portal routes
    _server.on("/", HTTP_GET, [this](AsyncWebServerRequest* req) {
        _handlePortalPage(req);
    });
    _server.on("/portal/login", HTTP_POST, [this](AsyncWebServerRequest* req) {
        _handleLogin(req);
    });
    _server.on("/portal/logout", HTTP_POST, [](AsyncWebServerRequest* req) {
        AsyncWebServerResponse* resp = req->beginResponse(302);
        resp->addHeader("Location", "/");
        resp->addHeader("Set-Cookie", "nodex_admin=; Path=/; HttpOnly; Max-Age=0");
        req->send(resp);
    });
    _server.on("/connected", HTTP_GET, [this](AsyncWebServerRequest* req) {
        _handleConnected(req);
    });

    // Admin dashboard
    _server.on("/admin", HTTP_GET, [this](AsyncWebServerRequest* req) {
        _handleAdminDashboard(req);
    });

    // API endpoints (all require admin)
    _server.on("/api/status", HTTP_GET, [this](AsyncWebServerRequest* req) {
        _handleStatus(req);
    });
    _server.on("/api/clients", HTTP_GET, [this](AsyncWebServerRequest* req) {
        _handleClients(req);
    });
    _server.on("/api/scan", HTTP_GET, [this](AsyncWebServerRequest* req) {
        _handleScan(req);
    });
    _server.on("/api/config", HTTP_GET, [this](AsyncWebServerRequest* req) {
        _handleConfig(req);
    });
    _server.on("/api/config", HTTP_POST,
        [](AsyncWebServerRequest* req) {},
        nullptr,
        [this](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t index, size_t total) {
            _handleSaveConfig(req, data, len, index, total);
        }
    );
    _server.on("/api/reboot", HTTP_POST, [this](AsyncWebServerRequest* req) {
        _handleReboot(req);
    });
    _server.on("/api/factory-reset", HTTP_POST, [this](AsyncWebServerRequest* req) {
        _handleFactoryReset(req);
    });
    // Session & client control
    _server.on("/api/sessions", HTTP_GET, [this](AsyncWebServerRequest* req) {
        _handleSessions(req);
    });
    _server.on("/api/sessions/deauth", HTTP_POST, [this](AsyncWebServerRequest* req) {
        _handleDeauthSession(req);
    });
    _server.on("/api/clients/kick", HTTP_POST, [this](AsyncWebServerRequest* req) {
        _handleKickClient(req);
    });
    // Saved networks
    _server.on("/api/networks", HTTP_GET, [this](AsyncWebServerRequest* req) {
        _handleNetworks(req);
    });
    _server.on("/api/networks/add", HTTP_POST,
        [](AsyncWebServerRequest* req) {},
        nullptr,
        [this](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t index, size_t total) {
            _handleAddNetwork(req, data, len, index, total);
        }
    );
    _server.on("/api/networks/remove", HTTP_POST, [this](AsyncWebServerRequest* req) {
        _handleRemoveNetwork(req);
    });
    _server.on("/api/networks/connect", HTTP_POST, [this](AsyncWebServerRequest* req) {
        _handleConnectSaved(req);
    });

    // Catch-all: redirect unauthenticated to portal (any path / method)
    _server.onNotFound([this](AsyncWebServerRequest* req) {
        if (!Portal.isAuthenticated(req->client()->remoteIP()) && !_isAdmin(req)) {
            String portalUrl = "http://" + WiFi.softAPIP().toString() + "/";
            AsyncWebServerResponse* rsp = req->beginResponse(302);
            rsp->addHeader("Location", portalUrl);
            rsp->addHeader("X-Android-Captive-Portal-Login-URL", portalUrl);
            rsp->addHeader("Cache-Control", "no-cache, no-store");
            req->send(rsp);
        } else {
            req->send(404, "text/plain", "Not found");
        }
    });
}

// ---- Captive portal handlers ----

void WebDashboard::_handlePortalPage(AsyncWebServerRequest* req) {
    if (_isAdmin(req)) { req->redirect("/admin"); return; }
    AsyncWebServerResponse* resp = req->beginResponse(200, "text/html", WEB_PORTAL_HTML);
    resp->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    req->send(resp);
}

void WebDashboard::_handleLogin(AsyncWebServerRequest* req) {
    if (!req->hasParam("code", true)) {
        req->send(400, "application/json", "{\"error\":\"missing\"}");
        return;
    }
    String code = req->getParam("code", true)->value();

    Config.lock();
    String apPass = Config.get().apPass;
    String adminCode = Config.get().adminCode;
    Config.unlock();

    if (code == adminCode) {
        String token = _createAdminSession();
        Portal.authenticateClient(req->client()->remoteIP());
        AsyncWebServerResponse* resp = req->beginResponse(200, "application/json", "{\"status\":\"admin\"}");
        resp->addHeader("Set-Cookie", "nodex_admin=" + token + "; Path=/; HttpOnly; SameSite=Strict");
        req->send(resp);
    } else if (code == apPass) {
        Portal.authenticateClient(req->client()->remoteIP());
        req->send(200, "application/json", "{\"status\":\"connected\"}");
    } else {
        req->send(401, "application/json", "{\"error\":\"denied\"}");
    }
}

void WebDashboard::_handleConnected(AsyncWebServerRequest* req) {
    req->send(200, "text/html", WEB_CONNECTED_HTML);
}

void WebDashboard::_handleCaptiveDetect(AsyncWebServerRequest* req) {
    IPAddress clientIP = req->client()->remoteIP();
    String url = req->url();

    if (Portal.isAuthenticated(clientIP) || _isAdmin(req)) {
        // Respond correctly so OS clears captive portal indicator
        if (url == "/generate_204" || url == "/gen_204") {
            req->send(204);
        } else if (url.indexOf("hotspot-detect") >= 0 || url.indexOf("success") >= 0) {
            req->send(200, "text/html",
                "<HTML><HEAD><TITLE>Success</TITLE></HEAD><BODY>Success</BODY></HTML>");
        } else if (url == "/connecttest.txt") {
            req->send(200, "text/plain", "Microsoft Connect Test");
        } else if (url == "/ncsi.txt") {
            req->send(200, "text/plain", "Microsoft NCSI");
        } else {
            req->send(204);
        }
    } else {
        // Not authenticated — redirect to captive portal.
        // X-Android-Captive-Portal-Login-URL is required by Android 9+ to
        // know which URL to open in the CaptivePortalLogin activity.
        String portalUrl = "http://" + WiFi.softAPIP().toString() + "/";
        AsyncWebServerResponse* rsp = req->beginResponse(302);
        rsp->addHeader("Location", portalUrl);
        rsp->addHeader("X-Android-Captive-Portal-Login-URL", portalUrl);
        rsp->addHeader("Cache-Control", "no-cache, no-store");
        req->send(rsp);
    }
}

void WebDashboard::_handleAdminDashboard(AsyncWebServerRequest* req) {
    if (!_isAdmin(req)) { req->redirect("/"); return; }
    req->send(200, "text/html", WEB_UI_HTML);
}

void WebDashboard::_handleStatus(AsyncWebServerRequest* req) {
    if (!_requireAdmin(req)) return;

    JsonDocument doc;
    doc["uptime"]         = millis() / 1000;
    doc["freeHeap"]       = ESP.getFreeHeap();
    doc["heapSize"]       = ESP.getHeapSize();
    doc["chipModel"]      = ESP.getChipModel();
    doc["cpuFreqMHz"]     = ESP.getCpuFreqMHz();
    doc["flashSizeMB"]    = ESP.getFlashChipSize() / (1024 * 1024);

    // STA info
    doc["staConnected"]   = WifiMgr.isSTAConnected();
    doc["staSSID"]        = Config.get().staSSID;
    doc["staIP"]          = WifiMgr.getSTAIP();
    doc["staMAC"]         = WifiMgr.getSTAMAC();
    doc["staRSSI"]        = WifiMgr.getSTARSSI();
    doc["staGateway"]     = WifiMgr.getUpstreamGateway();

    // AP info
    doc["apSSID"]         = Config.get().apSSID;
    doc["apIP"]           = WifiMgr.getAPIP();
    doc["apMAC"]          = WifiMgr.getAPMAC();
    doc["apClients"]      = WifiMgr.getAPClientCount();
    doc["apMaxClients"]   = Config.get().apMaxClients;
    doc["apChannel"]      = Config.get().apChannel;

    String json;
    serializeJson(doc, json);
    req->send(200, "application/json", json);
}

void WebDashboard::_handleClients(AsyncWebServerRequest* req) {
    if (!_requireAdmin(req)) return;

    auto clients = WifiMgr.getConnectedClients();
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    for (auto& c : clients) {
        JsonObject obj = arr.add<JsonObject>();
        obj["mac"] = c.mac;
        obj["ip"]  = c.ip;
    }

    String json;
    serializeJson(doc, json);
    req->send(200, "application/json", json);
}

void WebDashboard::_handleScan(AsyncWebServerRequest* req) {
    if (!_requireAdmin(req)) return;

    int n = WiFi.scanComplete();
    if (n == WIFI_SCAN_FAILED) {
        WiFi.scanNetworks(true);  // async scan
        req->send(202, "application/json", "{\"status\":\"scanning\"}");
        return;
    }
    if (n == WIFI_SCAN_RUNNING) {
        req->send(202, "application/json", "{\"status\":\"scanning\"}");
        return;
    }

    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();
    for (int i = 0; i < n; i++) {
        JsonObject obj = arr.add<JsonObject>();
        obj["ssid"]    = WiFi.SSID(i);
        obj["rssi"]    = WiFi.RSSI(i);
        obj["channel"] = WiFi.channel(i);
        obj["secure"]  = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
    }
    WiFi.scanDelete();

    String json;
    serializeJson(doc, json);
    req->send(200, "application/json", json);
}

void WebDashboard::_handleConfig(AsyncWebServerRequest* req) {
    if (!_requireAdmin(req)) return;

    Config.lock();
    NodeXConfig& cfg = Config.get();
    JsonDocument doc;
    doc["staSSID"]      = cfg.staSSID;
    doc["apSSID"]       = cfg.apSSID;
    doc["apChannel"]    = cfg.apChannel;
    doc["apMaxClients"] = cfg.apMaxClients;
    doc["apHidden"]     = cfg.apHidden;
    doc["apPass"]       = cfg.apPass;
    doc["adminCode"]    = cfg.adminCode;
    Config.unlock();
    // STA/web/OTA passwords deliberately omitted

    String json;
    serializeJson(doc, json);
    req->send(200, "application/json", json);
}

void WebDashboard::_handleSaveConfig(AsyncWebServerRequest* req,
                                      uint8_t* data, size_t len,
                                      size_t index, size_t total) {
    // Accumulate chunks before processing (JSON may span multiple TCP segments)
    if (index == 0) {
        if (total > 4096) {
            req->send(413, "application/json", "{\"error\":\"Request too large\"}");
            return;
        }
        _configBuffer = "";
        _configBuffer.reserve(total);
    }
    for (size_t i = 0; i < len; i++) _configBuffer += (char)data[i];
    if (index + len < total) return;  // more chunks pending

    if (!_requireAdmin(req)) return;

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, _configBuffer);
    if (err) {
        req->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
    }

    // Validate all fields before applying any change
    String newApPass = "";
    bool setApPass = false;
    if (doc["apPass"].is<String>()) {
        newApPass = doc["apPass"].as<String>();
        if (!newApPass.isEmpty()) setApPass = true;
    }
    String newAdminCode = "";
    bool setAdminCode = false;
    if (doc["adminCode"].is<String>()) {
        newAdminCode = doc["adminCode"].as<String>();
        if (!newAdminCode.isEmpty()) setAdminCode = true;
    }
    String newWebUser = "";
    bool setWebUser = false;
    if (doc["webUser"].is<String>()) {
        newWebUser = doc["webUser"].as<String>();
        setWebUser = true;
        if (newWebUser.isEmpty()) {
            req->send(400, "application/json", "{\"error\":\"Dashboard username cannot be empty\"}");
            return;
        }
    }
    String newWebPass = "";
    bool setWebPass = false;
    if (doc["webPass"].is<String>()) {
        newWebPass = doc["webPass"].as<String>();
        setWebPass = true;
        if (newWebPass.isEmpty()) {
            req->send(400, "application/json", "{\"error\":\"Dashboard password cannot be empty\"}");
            return;
        }
    }

    // Apply all changes under the config mutex (dual-core safety)
    Config.lock();
    NodeXConfig& cfg = Config.get();
    if (doc["staSSID"].is<String>())      cfg.staSSID      = doc["staSSID"].as<String>();
    if (doc["staPass"].is<String>())      cfg.staPass      = doc["staPass"].as<String>();
    if (doc["apSSID"].is<String>())       cfg.apSSID       = doc["apSSID"].as<String>();
    if (setApPass)                        cfg.apPass       = newApPass;
    if (setAdminCode)                     cfg.adminCode    = newAdminCode;
    if (doc["apChannel"].is<int>()) {
        uint8_t ch = doc["apChannel"].as<uint8_t>();
        if (ch >= 1 && ch <= 13) cfg.apChannel = ch;
    }
    if (doc["apMaxClients"].is<int>()) {
        uint8_t mc = doc["apMaxClients"].as<uint8_t>();
        if (mc >= 1 && mc <= 10) cfg.apMaxClients = mc;
    }
    if (doc["apHidden"].is<bool>())       cfg.apHidden     = doc["apHidden"].as<bool>();
    if (setWebUser)                       cfg.webUser      = newWebUser;
    if (setWebPass)                       cfg.webPass      = newWebPass;
    if (doc["otaPass"].is<String>())      cfg.otaPass      = doc["otaPass"].as<String>();
    Config.save();   // internally takes recursive mutex — safe while we hold it
    Config.unlock();

    req->send(200, "application/json", "{\"status\":\"saved\",\"note\":\"Reboot to apply\"}");
}

void WebDashboard::_handleReboot(AsyncWebServerRequest* req) {
    if (!_requireAdmin(req)) return;
    req->send(200, "application/json", "{\"status\":\"rebooting\"}");
    esp_timer_start_once(_restartTimer, 500000);  // 500 ms in µs (non-blocking)
}

void WebDashboard::_handleFactoryReset(AsyncWebServerRequest* req) {
    if (!_requireAdmin(req)) return;
    Config.factoryReset();
    req->send(200, "application/json", "{\"status\":\"factory reset done, rebooting\"}");
    esp_timer_start_once(_restartTimer, 500000);  // 500 ms in µs (non-blocking)
}

void WebDashboard::_handleSessions(AsyncWebServerRequest* req) {
    if (!_requireAdmin(req)) return;
    auto sessions = Portal.getSessions();
    auto clients  = WifiMgr.getConnectedClients();
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();
    for (auto& s : sessions) {
        JsonObject obj = arr.add<JsonObject>();
        String ipStr = s.ip.toString();
        obj["ip"]  = ipStr;
        obj["age"] = (uint32_t)(s.ageMs / 1000);
        for (auto& c : clients) {
            if (c.ip == ipStr) { obj["mac"] = c.mac; break; }
        }
    }
    String json;
    serializeJson(doc, json);
    req->send(200, "application/json", json);
}

void WebDashboard::_handleDeauthSession(AsyncWebServerRequest* req) {
    if (!_requireAdmin(req)) return;
    if (!req->hasParam("ip", true)) {
        req->send(400, "application/json", "{\"error\":\"missing ip\"}");
        return;
    }
    IPAddress ip;
    if (!ip.fromString(req->getParam("ip", true)->value())) {
        req->send(400, "application/json", "{\"error\":\"invalid ip\"}");
        return;
    }
    Portal.deauthClient(ip);
    req->send(200, "application/json", "{\"status\":\"ok\"}");
}

void WebDashboard::_handleKickClient(AsyncWebServerRequest* req) {
    if (!_requireAdmin(req)) return;
    if (!req->hasParam("mac", true)) {
        req->send(400, "application/json", "{\"error\":\"missing mac\"}");
        return;
    }
    String mac = req->getParam("mac", true)->value();
    // Also remove portal session for this device
    auto clients = WifiMgr.getConnectedClients();
    for (auto& c : clients) {
        if (c.mac.equalsIgnoreCase(mac) && c.ip != "pending") {
            IPAddress ip;
            if (ip.fromString(c.ip)) Portal.deauthClient(ip);
            break;
        }
    }
    if (WifiMgr.kickClientByMAC(mac))
        req->send(200, "application/json", "{\"status\":\"kicked\"}");
    else
        req->send(404, "application/json", "{\"error\":\"not found\"}");
}

void WebDashboard::_handleNetworks(AsyncWebServerRequest* req) {
    if (!_requireAdmin(req)) return;
    Config.lock();
    auto nets = Config.get().savedNetworks;
    Config.unlock();
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();
    for (auto& n : nets) {
        JsonObject obj = arr.add<JsonObject>();
        obj["ssid"] = n.ssid;  // passwords never sent to client
    }
    String json;
    serializeJson(doc, json);
    req->send(200, "application/json", json);
}

void WebDashboard::_handleAddNetwork(AsyncWebServerRequest* req,
                                      uint8_t* data, size_t len,
                                      size_t index, size_t total) {
    if (index == 0) {
        if (total > 512) { req->send(413, "application/json", "{\"error\":\"too large\"}"); return; }
        _networkBuffer = "";
        _networkBuffer.reserve(total);
    }
    for (size_t i = 0; i < len; i++) _networkBuffer += (char)data[i];
    if (index + len < total) return;
    if (!_requireAdmin(req)) return;
    JsonDocument doc;
    if (deserializeJson(doc, _networkBuffer) != DeserializationError::Ok) {
        req->send(400, "application/json", "{\"error\":\"invalid JSON\"}");
        return;
    }
    String ssid = doc["ssid"] | "";
    String pass = doc["pass"] | "";
    if (ssid.isEmpty()) {
        req->send(400, "application/json", "{\"error\":\"ssid required\"}");
        return;
    }
    Config.lock();
    auto& nets = Config.get().savedNetworks;
    bool found = false;
    for (auto& n : nets) {
        if (n.ssid == ssid) { n.pass = pass; found = true; break; }
    }
    if (!found) {
        if ((int)nets.size() >= MAX_SAVED_NETWORKS) {
            Config.unlock();
            req->send(400, "application/json", "{\"error\":\"max 5 networks reached\"}");
            return;
        }
        nets.push_back({ssid, pass});
    }
    Config.save();
    Config.unlock();
    req->send(200, "application/json", "{\"status\":\"saved\"}");
}

void WebDashboard::_handleRemoveNetwork(AsyncWebServerRequest* req) {
    if (!_requireAdmin(req)) return;
    if (!req->hasParam("index", true)) {
        req->send(400, "application/json", "{\"error\":\"missing index\"}");
        return;
    }
    int idx = req->getParam("index", true)->value().toInt();
    Config.lock();
    auto& nets = Config.get().savedNetworks;
    if (idx < 0 || idx >= (int)nets.size()) {
        Config.unlock();
        req->send(400, "application/json", "{\"error\":\"invalid index\"}");
        return;
    }
    nets.erase(nets.begin() + idx);
    Config.save();
    Config.unlock();
    req->send(200, "application/json", "{\"status\":\"removed\"}");
}

void WebDashboard::_handleConnectSaved(AsyncWebServerRequest* req) {
    if (!_requireAdmin(req)) return;
    if (!req->hasParam("index", true)) {
        req->send(400, "application/json", "{\"error\":\"missing index\"}");
        return;
    }
    int idx = req->getParam("index", true)->value().toInt();
    Config.lock();
    auto& nets = Config.get().savedNetworks;
    if (idx < 0 || idx >= (int)nets.size()) {
        Config.unlock();
        req->send(400, "application/json", "{\"error\":\"invalid index\"}");
        return;
    }
    Config.get().staSSID = nets[idx].ssid;
    Config.get().staPass = nets[idx].pass;
    Config.save();
    Config.unlock();
    req->send(200, "application/json", "{\"status\":\"set\",\"note\":\"rebooting\"}");
    esp_timer_start_once(_restartTimer, 800000);  // 800 ms
}
