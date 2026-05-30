#include "bluetooth.h"

#include <queue>
#include <vector>

#include <Arduino.h>

#include <NimBLEDevice.h>

#include "gps/gps_core.h"

static enum frame_state state = FRAME_STATE_IDLE;

static uint8_t waypoint_payload[BLUETOOTH_WAYPOINT_PAYLOAD_SIZE];
static size_t waypoint_payload_length = 0;

// --- RX queue (data written by central) ---
static std::queue<uint8_t> rxQueue;

// --- NimBLE objects ---
static NimBLEServer* g_server = nullptr;
static NimBLECharacteristic* g_rxChar = nullptr;
static NimBLECharacteristic* g_txChar = nullptr;

static bool g_bleConnected = false;

// ---- Parsing ----
static int parse_waypoint_message(const uint8_t* payload, double* latitude, double* longitude) {
    if (payload == NULL || latitude == NULL || longitude == NULL) {
        return 0;
    }

    // Browser sends little-endian float64. ESP32 is little-endian too.
    memcpy(latitude, payload, sizeof(double));
    memcpy(longitude, payload + sizeof(double), sizeof(double));
    return 1;
}

class BluetoothServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        (void)pServer;
        (void)connInfo;
        g_bleConnected = true;
    }

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        (void)connInfo;
        (void)reason;
        g_bleConnected = false;
        // Restart advertising to allow reconnect.
        NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
        if (adv) {
            adv->start();
        }
    }
};

class BluetoothRxCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        (void)connInfo;

        const std::string& value = pCharacteristic->getValue();
        for (size_t i = 0; i < value.size(); i++) {
            rxQueue.push((uint8_t)value[i]);
        }
    }
};

void bluetooth_init(const char* deviceName) {
    NimBLEDevice::init(deviceName ? deviceName : "VeloGPS");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    g_server = NimBLEDevice::createServer();
    g_server->setCallbacks(new BluetoothServerCallbacks());

    NimBLEService* service = g_server->createService(BLUETOOTH_SERVICE_UUID);

    g_rxChar = service->createCharacteristic(
        BLUETOOTH_RX_CHAR_UUID,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
    );
    g_rxChar->setCallbacks(new BluetoothRxCallbacks());

    g_txChar = service->createCharacteristic(
        BLUETOOTH_TX_CHAR_UUID,
        NIMBLE_PROPERTY::NOTIFY
    );

    // Note:
    // NimBLE-Arduino starts services implicitly when the server is running/advertising.
    // NimBLEService::start() is deprecated and has no effect in current versions.

    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    advertising->addServiceUUID(BLUETOOTH_SERVICE_UUID);

    // We intentionally do NOT call advertising->setScanResponse(true) because it is not
    // available in all NimBLE-Arduino versions. Advertising the service UUID is enough
    // for Web Bluetooth filters.
    advertising->start();
}

static int rx_available() {
    return (int)rxQueue.size();
}

static int rx_read() {
    if (rxQueue.empty()) return -1;
    uint8_t v = rxQueue.front();
    rxQueue.pop();
    return (int)v;
}

// ---- Lecture Bluetooth (now: read queued GATT writes) ----
uint8_t read_bluetooth_data(void) {
    uint8_t res = BLE_NONE;

    while (rx_available() > 0) {
        int r = rx_read();
        if (r < 0) break;

        uint8_t byte = (uint8_t)r;

        switch (state) {

            case FRAME_STATE_IDLE:
                switch (byte) {
                    case BLUETOOTH_FRAME_WAYPOINT:
                        state = FRAME_STATE_WAYPOINT_PAYLOAD;
                        waypoint_payload_length = 0;
                        break;
                    case BLUETOOTH_FRAME_CALIBRATE:
                        res = BLE_ENTER_CAL;
                        break;
                    case BLUETOOTH_FRAME_STOP_SAVE_CALIBRATE:
                        res = BLE_SAVE_CAL;
                        break;
                    case BLUETOOTH_FRAME_STOP_CALIBRATE:
                        res = BLE_ENTER_GPS;
                        break;
                    default:
                        continue; // Ignore unrecognized bytes
                }
                break;

            case FRAME_STATE_WAYPOINT_PAYLOAD:
                waypoint_payload[waypoint_payload_length++] = byte;

                if (waypoint_payload_length == BLUETOOTH_WAYPOINT_PAYLOAD_SIZE) {
                    double latitude = 0.0;
                    double longitude = 0.0;

                    if (parse_waypoint_message(waypoint_payload, &latitude, &longitude)) {
                        if (latitude != waypoint_position.lat ||
                            longitude != waypoint_position.lng) {

                            waypoint_position.lat = latitude;
                            waypoint_position.lng = longitude;
                            res = BLE_NEW_WAYPOINT;
                        }
                    }

                    state = FRAME_STATE_IDLE;
                    waypoint_payload_length = 0;
                }
                break;
        }
    }
    return res;
}

static void notify_bytes(const uint8_t* data, size_t len) {
    if (!g_bleConnected || g_txChar == nullptr || data == nullptr || len == 0) return;

    g_txChar->setValue(data, len);
    g_txChar->notify();
}

void bluetooth_notify_calibration_point(int16_t x, int16_t y, int16_t z) {
    uint8_t payload[1 + 2 + 2 + 2];
    payload[0] = BLUETOOTH_TX_CAL_POINT;

    memcpy(&payload[1], &x, sizeof(int16_t));
    memcpy(&payload[3], &y, sizeof(int16_t));
    memcpy(&payload[5], &z, sizeof(int16_t));

    notify_bytes(payload, sizeof(payload));
}

void bluetooth_notify_compensator(int16_t x_offset, int16_t y_offset) {
    uint8_t payload[1 + 2 + 2];
    payload[0] = BLUETOOTH_TX_COMPENSATOR;

    memcpy(&payload[1], &x_offset, sizeof(int16_t));
    memcpy(&payload[3], &y_offset, sizeof(int16_t));

    notify_bytes(payload, sizeof(payload));
}
