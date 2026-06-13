#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <math.h>

#define THERM_ADC_PIN 0

#define MUX_A_PIN 6
#define MUX_B_PIN 7
#define MUX_C_PIN 8

constexpr float THERM_VSUPPLY = 3.33f;   // measured ESP32 3.3V rail
constexpr float THERM_R_FIXED = 10000.0f;
constexpr float THERM_R25     = 10000.0f;
constexpr float THERM_BETA    = 3950.0f;
constexpr float THERM_T0_K    = 298.15f;

constexpr int NUM_SAMPLES = 32;
constexpr uint32_t SEND_PERIOD_MS = 500;

constexpr uint8_t NUM_THERMISTORS = 8;


//V Divider = VSUPPLY --- 10k fixed --- ADC node --- thermistor --- GND

// ESP-NOW setup
uint8_t MAIN_ESP32_MAC[] = {0xB0, 0xA6, 0x04, 0x5C, 0x88, 0x64};

struct RotatingThermistorPacket {
    uint32_t sequence;
    float temp_c[NUM_THERMISTORS];
    float v_adc[NUM_THERMISTORS];
    uint16_t raw[NUM_THERMISTORS];
};

static uint32_t sequence_counter = 0;

// Mux selector
void select_mux_channel(uint8_t channel) {
    digitalWrite(MUX_A_PIN, (channel >> 0) & 0x01);
    digitalWrite(MUX_B_PIN, (channel >> 1) & 0x01);
    digitalWrite(MUX_C_PIN, (channel >> 2) & 0x01);
    delay(10); // Wait for Mux + RC filter + ADC input to settle
}

// Thermistor functions
// uint16_t read_adc_average() {
//     uint32_t sum = 0;

//     for (int i = 0; i < NUM_SAMPLES; i++) {
//         sum += analogRead(THERM_ADC_PIN);
//         delayMicroseconds(200);
//     }

//     return static_cast<uint16_t>(sum / NUM_SAMPLES);
// }

// float read_thermistor_c(uint16_t& raw_out, float& v_adc_out) {
//     raw_out = read_adc_average();
//     v_adc_out = counts_to_voltage(raw_out);

//     float r_therm = voltage_to_resistance(v_adc_out);
//     return resistance_to_temp_c(r_therm);
// }

uint16_t read_adc_average_for_channel(uint8_t mux_channel) {
    select_mux_channel(mux_channel);

    uint32_t sum = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        sum += analogRead(THERM_ADC_PIN);
        delayMicroseconds(200);
    }

    return static_cast<uint16_t>(sum / NUM_SAMPLES);
}

float counts_to_voltage(uint16_t counts){ return ((float)counts / 4095.0f) * THERM_VSUPPLY; }

float voltage_to_resistance(float v_adc) {
    if (v_adc <= 0.001f || v_adc >= (THERM_VSUPPLY - 0.001f)) return NAN;
    return THERM_R_FIXED * (v_adc / (THERM_VSUPPLY - v_adc));
}

float resistance_to_temp_c(float r_therm) {
    if (r_therm <= 0.0f || !isfinite(r_therm)) return NAN;
    
    float ln_ratio = logf(r_therm / THERM_R25);
    float inv_T = (1.0f / THERM_T0_K) + (ln_ratio / THERM_BETA);

    float temp_K = 1.0f / inv_T;
    return temp_K - 273.15f;
}

float read_thermistor_c(uint8_t mux_channel, uint16_t& raw_out, float& v_adc_out) {
    raw_out = read_adc_average_for_channel(mux_channel);
    v_adc_out = counts_to_voltage(raw_out);

    float r_therm = voltage_to_resistance(v_adc_out);
    return resistance_to_temp_c(r_therm);
}

void on_data_sent(const wifi_tx_info_t* tx_info, esp_now_send_status_t status) { // Callback for sent temperature packets
    Serial.print("ESP-NOW send status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "success" : "fail");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Initialize mux pins
    pinMode(MUX_A_PIN, OUTPUT);
    pinMode(MUX_B_PIN, OUTPUT);
    pinMode(MUX_C_PIN, OUTPUT);

    digitalWrite(MUX_A_PIN, LOW);
    digitalWrite(MUX_B_PIN, LOW);
    digitalWrite(MUX_C_PIN, LOW);

    // Initialize ADC  
    analogReadResolution(12);
    analogSetPinAttenuation(THERM_ADC_PIN, ADC_11db);
    Serial.println("Rotating ESP32 muxed thermistor reader started");

    // Initialize ESP-NOW
    WiFi.mode(WIFI_STA);

    Serial.print("Rotating ESP32 MAC: ");
    Serial.println(WiFi.macAddress());

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }

    esp_now_register_send_cb(on_data_sent); // Bind callback for sent temperature packets

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, MAIN_ESP32_MAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add ESP-NOW peer");
        return;
    }

    Serial.println("Rotating thermistor ESP-NOW sender started");
}

void loop() {
    static uint32_t last_send = 0;

    if (millis() - last_send >= SEND_PERIOD_MS) {
        last_send = millis();

        RotatingThermistorPacket packet;

        packet.sequence = sequence_counter++;
        //packet.temp_c = read_thermistor_c(packet.raw, packet.v_adc);
        for (uint8_t ch = 0; ch < NUM_THERMISTORS; ch++) 
            packet.temp_c[ch] = read_thermistor_c(ch, packet.raw[ch], packet.v_adc[ch]);
        

        esp_err_t result = esp_now_send(MAIN_ESP32_MAC, reinterpret_cast<uint8_t*>(&packet), sizeof(packet));

        Serial.print("Sent seq=");
        Serial.print(packet.sequence);

        // Serial.print(" raw=");
        // Serial.print(packet.raw);
        // Serial.print(" v_adc=");
        // Serial.print(packet.v_adc, 3);
        // Serial.print(" temp_c=");
        // Serial.print(packet.temp_c, 2);
        for (uint8_t ch = 0; ch < NUM_THERMISTORS; ch++) {
            Serial.print(" | CH");
            Serial.print(ch);
            Serial.print(" raw=");
            Serial.print(packet.raw[ch]);
            Serial.print(" v=");
            Serial.print(packet.v_adc[ch], 3);
            Serial.print(" temp=");
            Serial.print(packet.temp_c[ch], 2);
        }

        Serial.print(" result=");
        Serial.println(result == ESP_OK ? "OK" : "ERR");

    }
}