// ============================================================================
// NodeX — Captive Portal DNS & Session Manager
//
// Per-client DNS handling:
//   Unauthenticated → all DNS resolves to AP IP (triggers OS captive portal)
//   Authenticated   → DNS forwarded to upstream via NAPT (normal browsing)
// ============================================================================
#pragma once

#include <Arduino.h>
#include <WiFiUdp.h>
#include <vector>
#include <freertos/semphr.h>
#include "nodex_defaults.h"

class CaptivePortal {
public:
    void begin(IPAddress apIP, IPAddress upstreamDNS);
    void stop();

    bool isAuthenticated(IPAddress clientIP);
    void authenticateClient(IPAddress clientIP);
    void deauthClient(IPAddress clientIP);
    void clearAllSessions();

    // Session info for admin panel
    struct SessionInfo { IPAddress ip; unsigned long ageMs; };
    std::vector<SessionInfo> getSessions();

    // Blocking loop — run from a dedicated FreeRTOS task
    void dnsLoop();

    void setSessionTimeoutMs(unsigned long ms);

private:
    IPAddress _apIP;
    unsigned long _sessionTimeoutMs = SESSION_TIMEOUT_MS;
    IPAddress _upstreamDNS;
    WiFiUDP   _dns;
    volatile bool _running = false;

    struct AuthEntry {
        uint32_t ip;
        unsigned long authTime;
    };
    std::vector<AuthEntry> _clients;
    SemaphoreHandle_t _mutex = nullptr;

    void _sendRedirect(IPAddress client, uint16_t port, uint8_t* buf, int len);
    void _forwardQuery(IPAddress client, uint16_t port, uint8_t* buf, int len);
};

extern CaptivePortal Portal;
