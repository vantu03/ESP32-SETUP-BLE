# ESP32 WiFi Setup via BLE

This project enables WiFi configuration on an ESP32 device using Bluetooth Low Energy (BLE). It allows users to send WiFi credentials over BLE, which the ESP32 will use to connect to a network.

## 🚀 Features

- 📡 Configure WiFi credentials via BLE
- 🔄 Clear saved WiFi settings
- 🔧 Reconnect or reconfigure via BLE if no WiFi is found
- ✅ Suitable for headless IoT devices (no display/input)

## 🧠 How It Works

1. ESP32 boots and starts BLE service.
2. User sends a string like `WIFI:SSID,PASSWORD` to ESP32 via BLE.
3. ESP32 parses and saves the WiFi credentials.
4. It tries to connect to the WiFi network.
5. If successful, BLE service stops. Otherwise, it stays in BLE mode.

## 📂 Project Structure

```

ESP32-SETUP-BLE/
├── main.ino           # Main Arduino code
├── LED.h              # (Optional) Control for onboard LED
└── WiFiManager.h      # BLE + WiFi logic

```

## 📲 Example BLE Payload

Send the following via any BLE app (e.g., nRF Connect):

```

WIFI\:MyNetwork,MyPassword123

````

## 🔧 Requirements

- ESP32 Dev Board
- Arduino IDE (tested on v2.3.6)
- ESP32 board package installed
- BLE app on your smartphone (e.g. nRF Connect)

## 📥 Installation

1. Clone this repository:
```bash
   git clone https://github.com/vantu03/ESP32-SETUP-BLE.git
```

2. Open main.ino with Arduino IDE.
3. Select the correct board (ESP32 Dev Module) and COM port.
4. Upload the sketch.

## 🧪 Debugging

Use *Serial Monitor* to view BLE payloads and connection status.

## 📜 License

This project is open-source under the MIT License.