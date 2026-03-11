// ============================================================================
// NodeX — Web Dashboard Server
// ============================================================================
#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <esp_timer.h>
#include <vector>
#include <freertos/semphr.h>
#include "nodex_defaults.h"

class WebDashboard {
public:
    void begin();

private:
    AsyncWebServer _server{WEB_PORT};
    AsyncEventSource _events{"/events"};
    String _configBuffer;
    esp_timer_handle_t _restartTimer = nullptr;
    static void _restartTimerCb(void*);

    // Admin sessions (cookie-based)
    struct AdminSession {
        String token;
        unsigned long created;
    };
    std::vector<AdminSession> _adminSessions;
    SemaphoreHandle_t _adminMutex = nullptr;

    // Route setup
    void _setupRoutes();

    // Captive portal handlers
    void _handlePortalPage(AsyncWebServerRequest* req);
    void _handleLogin(AsyncWebServerRequest* req);
    void _handleConnected(AsyncWebServerRequest* req);
    void _handleCaptiveDetect(AsyncWebServerRequest* req);

    // Admin dashboard
    void _handleAdminDashboard(AsyncWebServerRequest* req);

    // API handlers (admin only)
    void _handleStatus(AsyncWebServerRequest* req);
    void _handleClients(AsyncWebServerRequest* req);
    void _handleScan(AsyncWebServerRequest* req);
    void _handleConfig(AsyncWebServerRequest* req);
    void _handleSaveConfig(AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t index, size_t total);
    void _handleReboot(AsyncWebServerRequest* req);
    void _handleFactoryReset(AsyncWebServerRequest* req);

    // Session & client control
    void _handleSessions(AsyncWebServerRequest* req);
    void _handleDeauthSession(AsyncWebServerRequest* req);
    void _handleKickClient(AsyncWebServerRequest* req);

    // Saved networks
    void _handleNetworks(AsyncWebServerRequest* req);
    void _handleAddNetwork(AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t index, size_t total);
    void _handleRemoveNetwork(AsyncWebServerRequest* req);
    void _handleConnectSaved(AsyncWebServerRequest* req);
    String _networkBuffer;

    // Auth helpers
    bool _isAdmin(AsyncWebServerRequest* req);
    bool _requireAdmin(AsyncWebServerRequest* req);
    String _createAdminSession();
    void _cleanExpiredSessions();
};

extern WebDashboard Dashboard;
