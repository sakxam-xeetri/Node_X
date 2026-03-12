<div align="center">

```
███╗   ██╗ ██████╗ ██████╗ ███████╗██╗  ██╗
████╗  ██║██╔═══██╗██╔══██╗██╔════╝╚██╗██╔╝
██╔██╗ ██║██║   ██║██║  ██║█████╗   ╚███╔╝ 
██║╚██╗██║██║   ██║██║  ██║██╔══╝   ██╔██╗ 
██║ ╚████║╚██████╔╝██████╔╝███████╗██╔╝ ██╗
╚═╝  ╚═══╝ ╚═════╝ ╚═════╝ ╚══════╝╚═╝  ╚═╝
```

**Keychain-sized portable Wi-Fi utility device built on ESP32**

*Advanced networking tools · Configurable captive portal · Personal digital identity beacon*

---

[![Platform](https://img.shields.io/badge/platform-ESP32-blue?style=for-the-badge&logo=espressif&logoColor=white)](https://www.espressif.com/)
[![Framework](https://img.shields.io/badge/framework-Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![Build](https://img.shields.io/badge/build-PlatformIO-FF7F00?style=for-the-badge&logo=platformio&logoColor=white)](https://platformio.org/)
[![License](https://img.shields.io/badge/license-MIT-22C55E?style=for-the-badge)](LICENSE)
[![Form Factor](https://img.shields.io/badge/form_factor-keychain-8B5CF6?style=for-the-badge)]()

</div>

---

## What is NodeX?

**NodeX** is a keychain-sized portable Wi-Fi utility device built on ESP32 that combines advanced networking tools, a configurable captive portal, and a personal digital identity beacon.

It is designed for **real-world deployment**, **tech exhibitions**, **robotics demonstrations**, and **portable networking labs** — fitting in your pocket yet packing the power of a full wireless infrastructure node.

> Built by **[sakshyam](https://www.instagram.com/sakxam_console.log/)** — Embedded Systems Engineer, Kathmandu, Nepal.

### Why NodeX?

| Use Case | What it does |
|---|---|
| **Tech Exhibitions** | Broadcast your identity + portfolio as a captive portal — visitors connect and see *you* |
| **Robotics Demos** | Standalone Wi-Fi AP that bridges your bot to any upstream network on the fly |
| **Portable Network Lab** | Full STA+AP+NAT repeater, DNS interceptor, and client manager in your pocket |
| **Field Deployments** | OTA updates, NVS persistence, and watchdog recovery — runs unattended for days |
| **CTF / Security Research** | Captive portal with per-client auth, session management, and DNS interception |

---

## Feature Overview

<details>
<summary><strong>Core Networking</strong></summary>
<br>

| Feature | Detail |
|---|---|
| **Dual-mode Wi-Fi** | STA + AP simultaneously on a single radio |
| **NAT / NAPT forwarding** | Full internet routing for all AP clients through the upstream link |
| **Up to 10 AP clients** | Configurable 1–10 (ESP32 hardware limit), default 8 |
| **TX power boosted** | `esp_wifi_set_max_tx_power(78)` → 19.5 dBm for maximum range |
| **Auto-reconnect** | Health task retries upstream every 30 s on disconnect |
| **Saved networks** | Store up to 5 upstream credentials; switch with one click |

</details>

<details>
<summary><strong>Captive Portal — Identity Beacon</strong></summary>
<br>

The captive portal is NodeX's signature feature — it turns every Wi-Fi connection into a **personal digital identity broadcast**.

- **OS-native popup triggers automatically** on Android, iOS, Windows, and macOS
- **Per-client authentication** — every device must authenticate before DNS is forwarded
- **Two access tiers** — Network code (browsing) or Admin code (full dashboard)
- **Configurable session timeout** — 1 to 1440 minutes, applied live without reboot
- **DNS interception** — unauthenticated clients redirected to portal; authenticated clients forwarded to `8.8.8.8`

**Portal login page:**
- Arch Linux-themed interactive terminal emulator
- Animated boot sequence: BIOS → kernel → systemd → login prompt
- `neofetch`-style ASCII header + portfolio sections
- Sections: About · Skills · Approach · Contact · Socials
- Commands: `help` `login:<code>` `whoami` `about` `skills` `contact` `wifi` `status` `ls` `pwd` `date` `uname` `clear`
- Command history via ↑ / ↓ arrow keys

**Post-login connected page:**
- Terminal-themed dark UI with `[✓]` status confirmation lines
- Large QR code + direct social link so exhibition visitors can follow you instantly
- Pure terminal aesthetic — no emojis, no gradients

</details>

<details>
<summary><strong>Web Admin Dashboard</strong></summary>
<br>

Dark terminal-themed responsive UI at `/admin`, protected by cookie-based session tokens (30-minute timeout) with HTTP Basic Auth fallback. Auto-refreshes every 5 seconds.

**Dashboard tab**
- Uptime · Free heap · Chip model · CPU frequency · Flash size
- STA RSSI animated signal bar (green / yellow / red)
- AP: SSID · IP · MAC · client count / max

**Clients tab**
- Live MAC + IP table with per-client portal session age
- **Deauth** — revokes session, forces re-login through captive portal
- **Kick** — deauthenticates the device from Wi-Fi entirely

**Networks tab**
- All saved upstream networks with **Use** / **Remove** controls
- **Scan** — live scan with SSID, RSSI, channel, and lock icon
- **Select** — auto-fills SSID without leaving the tab

**Config tab**
- Station upstream SSID / password (show/hide toggle)
- AP: SSID · channel · max clients · hidden SSID toggle
- Login codes · dashboard credentials · OTA password (all show/hide)
- Session timeout control — applies immediately, no reboot required

**Tools tab**
- Reboot · Factory Reset (erases NVS) · OTA info

</details>

<details>
<summary><strong>Status LED</strong></summary>
<br>

PWM-driven on GPIO 2 — no discrete blink, smooth hardware PWM curves.

| Pattern | Meaning |
|---|---|
| **Breathe** — smooth pulse | Healthy: STA connected, no AP clients |
| **Slow blink** — 1 s on/off | Active: one or more clients connected to AP |
| **Fast blink** — 200 ms | Error: STA disconnected, no upstream |
| **Triple quick flash** | Event: new client connection detected |
| **Off** | System not yet initialized |

</details>

<details>
<summary><strong>OTA Updates</strong></summary>
<br>

- ArduinoOTA over Wi-Fi — no USB cable required after first flash
- Hostname: `NodeX` · Port: `3232`
- Password configurable from the dashboard
- Flash progress streamed to Serial

</details>

<details>
<summary><strong>Configuration Persistence</strong></summary>
<br>

- All settings stored in **ESP32 NVS** — survives reboots and power loss
- Thread-safe recursive FreeRTOS mutex for dual-core concurrent access
- Factory reset: hold boot button LOW for 3 seconds during power-on, or trigger via `/api/factory-reset`

</details>

---

## Project Structure

```
Node_X/
├── platformio.ini              # Build config, dependencies, partition table
├── index.html                  # Development preview of the dashboard
├── HARDWARE.md                 # Hardware setup & deployment guide
├── README.md                   # This file
├── include/
│   ├── nodex_defaults.h        # Compile-time defaults (SSID, passwords, timeouts)
│   ├── nodex_pins.h            # GPIO pin definitions
│   ├── config_manager.h        # NVS config struct + persistence API
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

NodeX is designed to run on a **compact, keychain-friendly ESP32 module** — no extra pins, no breadboard required.

### Recommended Boards

| Board | Notes |
|---|---|
| **ESP32-C3 SuperMini** | Best for keychain form factor — stamp-sized, USB-C |
| **ESP32-S3 Zero** | Compact, powerful, USB-C native |
| **ESP32-DevKitC V4** | Best for development and prototyping |
| **ESP32-WROOM-32D** | Production module; needs USB-UART adapter for first flash |
| **ESP32-WROVER-B** | Extra PSRAM — ideal for >6 concurrent clients |

### GPIO Pinout (default)

```
GPIO 2  → Onboard status LED  (breathing = healthy, fast blink = error)
GPIO 0  → Factory reset button (hold LOW for 3 s on boot)
GPIO 4  → (Optional) External activity LED
```

Edit `include/nodex_pins.h` to remap any pin.

### Power

| Source | Requirement |
|---|---|
| USB 5V | 1A minimum (2A recommended) |
| Regulated 3.3V | Direct to 3V3 pin, 600 mA minimum |
| Battery (LiPo) | 3.7V + boost to 5V → USB input |

> Avoid powering from a PC USB port under heavy Wi-Fi TX — brownouts cause random resets.
> Add 100 µF electrolytic + 100 nF ceramic capacitor across the 3.3V rail for stability.

> Full deployment guide, placement tips, and production checklist: [HARDWARE.md](HARDWARE.md)

---

## Quick Start

### Prerequisites
- [VS Code](https://code.visualstudio.com/) + [PlatformIO extension](https://platformio.org/)
- Any ESP32 module + USB cable (USB-C or micro depending on board)

### 1 — Clone & Open
```bash
git clone <repo-url> Node_X
cd Node_X
code .
```

### 2 — (Optional) Pre-configure Wi-Fi
Edit `include/nodex_defaults.h` before flashing:
```cpp
#define DEFAULT_STA_SSID  "YourUpstreamWiFi"
#define DEFAULT_STA_PASS  "YourPassword"
```
Or leave blank and configure via the web dashboard after first boot.

### 3 — Flash
```bash
pio run -t upload
pio device monitor -b 115200
```
Set the correct port in `platformio.ini` → `upload_port = COM7` (Windows) or `/dev/ttyUSB0` (Linux/macOS).

### 4 — Connect & Configure
1. Connect to Wi-Fi **`NodeX`** (open network)
2. Captive portal pops up automatically — or open `http://192.168.4.1`
3. Type `login: sumnima` in the terminal prompt to reach the admin dashboard
4. **Config tab** → enter upstream Wi-Fi credentials → **Save**
5. **Tools tab** → **Reboot**
6. NodeX connects upstream, LED starts breathing — ready to deploy
---

## Default Credentials

| Setting | Default | Change via |
|---|---|---|
| AP SSID | `NodeX` | Config tab |
| Network login code | `dev` | Config tab |
| Admin login code | `sumnima` | Config tab |
| Dashboard username | `admin` | Config tab |
| Dashboard password | `admin` | Config tab |
| OTA password | `nodex_ota` | Config tab |
| Client session timeout | `60 minutes` | Config tab |

> **Always change credentials before any public deployment.**

---

## REST API

All endpoints (except the portal) require admin auth via **cookie session** or **HTTP Basic Auth** (`admin` / `admin`).

### Endpoints

| Method | Endpoint | Auth | Description |
|---|---|---|---|
| `GET` | `/` | — | Captive portal login page |
| `POST` | `/portal/login` | — | Authenticate with code |
| `GET` | `/connected` | — | Post-login QR / identity page |
| `POST` | `/portal/logout` | — | Clear admin session |
| `GET` | `/admin` | ✓ | Web admin dashboard |
| `GET` | `/api/status` | ✓ | System info · STA/AP details · RSSI · heap |
| `GET` | `/api/clients` | ✓ | Connected AP client list (MAC + IP) |
| `GET` | `/api/scan` | ✓ | Trigger / fetch Wi-Fi network scan |
| `GET` | `/api/config` | ✓ | Current config (passwords excluded) |
| `POST` | `/api/config` | ✓ | Save configuration (JSON body) |
| `POST` | `/api/reboot` | ✓ | Reboot device |
| `POST` | `/api/factory-reset` | ✓ | Erase NVS config and reboot |
| `GET` | `/api/sessions` | ✓ | Active captive portal sessions |
| `POST` | `/api/sessions/deauth` | ✓ | Revoke a portal session by IP |
| `POST` | `/api/clients/kick` | ✓ | Kick a client by MAC address |
| `GET` | `/api/networks` | ✓ | List saved upstream networks |
| `POST` | `/api/networks/add` | ✓ | Save new network credentials |
| `POST` | `/api/networks/remove` | ✓ | Remove saved network by index |
| `POST` | `/api/networks/connect` | ✓ | Switch to a saved network (reboots) |

### Example Calls

```bash
# Save upstream Wi-Fi
curl -u admin:admin -X POST http://192.168.4.1/api/config \
  -H "Content-Type: application/json" \
  -d '{"staSSID":"MyNetwork","staPass":"secret123"}'

# Set client session timeout to 30 minutes
curl -u admin:admin -X POST http://192.168.4.1/api/config \
  -H "Content-Type: application/json" \
  -d '{"sessionTimeout":30}'

# Kick a client by MAC
curl -u admin:admin -X POST http://192.168.4.1/api/clients/kick \
  -d 'mac=AA:BB:CC:DD:EE:FF'

# Trigger Wi-Fi scan, then fetch results
curl -u admin:admin http://192.168.4.1/api/scan
sleep 5
curl -u admin:admin http://192.168.4.1/api/scan
```

---

## Configuration Reference

All writable fields for `POST /api/config` (JSON body):

| Field | Type | Description |
|---|---|---|
| `staSSID` | string | Upstream Wi-Fi name |
| `staPass` | string | Upstream Wi-Fi password |
| `apSSID` | string | NodeX broadcast SSID |
| `apPass` | string | Network portal login code |
| `adminCode` | string | Admin portal access code |
| `apChannel` | int 1–13 | Wi-Fi channel |
| `apMaxClients` | int 1–10 | Max simultaneous AP clients |
| `apHidden` | bool | Hide SSID from scan results |
| `webUser` | string | Dashboard API username |
| `webPass` | string | Dashboard API password |
| `otaPass` | string | OTA firmware update password |
| `sessionTimeout` | int 1–1440 | Client session duration in minutes |

---

## Architecture

```
┌──────────────────────────────────────────────────────────┐
│                        ESP32 SoC                          │
│                                                           │
│   Core 0 — Event / Network          Core 1 — App          │
│  ┌────────────────────────┐        ┌──────────────────┐   │
│  │   ESPAsyncWebServer    │        │   taskHealth     │   │
│  │   · REST API           │        │   · STA reconnect│   │
│  │   · Admin dashboard    │        │   · NAT re-enable│   │
│  │   · Captive portal     │        │   · LED state    │   │
│  │   · OTA endpoint       │        │   · Client count │   │
│  └────────────────────────┘        ├──────────────────┤   │
│                                    │   taskOTA        │   │
│  ┌────────────────────────┐        │   · ArduinoOTA   │   │
│  │   taskDNS              │        ├──────────────────┤   │
│  │   · DNS intercept      │        │   taskLED        │   │
│  │   · DNS forward        │        │   · PWM patterns │   │
│  └────────────────────────┘        └──────────────────┘   │
│                                                           │
│  ┌─────────────────────────────────────────────────────┐  │
│  │              Wi-Fi Stack  (STA + AP + NAPT)          │  │
│  │   Router ←──[STA]─── ESP32 ───[AP]──→ Clients        │  │
│  │                         └──── NAPT forwarding ────┘  │  │
│  └─────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────┘
```

### FreeRTOS Task Summary

| Task | Core | Priority | Stack | Responsibility |
|---|---|---|---|---|
| `taskHealth` | 1 | 2 | 3072 B | STA reconnect · NAT · LED state · client tracking |
| `taskLED` | 1 | 1 | 2048 B | LED PWM update every 20 ms |
| `taskOTA` | 1 | 3 | 8192 B | ArduinoOTA polling every 50 ms |
| `taskDNS` | 0 | 2 | 4096 B | DNS redirect / forward (blocking loop) |
| AsyncWebServer | 0 | — | — | HTTP requests (managed by library) |

---

## Performance

| Metric | Typical |
|---|---|
| Throughput | 20–50 Mbps TCP (signal-dependent) |
| Added latency | 2–5 ms per hop |
| Max concurrent clients | 8 (configurable up to 10) |
| Indoor range | 15–30 m with onboard antenna |
| Current draw | 160–260 mA @ 3.3V (TX bursts higher) |
| Boot to AP ready | ~3 seconds |

---

## License

MIT — free to use, modify, and distribute.

---

<div align="center">

Built with obsession by **[sakshyam](https://www.instagram.com/sakxam_console.log/)**
*Embedded Systems Engineer · Kathmandu, Nepal*

</div>


