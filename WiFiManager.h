#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>
#include <Preferences.h>

#define RESET_BUTTON 0         // GPIO0
#define RESET_HOLD_TIME 3000   // 3 giây (ms)

class WiFiManager {
private:
  bool isConnecting = false;
  unsigned long lasttime;
  unsigned long timeout;
  
  String ssid;
  String password;
  bool wifiReady;

  Preferences prefs;

  BLECharacteristic* pCharacteristic = nullptr;
  BLECharacteristic* statusChar = nullptr;

public:
  unsigned long pressedTime = 0;
  bool waitingForRelease = false;

public:
  void begin() {
    pinMode(RESET_BUTTON, INPUT_PULLUP);
    wifiReady = false;
    timeout = 0;

    if (isHaveWifi()) {
      
      prefs.begin("wifi", true);
      ssid = prefs.getString("ssid", "");
      password = prefs.getString("pass", "");
      prefs.end();

      wifiReady = true;
      Serial.printf("[WiFi] Đã tìm thấy cấu hình cũ: %s\n", ssid.c_str());
    } else {
      startBLE();  // nếu chưa có, bật BLE
    }

    lasttime = millis();
  }

  void startBLE() {
    Serial.println("[BLE] Bắt đầu chế độ cấu hình...");
    BLEDevice::init("ESP32-CONFIG");
    BLEServer* pServer = BLEDevice::createServer();

    BLEService* pService = pServer->createService("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
    pCharacteristic = pService->createCharacteristic(
      "6e400003-b5a3-f393-e0a9-e50e24dcca9e",
      BLECharacteristic::PROPERTY_WRITE
    );
    statusChar = pService->createCharacteristic(
      "6e400004-b5a3-f393-e0a9-e50e24dcca9e",
      BLECharacteristic::PROPERTY_NOTIFY
    );

    pCharacteristic->setCallbacks(new InternalCallback(this));
    pService->start();

    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->start();

    Serial.println("[BLE] Đang chờ cấu hình WiFi...");
  }

  void update() {
    unsigned long now = millis();

    if (wifiReady && !isConnecting && WiFi.status() != WL_CONNECTED) {
      timeout = 15000;
      isConnecting = true;
      Serial.printf("[WiFi] Đang kết nối tới %s...\n", ssid.c_str());
      WiFi.begin(ssid.c_str(), password.c_str());
    }

    if (isConnecting) {
      if (WiFi.status() == WL_CONNECTED) {
        Serial.print("[WiFi] Kết nối thành công! IP: ");
        Serial.println(WiFi.localIP());
        isConnecting = false;

        // Lưu lại nếu là kết nối qua BLE mới
        prefs.begin("wifi", false);
        prefs.putString("ssid", ssid);
        prefs.putString("pass", password);
        prefs.end();
        if (statusChar) {
          String json = "{\"status\":\"ok\",\"ip\":\"" + WiFi.localIP().toString() + "\"}";
          statusChar->setValue(json.c_str());
          statusChar->notify();
        }
      } else if (timeout > 0) {
        timeout -= now - lasttime;
        if (timeout <= 0) {
          Serial.println("[WiFi] Kết nối thất bại (timeout).");
          isConnecting = false;
          wifiReady = false;
          if (statusChar) {
            statusChar->setValue("{\"status\":\"fail\"}");
            statusChar->notify();
          }

          startBLE();  // Cho phép cấu hình lại
        }
      }
    }
    if (digitalRead(RESET_BUTTON) == LOW) {
      if (!waitingForRelease) {
        pressedTime = millis();  // bắt đầu đếm thời gian giữ
        waitingForRelease = true;
      } else if (millis() - pressedTime >= RESET_HOLD_TIME) {
        Serial.println("Nhấn giữ đủ lâu – reset WiFi!");
        reset();
        waitingForRelease = false;
      }
    } else {
      // Nút đã nhả, reset biến
      waitingForRelease = false;
    }
    lasttime = millis();
  }

  bool isWifiReady() const {
    return wifiReady;
  }

  bool isOnline() const {
    return WiFi.status() == WL_CONNECTED;
  }
  
  bool isHaveWifi() {
    prefs.begin("wifi", true);
    String s = prefs.getString("ssid", "");
    String p = prefs.getString("pass", "");
    prefs.end();
    return s.length() > 0 && p.length() > 0;
  }

  void reset() {
    prefs.begin("wifi", false);
    prefs.clear();
    prefs.end();

    // Ngắt WiFi sạch
    WiFi.disconnect(true);

    // Reset trạng thái
    ssid = "";
    password = "";
    wifiReady = false;
    isConnecting = false;

    startBLE();
  }


private:
  class InternalCallback : public BLECharacteristicCallbacks {
  public:
    InternalCallback(WiFiManager* parent) : parent(parent) {}

    void onWrite(BLECharacteristic* pChar) override {
      std::string value = std::string(pChar->getValue().c_str());

      if (!parent->isHaveWifi() && value.length() > 0 && value.find("WIFI:") == 0) {
        Serial.printf("[BLE] Nhận WiFi config: %s\n", value.c_str());

        std::string data = value.substr(5); // Bỏ "WIFI:"
        size_t sep = data.find("|PASS:");
        if (sep != std::string::npos) {
          parent->ssid = String(data.substr(0, sep).c_str());
          parent->password = String(data.substr(sep + 6).c_str());
          parent->wifiReady = true;
        }
      }
    }

  private:
    WiFiManager* parent;
  };
};
