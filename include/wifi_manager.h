// ============================================================================
// NodeX — Wi-Fi Manager (STA + AP dual-mode)
// ============================================================================
#pragma once

#include <Arduino.h>
#include <vector>
#include <WiFi.h>
#include <esp_wifi.h>

class WiFiManager {
public:
    void begin();
    bool connectSTA();
    void startAP();
    void enableNAT();
    void disableWiFiSleep();

    bool     isSTAConnected();
    int8_t   getSTARSSI();
    uint8_t  getAPClientCount();
    String   getSTAIP();
    String   getAPIP();
    String   getSTAMAC();
    String   getAPMAC();
    String   getUpstreamGateway();

    // Client info structure
    struct ClientInfo {
        String mac;
        String ip;
    };
    std::vector<ClientInfo> getConnectedClients();
    bool kickClientByMAC(const String& mac);

private:
    volatile bool _staConnected = false;  // volatile: read/written from two cores
    bool _natEnabled = false;             // track NAPT state to flush stale table on reconnect
};

extern WiFiManager WifiMgr;
