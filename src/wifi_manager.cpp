// ============================================================================
// NodeX — Wi-Fi Manager Implementation
// ============================================================================
#include "wifi_manager.h"
#include "config_manager.h"
#include "nodex_defaults.h"

#include <esp_wifi.h>
#include "esp_netif.h"
#include "lwip/lwip_napt.h"
#include "lwip/priv/tcpip_priv.h"

WiFiManager WifiMgr;

void WiFiManager::begin() {
    WiFi.mode(WIFI_AP_STA);
    disableWiFiSleep();
}

bool WiFiManager::connectSTA() {
    NodeXConfig& cfg = Config.get();

    if (cfg.staSSID.isEmpty()) {
        Serial.println("[WiFi] No STA SSID configured");
        _staConnected = false;
        return false;
    }

    Serial.printf("[WiFi] Connecting to: %s\n", cfg.staSSID.c_str());
    WiFi.begin(cfg.staSSID.c_str(), cfg.staPass.c_str());

    uint8_t retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < MAX_STA_RETRIES) {
        delay(500);
        Serial.print(".");
        retries++;
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        _staConnected = true;
        Serial.printf("[WiFi] STA connected — IP: %s  RSSI: %d dBm\n",
                      WiFi.localIP().toString().c_str(), WiFi.RSSI());
        return true;
    }

    _staConnected = false;
    Serial.println("[WiFi] STA connection FAILED");
    return false;
}

void WiFiManager::startAP() {
    NodeXConfig& cfg = Config.get();

    IPAddress apIP, apGW, apSN;
    apIP.fromString(DEFAULT_AP_IP);
    apGW.fromString(DEFAULT_AP_GATEWAY);
    apSN.fromString(DEFAULT_AP_SUBNET);

    WiFi.softAPConfig(apIP, apGW, apSN);
    WiFi.softAP(cfg.apSSID.c_str(),
                nullptr,              // open network — captive portal handles auth
                cfg.apChannel,
                cfg.apHidden ? 1 : 0,
                cfg.apMaxClients);

    // Increase AP TX power for range
    if (esp_wifi_set_max_tx_power(78) != ESP_OK) {  // 19.5 dBm
        Serial.println("[WiFi] Warning: could not set max TX power");
    }

    Serial.printf("[WiFi] AP started (OPEN) — SSID: %s  IP: %s  Ch: %d\n",
                  cfg.apSSID.c_str(),
                  WiFi.softAPIP().toString().c_str(),
                  cfg.apChannel);
}

void WiFiManager::enableNAT() {
    // Enable NAPT on the AP interface. Must hold TCPIP core lock.
    // If NAPT was already active, disable it first to flush stale NAT entries
    // that accumulate across reconnections and eventually exhaust the table.
    LOCK_TCPIP_CORE();
    if (_natEnabled) {
        ip_napt_enable(WiFi.softAPIP(), 0);  // flush stale translation table
    }
    ip_napt_enable(WiFi.softAPIP(), 1);
    UNLOCK_TCPIP_CORE();
    _natEnabled = true;
    Serial.println("[WiFi] NAT/NAPT enabled — repeater active");
}

void WiFiManager::disableWiFiSleep() {
    esp_wifi_set_ps(WIFI_PS_NONE);
    Serial.println("[WiFi] Power save DISABLED for max throughput");
}

bool WiFiManager::isSTAConnected() {
    _staConnected = (WiFi.status() == WL_CONNECTED);
    return _staConnected;
}

int8_t WiFiManager::getSTARSSI() {
    return WiFi.RSSI();
}

uint8_t WiFiManager::getAPClientCount() {
    wifi_sta_list_t stationList;
    esp_wifi_ap_get_sta_list(&stationList);
    return stationList.num;
}

String WiFiManager::getSTAIP() {
    return WiFi.localIP().toString();
}

String WiFiManager::getAPIP() {
    return WiFi.softAPIP().toString();
}

String WiFiManager::getSTAMAC() {
    return WiFi.macAddress();
}

String WiFiManager::getAPMAC() {
    return WiFi.softAPmacAddress();
}

String WiFiManager::getUpstreamGateway() {
    return WiFi.gatewayIP().toString();
}

std::vector<WiFiManager::ClientInfo> WiFiManager::getConnectedClients() {
    std::vector<ClientInfo> clients;

    wifi_sta_list_t stationList;
    esp_wifi_ap_get_sta_list(&stationList);

    if (stationList.num == 0) return clients;

    // Use esp_netif DHCP server to resolve MAC→IP
    esp_netif_t* ap_netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
    esp_netif_pair_mac_ip_t mac_ip_pairs[10];

    // Fill MAC addresses into the pairs array, then resolve IPs
    int num = stationList.num;
    if (num > 10) num = 10;

    for (int i = 0; i < num; i++) {
        memcpy(mac_ip_pairs[i].mac, stationList.sta[i].mac, 6);
        mac_ip_pairs[i].ip.addr = 0;
    }

    if (ap_netif) {
        esp_netif_dhcps_get_clients_by_mac(ap_netif, num, mac_ip_pairs);
    }

    for (int i = 0; i < num; i++) {
        ClientInfo ci;
        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                 mac_ip_pairs[i].mac[0], mac_ip_pairs[i].mac[1],
                 mac_ip_pairs[i].mac[2], mac_ip_pairs[i].mac[3],
                 mac_ip_pairs[i].mac[4], mac_ip_pairs[i].mac[5]);
        ci.mac = String(macStr);

        if (mac_ip_pairs[i].ip.addr != 0) {
            ci.ip = IPAddress(mac_ip_pairs[i].ip.addr).toString();
        } else {
            ci.ip = "pending";
        }

        clients.push_back(ci);
    }
    return clients;
}

bool WiFiManager::kickClientByMAC(const String& mac) {
    wifi_sta_list_t stationList;
    esp_wifi_ap_get_sta_list(&stationList);
    String target = mac;
    target.toUpperCase();
    for (int i = 0; i < stationList.num; i++) {
        char buf[18];
        snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
                 stationList.sta[i].mac[0], stationList.sta[i].mac[1],
                 stationList.sta[i].mac[2], stationList.sta[i].mac[3],
                 stationList.sta[i].mac[4], stationList.sta[i].mac[5]);
        if (String(buf) == target) {
            esp_wifi_deauth_sta(i + 1);  // AID = index + 1
            return true;
        }
    }
    return false;
}
