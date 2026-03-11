# NodeX — ESP32 Wi-Fi Repeater

<p align="center">
<strong>A production-ready ESP32-based Wi-Fi repeater with NAT, web dashboard, OTA updates, and FreeRTOS task architecture.</strong>
</p>

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
