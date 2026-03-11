// ============================================================================
// NodeX — Compile-Time Default Configuration
// ============================================================================
#pragma once

// ----- Station (upstream network) -----
#define DEFAULT_STA_SSID          ""
#define DEFAULT_STA_PASS          ""

// ----- Access Point (repeater broadcast) -----
#define DEFAULT_AP_SSID           "NodeX"
#define DEFAULT_AP_PASS           "dev"             // network login code
#define DEFAULT_ADMIN_CODE        "sumnima"         // admin login code
#define DEFAULT_AP_CHANNEL        1
#define DEFAULT_AP_MAX_CLIENTS    8
#define DEFAULT_AP_HIDDEN         false

// ----- Network -----
#define DEFAULT_AP_IP             "192.168.4.1"
#define DEFAULT_AP_GATEWAY        "192.168.4.1"
#define DEFAULT_AP_SUBNET         "255.255.255.0"
#define DEFAULT_DNS_PORT          53

// ----- Web dashboard -----
#define WEB_PORT                  80
#define WEB_USERNAME              "admin"
#define WEB_PASSWORD              "admin"

// ----- OTA -----
#define OTA_HOSTNAME              "NodeX"
#define OTA_PASSWORD              "nodex_ota"
#define OTA_PORT                  3232

// ----- Watchdog / health -----
#define HEALTH_CHECK_INTERVAL_MS  10000
#define STA_RECONNECT_INTERVAL_MS 30000  // 30-second cooldown between reconnect attempts
#define MAX_STA_RETRIES           10   // 5-second max per attempt (10 × 500 ms)

// ----- FreeRTOS task stack sizes -----
#define TASK_STACK_WIFI           4096
#define TASK_STACK_WEB            8192
#define TASK_STACK_HEALTH         3072
#define TASK_STACK_LED            2048
#define TASK_STACK_DNS            4096

// ----- Captive portal -----
#define MAX_PORTAL_SESSIONS       16
#define MAX_ADMIN_SESSIONS        8
#define SESSION_TIMEOUT_MS        3600000UL    // 1 hour
#define ADMIN_SESSION_TIMEOUT_MS  1800000UL    // 30 minutes
#define DNS_FORWARD_TIMEOUT_MS    3000         // 3 seconds
#define MAX_SAVED_NETWORKS        5            // extra Wi-Fi credentials stored in NVS
#define DEFAULT_SESSION_TIMEOUT_MIN  60         // client session timeout in minutes
