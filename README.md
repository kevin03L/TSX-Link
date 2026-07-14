# TSX-Link

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Ready-brightgreen.svg)](https://platformio.org/)
[![Docs](https://img.shields.io/badge/Docs-Latest-blue.svg)](docs/)

**TSX-Link** is an Open Source industrial gateway that connects Schneider Electric TSX Micro PLC (TSX3721001) to modern IoT ecosystems via Wi-Fi and Ethernet.

## 🎯 Features

### Connectivity
- **Wi-Fi 802.11 b/g/n** - 2.4 GHz dual-mode (STA/AP)
- **Ethernet** - 10/100 Mbps via LAN8720A PHY
- **Uni-Telway Protocol** - Native TER Mini-DIN port communication
- **RS485** - Industrial-grade isolated serial communication

### Integration Protocols
- **MQTT** - Full-featured with Home Assistant Auto Discovery
- **Modbus TCP** - Complete TCP server implementation
- **REST API** - Modern HTTPS/HTTP endpoints
- **WebSocket** - Real-time bidirectional communication
- **Syslog** - Remote logging support

### Features
- **OTA Updates** - Secure over-the-air firmware updates
- **Dashboard** - Modern responsive web interface
- **Configuration Management** - Persistent JSON-based configuration
- **Event Logging** - Comprehensive system and event logs
- **Watchdog** - Automatic recovery from system failures
- **mDNS/DNS** - Network discovery and resolution
- **NTP** - Time synchronization
- **Authentication** - User management and HTTPS
- **Home Assistant Integration** - Automatic discovery and integration

### Hardware Features
- **ESP32-S3-WROOM-1** - High-performance dual-core MCU
- **Isolated RS485** - 10 kV isolation via optocouplers
- **24V Input** - Industrial power supply compatibility
- **ESD/EMI Protection** - Industry-standard protections
- **DIN Rail Mount** - Standard industrial enclosure
- **Status LEDs** - Power, Wi-Fi, Ethernet, Activity indicators

## 📦 Quick Start

### Prerequisites

- ESP-IDF v5.0 or later
- PlatformIO CLI
- Python 3.8+
- KiCad 8.0+ (for hardware modifications)
- FreeCAD (for enclosure design)

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/kevin03L/TSX-Link.git
   cd TSX-Link
   ```

2. **Install firmware dependencies**
   ```bash
   cd firmware
   pio pkg install
   ```

3. **Build the firmware**
   ```bash
   pio run -e esp32s3
   ```

4. **Flash to device**
   ```bash
   pio run -e esp32s3 -t upload
   ```

5. **Monitor output**
   ```bash
   pio device monitor -b 115200
   ```

## 📋 Project Structure

```
TSX-Link/
├── firmware/                # ESP-IDF/PlatformIO firmware
│   ├── src/
│   ├── include/
│   ├── components/         # Reusable driver components
│   └── platformio.ini
├── hardware/                # KiCad PCB design
│   ├── tsx-link.kicad_pro
│   ├── schematics/
│   ├── pcb/
│   ├── symbols/
│   ├── footprints/
│   ├── bom/
│   └── gerbers/
├── web/                     # Web interface (Vue 3 + TypeScript)
│   ├── src/
│   ├── public/
│   └── package.json
├── docs/                    # Complete documentation
│   ├── user/
│   ├── developer/
│   ├── hardware/
│   └── api/
├── enclosure/               # FreeCAD 3D design
│   ├── tsx-link.FCStd
│   ├── stl/
│   └── step/
├── tests/                   # Test suites
│   ├── unit/
│   ├── integration/
│   └── hardware/
├── examples/                # Usage examples
│   ├── mqtt/
│   ├── modbus/
│   ├── rest-api/
│   └── home-assistant/
├── scripts/                 # Build and utility scripts
└── .github/                 # GitHub Actions CI/CD
```

## 🔌 Hardware Specifications

### Microcontroller
- **Chipset**: ESP32-S3-WROOM-1 (240 MHz dual-core)
- **RAM**: 8 MB PSRAM
- **Flash**: 4/8 MB internal + microSD optional
- **Interfaces**: SPI, I2C, UART, USB 2.0 FS

### Connectivity
- **Wi-Fi**: IEEE 802.11 b/g/n, 20 MHz channel bandwidth
- **Ethernet**: 10/100 Mbps via LAN8720A (RMII interface)
- **Serial**: TER Mini-DIN (Uni-Telway protocol)
- **RS485**: Isolated via MAX485 + 6N137 optocoupler

### Power
- **Input**: 24V DC ±10% (industrial standard)
- **Converter**: MP2307 buck 24V→5V (2A)
- **LDO**: AMS1117-3.3 for 3.3V rail
- **Consumption**: ~500 mA typical (Wi-Fi active)

### Protection
- **Input**: Polyfuse PTC 2A + D1N4007 reverse protection
- **Transient**: TVS diodes on all I/O (5V1 SMD)
- **Isolation**: 6N137 optocouplers on RS485
- **ESD**: ESD7009 arrays on sensitive pins

### Enclosure
- **Form Factor**: DIN C45/35
- **Mounting**: Top-hat DIN rail
- **Dimensions**: 90×90×60 mm
- **Material**: Polycarbonate IP65 rated
- **Indicators**: 4× 3mm LEDs (Power, Wi-Fi, Ethernet, Activity)

## 🚀 Getting Started

### First Boot

1. Power on the device (24V DC input)
2. Device will boot into **Setup Mode** with:
   - AP SSID: `TSX-Link-XXXX` (random MAC suffix)
   - Default password: `tsx-link-initial`
3. Connect to the AP and access: `http://192.168.4.1`
4. Configure Wi-Fi and device parameters
5. Device reboots and connects to your network

### Web Dashboard

Once connected to network:
- Open `http://<device-ip>` or `http://tsx-link.local`
- Default credentials: `admin` / `admin` (change immediately!)
- Browse variables, logs, and configure settings

### MQTT Integration

1. Go to Dashboard → Configuration → MQTT
2. Enter broker IP and credentials
3. Enable Auto Discovery for Home Assistant
4. Home Assistant will auto-detect the device

### Modbus TCP

The device acts as a Modbus TCP server:
- **Address**: Device IP
- **Port**: 502
- **Slave ID**: 1
- **Holding Registers**: 0-255 (PLC memory map)

### REST API

```bash
# Get device status
curl -H "Authorization: Bearer TOKEN" https://tsx-link.local/api/status

# Read PLC memory
curl https://tsx-link.local/api/memory?address=0&size=16

# Write to PLC
curl -X POST https://tsx-link.local/api/write -d '{"address": 0, "value": 42}'
```

## 📚 Documentation

Full documentation available in the `docs/` folder:

- **[User Guide](docs/user/README.md)** - Setup, configuration, operation
- **[Developer Guide](docs/developer/README.md)** - Architecture, extending, testing
- **[Hardware Guide](docs/hardware/README.md)** - Schematics, PCB, BOM, assembly
- **[API Reference](docs/api/REST-API.md)** - Complete REST API endpoints
- **[MQTT Guide](docs/integration/MQTT.md)** - MQTT topics and payloads
- **[Modbus Guide](docs/integration/Modbus-TCP.md)** - Register mapping
- **[Home Assistant Guide](docs/integration/Home-Assistant.md)** - HA integration
- **[OTA Guide](docs/operations/OTA.md)** - Firmware updates
- **[Security Guide](docs/operations/Security.md)** - Hardening and best practices
- **[Troubleshooting](docs/operations/Troubleshooting.md)** - Common issues

## 🛠️ Development

### Building from Source

```bash
# Setup development environment
cd firmware
pio pkg install

# Build firmware
pio run -e esp32s3

# Run tests
pio test -e esp32s3

# Generate documentation
cd ../docs
make html
```

### Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on:
- Code style and conventions
- Testing requirements
- Commit message format
- Pull request process

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🤝 Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md).

## 🐛 Bug Reports

Found a bug? Please create an issue with:
- Detailed description
- Steps to reproduce
- Expected vs actual behavior
- System information (firmware version, hardware revision, etc.)

## 📞 Support

- **Issues**: https://github.com/kevin03L/TSX-Link/issues
- **Discussions**: https://github.com/kevin03L/TSX-Link/discussions
- **Security**: See [SECURITY.md](SECURITY.md)

## 🙏 Acknowledgments

- Schneider Electric for the TSX Micro PLC ecosystem
- Espressif for ESP-IDF and ESP32-S3
- Home Assistant community for MQTT Discovery standard
- Open source community for excellent libraries and tools

---

**Made with ❤️ for industrial automation**
