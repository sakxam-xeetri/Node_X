# NodeX — ESP32 Wi-Fi Repeater

<p align="center">
<strong>Production-ready ESP32 Wi-Fi repeater with NAT forwarding, captive portal, web admin dashboard, OTA updates, and FreeRTOS task architecture.</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/platform-ESP32-blue?style=flat-square">
  <img src="https://img.shields.io/badge/framework-Arduino-teal?style=flat-square">
  <img src="https://img.shields.io/badge/build-PlatformIO-orange?style=flat-square">
  <img src="https://img.shields.io/badge/license-MIT-green?style=flat-square">
</p>

---

## Overview

NodeX turns any ESP32 development board into a full-featured Wi-Fi repeater/hotspot. It connects to an upstream network (STA mode) and rebroadcasts it as its own open Access Point (AP mode) with a captive portal for authentication. Clients connecting to NodeX are shown a terminal-themed portfolio page before gaining internet access.

**Built by [sakshyam](https://www.instagram.com/sakxam_console.log/) — Embedded Systems Engineer, Kathmandu, Nepal.**

---

## Feature List

### Core Networking
- **Dual-mode Wi-Fi** — STA + AP simultaneously on the same radio
- **NAT/NAPT forwarding** — Full internet routing for all AP clients through the upstream STA link
- **Up to 10 AP clients** — Configurable 1–10 (ESP32 hardware limit), default 8
- **Wi-Fi power save disabled** — Maximum throughput, minimum latency
- **TX power boosted** — `esp_wifi_set_max_tx_power(78)` → 19.5 dBm for range
- **Auto-reconnect** — Health task reconnects to upstream every 30 seconds on disconnect
- **Saved networks** — Store up to 5 upstream credentials; switch with one click

### Captive Portal
- **OS-native popup** — Triggers the built-in captive portal browser on Android, iOS, Windows, and macOS automatically
- **Per-client authentication** — Each device must authenticate before DNS is forwarded
- **Two access levels** — Network code (browsing only) or Admin code (full dashboard access)
- **Configurable session timeout** — Set how many minutes clients can browse before re-login (1–1440 min)
- **Session management** — Admin can view, deauth, or kick any client from the dashboard
- **DNS interception** — Unauthenticated clients get all DNS redirected to AP IP; authenticated clients get DNS forwarded to 8.8.8.8

### Captive Portal Login Page
- Fully interactive **terminal/portfolio page** themed as an Arch Linux session
- Animated **boot sequence** (BIOS → kernel → systemd → login)
- `neofetch`-style ASCII art header
- Sections: About, Skills (Programming, Hardware, AI/ML, Security, Exploring), Approach, Contact, Socials
- **Interactive terminal** — supports `help`, `login: <code>`, `clear`, `whoami`, `about`, `skills`, `contact`, `wifi`, `status`, `ls`, `pwd`, `date`, `uname`
- Command history with ↑/↓ arrow keys
- Click anywhere to focus the prompt

### Connected / QR Page
- After successful login, client is redirected to `/connected`
- Full **terminal-themed page** (dark background, monospace font)
- Red `[✓]` connection status lines
- **Large QR code** + **Instagram link** so clients can follow the developer
- Yellow section headers, red link button, blinking green cursor
- No emojis — pure terminal aesthetic

### Web Admin Dashboard
- Dark terminal-themed responsive UI (`/admin`)
- Protected by **cookie-based session tokens** (30-minute timeout) with HTTP Basic Auth fallback
- Auto-refreshes status every 5 seconds

#### Dashboard Tab
- Uptime, free heap, chip model, CPU frequency, flash size
- STA status with animated signal strength bar (green/yellow/red by RSSI)
- AP info: SSID, IP, MAC, client count / max clients

#### Clients Tab
- Live table of all connected MAC addresses and IPs
- Portal session status per client (authenticated age shown)
- **Deauth** button — revokes session (forces re-login through captive portal)
- **Kick** button — deauthenticates device from Wi-Fi entirely

#### Networks Tab
- List of all saved upstream networks
- **Use** button — sets as upstream and reboots NodeX
- **Remove** button — deletes saved credentials
- **Scan WiFi Networks** button — scans nearby APs and lists them with SSID, RSSI, channel, and lock icon
- **Select** button on any scanned network — auto-fills SSID into the save form
- Save new credentials form (SSID + password)

#### Config Tab
- **Station (Upstream)**: SSID + password with **show/hide toggle**
- **Access Point**: SSID, channel (1–13), max clients (1–10), hidden SSID toggle
- **Login Codes**: Network code, Admin code (current values shown as placeholders)
- **Dashboard Auth (API)**: Username, password with **show/hide toggle**
- **OTA Password** with **show/hide toggle**
- **Session Control**: Client timeout in minutes — how long a client can use WiFi before re-login is required (applies immediately, no reboot needed)
- Inline **Scan Networks** button auto-fills SSID without leaving the config tab

#### Tools Tab
- **Reboot** device button
- **Factory Reset** — erases all NVS config and reboots
- OTA hostname and port info

### Status LED (GPIO 2, PWM)

| Pattern | Trigger |
|---|---|
| **Breathe** (smooth pulse) | Normal — STA connected, no AP clients |
| **Slow blink** (1s on/off) | Client(s) connected to AP — continuous while any device is on the repeater |
| **Fast blink** (200ms) | Error — STA not connected, no clients |
| **Triple quick flash** (NOTIFY) | New connection event detected |
| **Off** | System not initialized |

> LED automatically switches back to breathe/fast-blink when all clients disconnect.

### OTA Updates
- ArduinoOTA over Wi-Fi (no USB cable needed after first flash)
- Hostname: `NodeX`, Port: `3232`
- Configurable OTA password via dashboard
- Progress logged to Serial

### Configuration Persistence
- All settings stored in **ESP32 NVS** (Non-Volatile Storage) — survives reboots and power cuts
- Thread-safe with recursive FreeRTOS mutex for dual-core access
- Factory reset: hold GPIO26 LOW for 3 seconds during boot, or use the web API

---

## Project Structure

```
Node_X/
├── platformio.ini              # Build config, dependencies, partition table
├── index.html                  # Development preview of the dashboard
├── HARDWARE.md                 # Hardware setup & pin guide
├── README.md                   # This file
├── include/
│   ├── nodex_defaults.h        # Compile-time defaults (SSID, passwords, timeouts)
│   ├── nodex_pins.h            # GPIO pin definitions
│   ├── config_manager.h        # NVS configuration struct + persistence API
│   ├── wifi_manager.h          # STA+AP dual-mode + NAT/NAPT control
│   ├── captive_portal.h        # DNS intercept + per-client session management
│   ├── web_dashboard.h         # ESPAsyncWebServer + REST API declarations
│   ├── web_ui.h                # Embedded HTML/CSS/JS (portal, connected, admin)
│   ├── ota_manager.h           # ArduinoOTA wrapper
│   └── status_led.h            # LED PWM pattern controller
└── src/
    ├── main.cpp                # Entry point, FreeRTOS task setup + health loop
    ├── config_manager.cpp      # NVS read/write/migrate implementation
    ├── wifi_manager.cpp        # Wi-Fi + NAT implementation + client list
    ├── captive_portal.cpp      # DNS redirect/forward + session CRUD
    ├── web_dashboard.cpp       # All HTTP route handlers + API logic
    ├── ota_manager.cpp         # OTA callbacks
    └── status_led.cpp          # LED PWM: blink/breathe/notify patterns
```

---

## Hardware

### Required
- Any **ESP32 development board** with onboard LED on GPIO 2
  - ESP32-DevKitC V4 (recommended)
  - ESP32-WROOM-32D / 32E
  - ESP32-WROVER-B (extra PSRAM, best for >6 clients)
  - NodeMCU-32S

### Optional
| Component | Purpose | GPIO |
|---|---|---|
| Onboard LED | Status indicator (built-in) | 2 |
| External LED | Activity indicator | 4 |
| Tactile button | Factory reset (hold 3s on boot) | 26 |
| External antenna | Extended range | U.FL (board dependent) |

### Power
- **USB 5V / 1A minimum** — ESP32 peaks ~500mA during Wi-Fi TX
- Recommended: **5V / 2A** USB adapter or regulated 3.3V supply
- Avoid PC USB ports under load — brownouts cause random resets

> See [HARDWARE.md](HARDWARE.md) for full deployment guide, placement tips, and production checklist.

---

## Quick Start

### 1. Prerequisites
- [VS Code](https://code.visualstudio.com/) + [PlatformIO extension](https://platformio.org/install/ide?install=vscode)
- ESP32 dev board + USB cable

### 2. Clone & Open
```bash
git clone <repo-url> Node_X
cd Node_X
code .
```

### 3. (Optional) Pre-configure Wi-Fi
Edit `include/nodex_defaults.h` before flashing:
```cpp
#define DEFAULT_STA_SSID  "YourUpstreamWiFi"
#define DEFAULT_STA_PASS  "YourPassword"
```
Or leave blank — configure via web dashboard after first boot.

### 4. Flash
```bash
pio run -t upload
pio device monitor -b 115200
```
Set the correct COM port in `platformio.ini` (`upload_port = COM7`).

### 5. Connect & Set Up
1. On your phone/laptop, connect to Wi-Fi network **`NodeX`** (open — no password)
2. The captive portal will pop up automatically (or open `http://192.168.4.1`)
3. Type `login: sumnima` in the terminal to access the admin dashboard
4. Go to **Config tab** → enter your upstream Wi-Fi SSID and password → **Save Configuration**
5. Go to **Tools tab** → **Reboot**
6. NodeX reconnects to upstream, LED starts breathing — repeater is active

---

## Default Credentials

| Setting | Default Value |
|---|---|
| AP SSID | `NodeX` |
| Network login code | `dev` |
| Admin login code | `sumnima` |
| Dashboard username | `admin` |
| Dashboard password | `admin` |
| OTA password | `nodex_ota` |
| Client session timeout | `60 minutes` |

> **Change these before deploying publicly.** All credentials are configurable via the Config tab.

---

## REST API

All endpoints require admin authentication (cookie session **or** HTTP Basic Auth: `admin`/`admin`).

| Method | Endpoint | Auth | Description |
|---|---|---|---|
| `GET` | `/` | — | Captive portal login page |
| `POST` | `/portal/login` | — | Authenticate with code |
| `GET` | `/connected` | — | Post-login QR/Instagram page |
| `POST` | `/portal/logout` | — | Clear admin session |
| `GET` | `/admin` | ✓ | Web admin dashboard |
| `GET` | `/api/status` | ✓ | System info, STA/AP details, RSSI, heap |
| `GET` | `/api/clients` | ✓ | Connected AP client list (MAC + IP) |
| `GET` | `/api/scan` | ✓ | Trigger/fetch Wi-Fi network scan |
| `GET` | `/api/config` | ✓ | Current config (passwords omitted from response) |
| `POST` | `/api/config` | ✓ | Save configuration (JSON body) |
| `POST` | `/api/reboot` | ✓ | Reboot device |
| `POST` | `/api/factory-reset` | ✓ | Erase NVS config and reboot |
| `GET` | `/api/sessions` | ✓ | Active captive portal sessions |
| `POST` | `/api/sessions/deauth` | ✓ | Revoke a portal session by IP |
| `POST` | `/api/clients/kick` | ✓ | Kick a client by MAC address |
| `GET` | `/api/networks` | ✓ | List saved upstream networks |
| `POST` | `/api/networks/add` | ✓ | Save new network credentials |
| `POST` | `/api/networks/remove` | ✓ | Remove saved network by index |
| `POST` | `/api/networks/connect` | ✓ | Switch to saved network (reboots) |

### Example API Calls

**Save upstream Wi-Fi:**
```bash
curl -u admin:admin -X POST http://192.168.4.1/api/config \
  -H "Content-Type: application/json" \
  -d '{"staSSID":"MyNetwork","staPass":"secret123"}'
```

**Set client session timeout to 30 minutes:**
```bash
curl -u admin:admin -X POST http://192.168.4.1/api/config \
  -H "Content-Type: application/json" \
  -d '{"sessionTimeout":30}'
```

**Kick a client:**
```bash
curl -u admin:admin -X POST http://192.168.4.1/api/clients/kick \
  -d 'mac=AA:BB:CC:DD:EE:FF'
```

**Trigger a Wi-Fi scan then fetch results:**
```bash
curl -u admin:admin http://192.168.4.1/api/scan        # triggers scan
sleep 5
curl -u admin:admin http://192.168.4.1/api/scan        # fetch results
```

---

## Configuration Reference

All fields in `POST /api/config` (JSON body):

| Field | Type | Description |
|---|---|---|
| `staSSID` | string | Upstream Wi-Fi network name |
| `staPass` | string | Upstream Wi-Fi password |
| `apSSID` | string | NodeX broadcast name |
| `apPass` | string | Network login code for clients |
| `adminCode` | string | Admin portal access code |
| `apChannel` | int (1–13) | Wi-Fi channel |
| `apMaxClients` | int (1–10) | Max simultaneous AP clients |
| `apHidden` | bool | Hide SSID from scans |
| `webUser` | string | Dashboard API username |
| `webPass` | string | Dashboard API password |
| `otaPass` | string | OTA firmware update password |
| `sessionTimeout` | int (1–1440) | Client session duration in minutes |

---

## Architecture

```
┌─────────────────────────────────────────────────────┐
│                     ESP32 SoC                        │
│                                                      │
│  Core 0 (Arduino event core)   Core 1 (app core)    │
│  ┌──────────────────────┐     ┌──────────────────┐   │
│  │  ESPAsyncWebServer   │     │  taskHealth      │   │
│  │  - REST API          │     │   STA reconnect  │   │
│  │  - Admin dashboard   │     │   NAT re-enable  │   │
│  │  - Captive portal    │     │   LED management │   │
│  │  - OTA page          │     │   Client counter │   │
│  └──────────────────────┘     ├──────────────────┤   │
│                               │  taskOTA         │   │
│  ┌──────────────────────┐     │   ArduinoOTA     │   │
│  │  taskDNS (Core 0)    │     ├──────────────────┤   │
│  │   DNS intercept      │     │  taskLED         │   │
│  │   DNS forward        │     │   PWM patterns   │   │
│  └──────────────────────┘     └──────────────────┘   │
│                                                      │
│  ┌────────────────────────────────────────────────┐  │
│  │              Wi-Fi (STA + AP + NAT)             │  │
│  │  Router ←──[STA]──── ESP32 ────[AP]──→ Clients  │  │
│  │                        └── NAPT forwarding ──┘  │  │
│  └────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────┘
```

### FreeRTOS Task Summary

| Task | Core | Priority | Stack | Responsibility |
|---|---|---|---|---|
| `taskHealth` | 1 | 2 | 3072 B | STA reconnect, NAT, LED state, client tracking |
| `taskLED` | 1 | 1 | 2048 B | LED PWM update every 20ms |
| `taskOTA` | 1 | 3 | 8192 B | ArduinoOTA polling every 50ms |
| `taskDNS` | 0 | 2 | 4096 B | DNS redirect/forward (blocking loop) |
| AsyncWebServer | 0 | — | — | HTTP requests (managed by library) |

---

## LED Behaviour in Detail

The `taskHealth` loop runs every 1 second and manages LED state based on real-time conditions:

```
Client(s) on AP?  →  BLINK_SLOW (1s on/off) — continuous while any device connected
      ↓ (no clients)
STA connected?    →  BREATHE (smooth PWM fade)
      ↓ (STA down)
Error state       →  BLINK_FAST (200ms on/off)

Connection event  →  BLINK_NOTIFY (triple 100ms flash) then resume previous pattern
```

The LED never overrides a reconnect attempt or notify sequence mid-execution.

---

## Captive Portal Flow

```
Client connects to NodeX AP
         ↓
All DNS queries → redirected to 192.168.4.1 (AP IP)
         ↓
OS detects captive portal → opens mini browser
         ↓
Terminal portfolio page shown
         ↓
Client types: login: <code>
         ↓
         ├── code == apPass  →  session created → /connected page shown
         │                      (QR + Instagram link, then browse freely)
         │
         └── code == adminCode → session created + admin cookie set → /admin
                                  (Full web dashboard)
```

After authentication, DNS is forwarded to `8.8.8.8` for that client's IP. Sessions expire after the configured timeout.

---

## OTA Updates

After the first USB flash, all future updates can be done over Wi-Fi:

### PlatformIO OTA
Uncomment in `platformio.ini`:
```ini
upload_protocol = espota
upload_port     = 192.168.4.1
upload_flags    = --auth=nodex_ota
```
Then:
```bash
pio run -t upload
```

### Arduino IDE
**Tools → Port → NodeX at 192.168.4.1** → Upload normally.

Change the OTA password in the Config tab before deploying publicly.

---

## Performance

| Optimization | Implementation |
|---|---|
| Wi-Fi power save disabled | `esp_wifi_set_ps(WIFI_PS_NONE)` |
| Max TX power | `esp_wifi_set_max_tx_power(78)` — 19.5 dBm |
| FreeRTOS task separation | LED/OTA/health on Core 1; web on Core 0 |
| Async web server | Non-blocking HTTP — no main loop blocking |
| Main loop yields | `vTaskDelay(portMAX_DELAY)` — zero CPU waste |
| NVS config caching | Loaded once at boot, modified in RAM, written on save |
| NAT table flush | NAPT disabled+re-enabled on reconnect to clear stale entries |

Typical throughput: **10–25 Mbps** depending on channel congestion and board antenna.

---

## Security Notes

- **Change all default credentials** before public deployment (Config tab)
- AP uses **open network** (no WPA2) — authentication handled by captive portal
- Admin dashboard protected by **cookie-based session tokens** (32-char hex, 30-min expiry)
- **HTTP Basic Auth fallback** for API/curl access
- Passwords are **never returned** by `GET /api/config` — only SSIDs and non-secret fields
- All passwords stored in ESP32 NVS flash (not plain text in source after first config)
- Input validation and size limits on all POST endpoints
- Factory reset requires physical GPIO26 button OR authenticated admin API call
- `adminCode` and `apPass` are validated to never be equal (config_manager enforces this)

---

## Captive Portal Detection Endpoints

NodeX responds to all OS captive portal probes:

| OS | Probe URL |
|---|---|
| Android / Chrome | `/generate_204`, `/gen_204`, `/api/generate_204` |
| iOS / macOS | `/hotspot-detect.html`, `/library/test/success.html` |
| Windows NCSI | `/connecttest.txt`, `/ncsi.txt` |
| Generic | `/redirect`, `/success.txt`, `/mobile/status.php` |

Authenticated clients receive correct `204` / `200 Success` responses so the OS clears the captive portal indicator and Wi-Fi shows as fully connected.

---

## Dependencies

| Library | Version | Purpose |
|---|---|---|
| [ArduinoJson](https://github.com/bblanchon/ArduinoJson) | ^7.0.0 | JSON for REST API |
| [ESPAsyncWebServer](https://github.com/mathieucarbou/ESPAsyncWebServer) | ^3.6.0 | Async HTTP server |
| [AsyncTCP](https://github.com/mathieucarbou/AsyncTCP) | ^3.3.2 | TCP for async server |
| ESP-IDF lwIP NAT | built-in (build flags) | NAT/NAPT forwarding |
| ArduinoOTA | built-in (Arduino core) | Wireless firmware updates |
| Preferences | built-in (Arduino core) | NVS config storage |

Build flags required in `platformio.ini`:
```ini
build_flags =
    -DCONFIG_LWIP_IP_FORWARD=1
    -DCONFIG_LWIP_IPV4_NAPT=1
    -DARDUINO_RUNNING_CORE=1
    -DARDUINO_EVENT_RUNNING_CORE=0
```

---

## Defaults Reference (`nodex_defaults.h`)

```cpp
DEFAULT_STA_SSID         ""             // configure via dashboard
DEFAULT_AP_SSID          "NodeX"
DEFAULT_AP_PASS          "dev"          // network login code
DEFAULT_ADMIN_CODE       "sumnima"      // admin login code
DEFAULT_AP_CHANNEL       1
DEFAULT_AP_MAX_CLIENTS   8
DEFAULT_AP_HIDDEN        false
DEFAULT_AP_IP            "192.168.4.1"
WEB_USERNAME             "admin"
WEB_PASSWORD             "admin"
OTA_PASSWORD             "nodex_ota"
SESSION_TIMEOUT_MS       3600000        // 1 hour (default)
ADMIN_SESSION_TIMEOUT_MS 1800000        // 30 minutes
DEFAULT_SESSION_TIMEOUT_MIN 60          // configurable via dashboard
MAX_PORTAL_SESSIONS      16
MAX_ADMIN_SESSIONS       8
MAX_SAVED_NETWORKS       5
STA_RECONNECT_INTERVAL   30 seconds
HEALTH_CHECK_INTERVAL    10 seconds
```

---

## Troubleshooting

| Symptom | Likely Cause | Fix |
|---|---|---|
| NodeX AP not visible | Boot failed | Check serial monitor for errors |
| LED fast-blinking only | STA not connected | Check upstream SSID/password in Config tab |
| Captive portal not popping up | OS cached previous state | Forget & reconnect to NodeX Wi-Fi |
| Can't access `/admin` | Session expired or wrong code | Use `login: <adminCode>` in portal terminal |
| OTA upload fails | Wrong password or IP | Confirm IP in serial output; check OTA password |
| Internet access after login not working | STA disconnected | Check LED — if fast blink, upstream is down |
| Clients can't get IP | DHCP not started | Reboot NodeX; check AP is active in dashboard |
| Build error: `SESSION_TIMEOUT_MS` | Missing include | `captive_portal.h` must include `nodex_defaults.h` |

---

## License

MIT — Free to use for personal and commercial projects.

---

<p align="center">Built with ❤️ by <a href="https://www.instagram.com/sakxam_console.log/">@sakxam_console.log</a></p>


---

## Features

- **Dual-mode Wi-Fi** — Connects to upstream network (STA) and rebroadcasts as a secure Access Point (AP) simultaneously
- **NAT/NAPT forwarding** — Full internet access for all connected AP clients via the upstream STA link
- **Web dashboard** — Dark-themed responsive UI for setup, monitoring, signal strength, and client management
- **OTA updates** — Flash new firmware wirelessly via PlatformIO or Arduino IDE
- **FreeRTOS tasks** — Dedicated tasks for health monitoring, LED status, and OTA on separate cores
- **Persistent config** — All settings stored in NVS flash; survives reboots
- **Factory reset** — Physical button (GPIO0) or web API
- **Wi-Fi sleep disabled** — Maximum throughput and minimum latency
- **Up to 8 clients** — Configurable; ESP32 hardware supports up to 10

---

## Project Structure

```
Node__X/
├── platformio.ini              # Build config, dependencies, partitions
├── include/
│   ├── nodex_defaults.h        # Compile-time default values
│   ├── nodex_pins.h            # GPIO pin definitions
│   ├── config_manager.h        # NVS configuration persistence
│   ├── wifi_manager.h          # STA+AP dual-mode + NAT control
│   ├── web_dashboard.h         # Async web server + REST API
│   ├── web_ui.h                # Embedded HTML/CSS/JS dashboard
│   ├── ota_manager.h           # ArduinoOTA wrapper
│   └── status_led.h            # LED pattern controller
├── src/
│   ├── main.cpp                # Entry point, FreeRTOS task setup
│   ├── config_manager.cpp      # NVS read/write implementation
│   ├── wifi_manager.cpp        # Wi-Fi + NAT implementation
│   ├── web_dashboard.cpp       # REST API handlers
│   ├── ota_manager.cpp         # OTA implementation
│   └── status_led.cpp          # LED PWM patterns
├── HARDWARE.md                 # Hardware setup & deployment guide
└── README.md                   # This file
```

---

## Quick Start

### 1. Clone & Open
```bash
git clone <repo-url> NodeX
cd NodeX
code .   # Open in VS Code with PlatformIO extension
```

### 2. Configure Upstream Wi-Fi (optional before flash)
Edit `include/nodex_defaults.h`:
```cpp
#define DEFAULT_STA_SSID  "YourWiFiName"
#define DEFAULT_STA_PASS  "YourWiFiPassword"
```
Or leave blank and configure via the web dashboard after boot.

### 3. Flash
```bash
pio run -t upload
pio device monitor -b 115200
```

### 4. Connect & Configure
1. Connect to Wi-Fi network **NodeX_Repeater** (password: `nodex1234`)
2. Open browser → **http://192.168.4.1**
3. Login: `admin` / `admin`
4. Go to **Configuration** tab → enter upstream Wi-Fi credentials → Save → Reboot

---

## Web Dashboard

The built-in dashboard provides:

| Tab | Features |
|---|---|
| **Dashboard** | STA/AP status, signal strength bar, system info, uptime |
| **Clients** | List of connected devices with MAC and IP addresses |
| **Configuration** | STA credentials, AP settings, channel, security, network scan |
| **Tools** | Reboot, factory reset, OTA info |

Auto-refreshes every 5 seconds. Secured with HTTP Basic Authentication.

---

## REST API

All endpoints require HTTP Basic Auth (default `admin`/`admin`).

| Method | Endpoint | Description |
|---|---|---|
| GET | `/api/status` | System status, STA/AP info, RSSI, heap |
| GET | `/api/clients` | List of connected AP clients |
| GET | `/api/scan` | Trigger/fetch Wi-Fi network scan |
| GET | `/api/config` | Current configuration (passwords omitted) |
| POST | `/api/config` | Save configuration (JSON body) |
| POST | `/api/reboot` | Reboot device |
| POST | `/api/factory-reset` | Erase config and reboot |

### Example: Save Config
```bash
curl -u admin:admin -X POST http://192.168.4.1/api/config \
  -H "Content-Type: application/json" \
  -d '{"staSSID":"MyNetwork","staPass":"secret123"}'
```

---

## Architecture

```
┌─────────────────────────────────────────────────┐
│                    ESP32 SoC                     │
│                                                  │
│  Core 0                    Core 1                │
│  ┌──────────────┐         ┌──────────────────┐   │
│  │ AsyncWebSrv  │         │ taskHealth       │   │
│  │ (dashboard)  │         │  - STA reconnect │   │
│  └──────────────┘         │  - NAT re-enable │   │
│                           │  - Heap monitor  │   │
│                           ├──────────────────┤   │
│                           │ taskOTA          │   │
│                           │  - OTA polling   │   │
│                           ├──────────────────┤   │
│                           │ taskLED          │   │
│                           │  - Status LED    │   │
│                           └──────────────────┘   │
│                                                  │
│  ┌──────────────────────────────────────────┐    │
│  │          Wi-Fi (STA + AP + NAT)          │    │
│  │   STA ←──→ Router    AP ←──→ Clients     │    │
│  │              └── NAPT forwarding ──┘      │    │
│  └──────────────────────────────────────────┘    │
└─────────────────────────────────────────────────┘
```

---

## Performance Tuning

These optimizations are already applied in the firmware:

| Optimization | Implementation |
|---|---|
| Wi-Fi power save disabled | `esp_wifi_set_ps(WIFI_PS_NONE)` |
| AMPDU TX/RX enabled | Build flags in platformio.ini |
| Max TX power | `esp_wifi_set_max_tx_power(78)` — 19.5 dBm |
| FreeRTOS task separation | Health, OTA, LED on Core 1; Web on Core 0 |
| Async web server | Non-blocking request handling |
| Main loop yields | `vTaskDelay(portMAX_DELAY)` — zero CPU waste |
| NVS config caching | Config loaded once, modified in RAM, persisted on save |

---

## OTA Updates

After initial USB flash, update wirelessly:

### PlatformIO
Uncomment the OTA section in `platformio.ini`, then:
```bash
pio run -t upload
```

### Arduino IDE
**Tools → Port → NodeX at 192.168.4.1** → Upload

Default OTA password: `nodex_ota` (change in Configuration tab).

---

## Security Notes

- **Change default passwords** before deploying (web dashboard + OTA)
- AP uses **WPA2** encryption by default
- Dashboard protected with **HTTP Basic Auth**
- Passwords are **never exposed** via the GET `/api/config` endpoint
- AP password requires minimum 8 characters (WPA2 standard)
- Input validation on all API endpoints
- Factory reset requires physical button access OR authenticated API call

---

## LED Status Patterns

| Pattern | Meaning |
|---|---|
| **Breathing** (slow pulse) | Normal operation — STA connected |
| **Slow blink** (1s interval) | Connecting to upstream Wi-Fi |
| **Fast blink** (200ms interval) | Error — STA connection failed |
| **Off** | System not initialized |

---

## Hardware

See [HARDWARE.md](HARDWARE.md) for:
- Recommended ESP32 boards
- Power supply requirements
- Pin mapping
- Optimal placement guide
- Production reliability checklist
- Troubleshooting table

---

## Dependencies

| Library | Version | Purpose |
|---|---|---|
| [ArduinoJson](https://github.com/bblanchon/ArduinoJson) | ^7.0.0 | JSON serialization for API |
| [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) | ^1.2.3 | Async HTTP server |
| [AsyncTCP](https://github.com/me-no-dev/AsyncTCP) | ^1.1.1 | TCP transport for async server |
| [lwip_nat_lib](https://github.com/martin-ger/lwip_nat_lib) | ^1.0.0 | NAT/NAPT for Wi-Fi repeating |

---

## License

MIT — Use freely for personal and commercial projects.
