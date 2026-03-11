// ============================================================================
// NodeX — Captive Portal DNS & Session Manager Implementation
// ============================================================================
#include "captive_portal.h"
#include "nodex_defaults.h"

CaptivePortal Portal;

// ---- Lifecycle ----

void CaptivePortal::begin(IPAddress apIP, IPAddress upstreamDNS) {
    _apIP = apIP;
    _upstreamDNS = upstreamDNS;
    _mutex = xSemaphoreCreateMutex();
    _dns.begin(53);
    _running = true;
    Serial.printf("[Portal] Captive DNS started (upstream %s)\n",
                  upstreamDNS.toString().c_str());
}

void CaptivePortal::stop() {
    _running = false;
    _dns.stop();
}

// ---- Session management (thread-safe) ----

bool CaptivePortal::isAuthenticated(IPAddress clientIP) {
    if (!_mutex) return false;
    xSemaphoreTake(_mutex, portMAX_DELAY);
    uint32_t ip = (uint32_t)clientIP;
    unsigned long now = millis();
    bool found = false;
    for (auto it = _clients.begin(); it != _clients.end(); ) {
        if ((now - it->authTime) >= SESSION_TIMEOUT_MS) {
            it = _clients.erase(it);          // prune expired
        } else {
            if (it->ip == ip) found = true;
            ++it;
        }
    }
    xSemaphoreGive(_mutex);
    return found;
}

void CaptivePortal::authenticateClient(IPAddress clientIP) {
    if (!_mutex) return;
    xSemaphoreTake(_mutex, portMAX_DELAY);
    uint32_t ip = (uint32_t)clientIP;
    for (auto& c : _clients) {
        if (c.ip == ip) { c.authTime = millis(); xSemaphoreGive(_mutex); return; }
    }
    if (_clients.size() < MAX_PORTAL_SESSIONS) {
        _clients.push_back({ip, millis()});
        Serial.printf("[Portal] Authenticated %s (%d sessions)\n",
                      clientIP.toString().c_str(), _clients.size());
    }
    xSemaphoreGive(_mutex);
}

void CaptivePortal::deauthClient(IPAddress clientIP) {
    if (!_mutex) return;
    xSemaphoreTake(_mutex, portMAX_DELAY);
    uint32_t ip = (uint32_t)clientIP;
    _clients.erase(
        std::remove_if(_clients.begin(), _clients.end(),
            [ip](const AuthEntry& e) { return e.ip == ip; }),
        _clients.end());
    xSemaphoreGive(_mutex);
}

void CaptivePortal::clearAllSessions() {
    if (!_mutex) return;
    xSemaphoreTake(_mutex, portMAX_DELAY);
    _clients.clear();
    xSemaphoreGive(_mutex);
}

std::vector<CaptivePortal::SessionInfo> CaptivePortal::getSessions() {
    std::vector<SessionInfo> result;
    if (!_mutex) return result;
    xSemaphoreTake(_mutex, portMAX_DELAY);
    unsigned long now = millis();
    for (auto& c : _clients) {
        unsigned long age = now - c.authTime;
        if (age < SESSION_TIMEOUT_MS)
            result.push_back({IPAddress(c.ip), age});
    }
    xSemaphoreGive(_mutex);
    return result;
}

// ---- DNS loop (runs in its own FreeRTOS task) ----

void CaptivePortal::dnsLoop() {
    while (_running) {
        int pktSize = _dns.parsePacket();
        if (pktSize > 0 && pktSize <= 512) {
            uint8_t buf[512];
            int len = _dns.read(buf, sizeof(buf));
            IPAddress remote = _dns.remoteIP();
            uint16_t  port   = _dns.remotePort();

            if (isAuthenticated(remote)) {
                _forwardQuery(remote, port, buf, len);
            } else {
                _sendRedirect(remote, port, buf, len);
            }
            // No delay here — immediately poll for the next pending packet
        } else {
            vTaskDelay(pdMS_TO_TICKS(1));  // 1 ms idle to yield CPU
        }
    }
}

