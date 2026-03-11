# NodeX — Hardware Setup & Deployment Guide

## Recommended Hardware

### ESP32 Dev Boards (tested & recommended)
| Board | Notes |
|---|---|
| **ESP32-DevKitC V4** | Best all-around; onboard USB-UART, LED on GPIO2 |
| **ESP32-WROOM-32D** | Production module; needs external USB-UART for flashing |
| **ESP32-WROVER-B** | Extra PSRAM — recommended for >6 concurrent clients |
| **ESP-32S NodeMCU** | Budget-friendly; works perfectly for home use |

### Power Supply
- **USB 5V / 1A minimum** — ESP32 peaks at ~500mA during Wi-Fi TX bursts
- For production: use a quality **5V/2A** USB adapter or a regulated 3.3V supply
- Avoid powering from a PC USB port under heavy load — brownouts cause resets

### Optional Components
| Component | Purpose | Pin |
|---|---|---|
| External LED | Activity indicator | GPIO 4 |
| Tactile button | Factory reset (hold 3s on boot) | GPIO 0 (boot button works) |
| External antenna | Extended range (+6–8 dBi) | U.FL connector if board supports |

---

## Pin Mapping (default)

```
GPIO 2  → Onboard status LED (breathing = OK, fast blink = error)
GPIO 0  → Factory reset button (hold LOW 3s during boot)
GPIO 4  → (Optional) External activity LED
```

Edit `include/nodex_pins.h` to change pin assignments.

---

## Flashing Instructions

### PlatformIO (recommended)
```bash
# First-time flash via USB:
pio run -t upload --upload-port COM3    # Windows
pio run -t upload --upload-port /dev/ttyUSB0  # Linux/Mac

# Monitor serial output:
pio device monitor -b 115200
```

### Arduino IDE
1. Install ESP32 board package: `https://dl.espressif.com/dl/package_esp32_index.json`
2. Select board: **ESP32 Dev Module**
3. Partition scheme: **Minimal SPIFFS (1.9MB APP with OTA)**
4. Upload speed: **921600**
5. Install libraries: ArduinoJson, ESPAsyncWebServer, AsyncTCP, lwip_nat_lib

### OTA (subsequent updates)
```bash
# Uncomment OTA section in platformio.ini, then:
pio run -t upload
# Or from Arduino IDE: Tools → Port → NodeX at 192.168.4.1
```

---

## Optimal Placement

```
                     ┌─────────────┐
        Strong       │   Upstream  │      Weak/No
        Signal       │   Router    │      Signal
       ◄────────────►│             │◄────────────►
                     └──────┬──────┘
                            │
              40–70% signal │ zone
               (ideal spot) │
                            │
                     ┌──────┴──────┐
                     │   NodeX     │
                     │  Repeater   │
                     └──────┬──────┘
                            │
                 Rebroadcast│coverage
                            │
                     ┌──────┴──────┐
                     │   Client    │
                     │  Devices    │
                     └─────────────┘
```

### Placement Rules
1. **Place NodeX where upstream signal is -50 to -70 dBm** (check via dashboard)
2. **Elevate the board** — even 1–2 meters above floor improves range significantly
3. **Avoid metal enclosures** — use plastic or 3D-printed cases
4. **Keep away from** microwaves, Bluetooth speakers, baby monitors (2.4 GHz interference)
5. **Line of sight** to both router and client area is ideal

---

## Production Reliability Checklist

### Power
- [ ] Use regulated 5V/2A supply with overcurrent protection
- [ ] Add 100µF electrolytic + 100nF ceramic capacitor across 3.3V rail
- [ ] Consider UPS or battery backup for critical deployments

### Thermal
- [ ] ESP32 safe operating range: -40°C to +85°C
- [ ] In enclosed cases, add ventilation holes or a small heatsink
- [ ] Monitor heap via dashboard — memory leaks indicate thermal throttling

### Wi-Fi
- [ ] Disable Wi-Fi power save (already done in firmware)
- [ ] Set AP channel to match upstream router's channel for best performance
- [ ] For outdoor use, consider ESP32 boards with U.FL + external antenna
- [ ] Keep max clients ≤ 8 for stable throughput per client

### Firmware
- [ ] Change default web dashboard password before deployment
- [ ] Change OTA password from default
- [ ] Test OTA update path before physical installation
- [ ] Enable watchdog (handled by FreeRTOS kernel automatically)

### Network Security
- [ ] Use WPA2 with strong AP password (≥12 chars recommended)
- [ ] Dashboard uses HTTP Basic Auth — access only on local network
- [ ] For public deployments, consider hiding the AP SSID
- [ ] Periodically update firmware for security patches

---

## Troubleshooting

| Symptom | Cause | Fix |
|---|---|---|
| No AP visible | AP not started / crash | Check serial log; reflash |
| AP visible but no internet | STA not connected | Configure STA SSID via dashboard |
| Slow speeds | Poor placement / too many clients | Move closer to router; reduce max clients |
| Frequent disconnects | Power supply issue | Use better 5V supply; add capacitors |
| Can't access dashboard | Wrong IP | Default: `192.168.4.1` |
| OTA fails | Wrong password / network | Verify OTA password; ensure on same network |
| Factory reset needed | Forgotten password | Hold GPIO0 LOW for 3s during boot |

---

## Estimated Performance

| Metric | Typical Value |
|---|---|
| **Throughput** | 20–50 Mbps (TCP), depending on signal |
| **Latency overhead** | 2–5 ms added hop |
| **Max concurrent clients** | 8 (configurable, ESP32 hardware limit: 10) |
| **Range extension** | 15–30 meters indoor with onboard antenna |
| **Power consumption** | 160–260 mA @ 3.3V (TX bursts peak higher) |
| **Boot time** | ~3 seconds to AP ready |
