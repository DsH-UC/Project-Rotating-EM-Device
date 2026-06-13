#include <Arduino.h>
#include <NimBLEDevice.h>
#include <esp_now.h>
#include <WiFi.h>

// STM32 pin connection
#define STM32_RX_PIN 4   // ESP32-C3 RX pin from STM32 TX
#define STM32_TX_PIN 5   // ESP32-C3 TX pin to STM32 RX
#define STM32_BAUD   115200

// UART Service UUIDs
static NimBLECharacteristic* txCharacteristic;
static bool deviceConnected = false;

static const char* UART_SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
static const char* UART_RX_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"; // The laptop writes to this
static const char* UART_TX_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"; // ESP32 notification here

// ESP-NOW Packet structure
constexpr uint8_t NUM_THERMISTORS = 8;
struct RotatingThermistorPacket { // Packet order index and temp 
    uint32_t sequence;
    float temp_c[NUM_THERMISTORS];
    float v_adc[NUM_THERMISTORS];
    uint16_t raw[NUM_THERMISTORS];
};

// Callback functions for BLE
class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* server, NimBLEConnInfo& connInfo) override {
        deviceConnected = true;
        Serial.println("BLE client connected");
    }

    void onDisconnect(NimBLEServer* server, NimBLEConnInfo& connInfo, int reason) override {
        deviceConnected = false;
        Serial.println("BLE client disconnected");
        NimBLEDevice::startAdvertising();
    }
};

class RxCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* characteristic, NimBLEConnInfo& connInfo) override {
        std::string value = characteristic->getValue();

        //for (char c : value) Serial1.write(c);  // Send command to STM32 UART over BLE
        Serial1.write((const uint8_t*)value.data(), value.length());
    
        Serial.print("BLE RX -> STM32 UART: ");
        Serial.println(value.c_str());
    }
};

// Callback functions for ESP-NOW
void on_espnow_recv(const esp_now_recv_info_t* info, const uint8_t* incomingData, int len) {
    if (len != sizeof(RotatingThermistorPacket)) {
        Serial.print("Bad ESP-NOW packet length: ");
        Serial.println(len);
        Serial.print(" expected=");
        Serial.println(sizeof(RotatingThermistorPacket));
        return;
    }

    RotatingThermistorPacket packet;
    memcpy(&packet, incomingData, sizeof(packet));

    // Send rotating thermistor telemetry to STM32
    char msg[160];
    // snprintf(msg, sizeof(msg), "RTEMP seq=%lu temp=%.2f raw=%u v=%.3f\r\n", (unsigned long)packet.sequence,
    //          packet.temp_c, packet.raw, packet.v_adc);
    snprintf(msg, sizeof(msg),
        "RTEMP seq=%lu t0=%.2f t1=%.2f t2=%.2f t3=%.2f t4=%.2f t5=%.2f\r\n",
        (unsigned long)packet.sequence,
        packet.temp_c[0],
        packet.temp_c[1],
        packet.temp_c[2],
        packet.temp_c[3],
        packet.temp_c[4],
        packet.temp_c[5]
        );
    Serial1.print(msg);

    // Send message directly to GUI over BLE notification
    if (deviceConnected && txCharacteristic != nullptr) {
        txCharacteristic->setValue(msg);
        txCharacteristic->notify();
    }

    Serial.print("ESP-NOW RX -> "); // USB debug print
    Serial.print(msg);
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("Booting ESP32-C3 BLE bridge");
    Serial1.begin(STM32_BAUD, SERIAL_8N1, STM32_RX_PIN, STM32_TX_PIN);

    // ESP-NOW setup
    WiFi.mode(WIFI_STA); // Station mode

    Serial.print("Main ESP32C3 MAC Address: "); // MAC Address for debug
    Serial.println(WiFi.macAddress());

    if (esp_now_init() != ESP_OK) Serial.println("ESP-NOW init failed");
    else {
        esp_now_register_recv_cb(on_espnow_recv);
        Serial.println("ESP-NOW receiver started");
    }

    // BLE w/ UART setup
    NimBLEDevice::init("ESP32C3_STM32_BRIDGE");

    NimBLEServer* server = NimBLEDevice::createServer();
    server->setCallbacks(new ServerCallbacks());

    NimBLEService* uartService = server->createService(UART_SERVICE_UUID);

    txCharacteristic = uartService->createCharacteristic(UART_TX_UUID, NIMBLE_PROPERTY::NOTIFY);

    NimBLECharacteristic* rxCharacteristic = uartService->createCharacteristic(
      UART_RX_UUID, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
    );

    rxCharacteristic->setCallbacks(new RxCallbacks());

    uartService->start();

    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();

    NimBLEAdvertisementData advData;
    advData.setName("ESP32C3_STM32_BRIDGE");
    advData.setCompleteServices(NimBLEUUID(UART_SERVICE_UUID));

    NimBLEAdvertisementData scanData;
    scanData.setName("ESP32C3_STM32_BRIDGE");

    advertising->setAdvertisementData(advData);
    advertising->setScanResponseData(scanData);

    advertising->start();

    Serial.println("BLE UART bridge has started");
}

void loop() {
    // Print before stm32 connection
    // static uint32_t last = 0;


    // if (millis() - last > 1000) {
    //     last = millis();
    //     Serial.println("ESP32 alive");
    // }
    // Optional: forward STM32 UART responses back to laptop over BLE
    if (deviceConnected && Serial1.available()) {
        String response;

        while (Serial1.available()) { response += (char)Serial1.read(); }

        if (response.length() > 0) {
            txCharacteristic->setValue(response.c_str());
            txCharacteristic->notify();
            
            //Serial.print("STM32 UART -> BLE: "); // Debug print in addition to UART print
            //Serial.print(response);
        }
    }
    delay(10);
}