// ---- Build spoofed A-record response pointing to AP IP ----

void CaptivePortal::_sendRedirect(IPAddress client, uint16_t port,
                                   uint8_t* qBuf, int qLen) {
    if (qLen < 12) return;

    // Extract QTYPE before building the response
    int qi = 12;
    while (qi < qLen && qBuf[qi] != 0) qi += qBuf[qi] + 1;
    if (qi + 4 >= qLen) return;  // need null + QTYPE(2) + QCLASS(2)
    uint16_t qtype = ((uint16_t)qBuf[qi + 1] << 8) | qBuf[qi + 2];
    qi += 5;  // null terminator + QTYPE(2) + QCLASS(2)
    int qSectionLen = qi - 12;

    uint8_t resp[512];
    resp[0] = qBuf[0]; resp[1] = qBuf[1];               // Transaction ID
    resp[2] = 0x81;    resp[3] = 0x80;                   // Flags: reply, no error
    resp[4] = qBuf[4]; resp[5] = qBuf[5];                // Questions count
    resp[8] = 0x00;    resp[9] = 0x00;                   // 0 authority
    resp[10] = 0x00;   resp[11] = 0x00;                  // 0 additional

    if (qtype != 0x0001) {
        // AAAA or other non-A type: return NOERROR with 0 answers so the
        // client falls back to an IPv4 (A) query which we CAN intercept.
        resp[6] = 0x00; resp[7] = 0x00;                  // 0 answers
        if (12 + qSectionLen > (int)sizeof(resp)) return;
        memcpy(resp + 12, qBuf + 12, qSectionLen);
        _dns.beginPacket(client, port);
        _dns.write(resp, 12 + qSectionLen);
        _dns.endPacket();
        return;
    }

    // Type A: redirect all hostnames to AP IP with a short TTL so the client
    // re-resolves quickly after authentication.
    resp[6] = 0x00; resp[7] = 0x01;                      // 1 answer
    if (12 + qSectionLen + 16 > (int)sizeof(resp)) return;
    memcpy(resp + 12, qBuf + 12, qSectionLen);

    int off = 12 + qSectionLen;
    resp[off++] = 0xC0; resp[off++] = 0x0C;              // Name pointer
    resp[off++] = 0x00; resp[off++] = 0x01;              // Type A
    resp[off++] = 0x00; resp[off++] = 0x01;              // Class IN
    resp[off++] = 0x00; resp[off++] = 0x00;
    resp[off++] = 0x00; resp[off++] = 0x05;              // TTL = 5 s
    resp[off++] = 0x00; resp[off++] = 0x04;              // Data length
    resp[off++] = _apIP[0];
    resp[off++] = _apIP[1];
    resp[off++] = _apIP[2];
    resp[off++] = _apIP[3];

    _dns.beginPacket(client, port);
    _dns.write(resp, off);
    _dns.endPacket();
}

// ---- Forward DNS to upstream and relay the response ----

void CaptivePortal::_forwardQuery(IPAddress client, uint16_t port,
                                   uint8_t* qBuf, int qLen) {
    WiFiUDP fwd;
    if (!fwd.begin(0)) return;

    fwd.beginPacket(_upstreamDNS, 53);
    fwd.write(qBuf, qLen);
    fwd.endPacket();

    unsigned long start = millis();
    int respSize = 0;
    while (millis() - start < DNS_FORWARD_TIMEOUT_MS) {
        respSize = fwd.parsePacket();
        if (respSize > 0) break;
        vTaskDelay(pdMS_TO_TICKS(2));
    }

    if (respSize > 0 && respSize <= 512) {
        uint8_t respBuf[512];
        int len = fwd.read(respBuf, sizeof(respBuf));
        _dns.beginPacket(client, port);
        _dns.write(respBuf, len);
        _dns.endPacket();
    }
    fwd.stop();
}
