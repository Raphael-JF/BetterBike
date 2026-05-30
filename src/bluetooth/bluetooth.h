#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

// BLE protocol constants
#define BLUETOOTH_FRAME_WAYPOINT  0x01
#define BLUETOOTH_FRAME_CALIBRATE 0x02
#define BLUETOOTH_FRAME_STOP_SAVE_CALIBRATE 0x03
#define BLUETOOTH_FRAME_STOP_CALIBRATE 0x04

#define BLUETOOTH_WAYPOINT_PAYLOAD_SIZE (sizeof(double) * 2)

// Telemetry frames (bike -> web, notifications)
#define BLUETOOTH_TX_CAL_POINT      0x11
#define BLUETOOTH_TX_COMPENSATOR    0x12

// Custom BLE UUIDs (128-bit). Must match web side.
//
// Generated once; keep stable for compatibility.
// Service UUID: 5a7d7c20-2d20-4a66-b52a-8c88c7f823b1
// RX Char UUID: 5a7d7c21-2d20-4a66-b52a-8c88c7f823b1 (Write)
// TX Char UUID: 5a7d7c22-2d20-4a66-b52a-8c88c7f823b1 (Notify)
#define BLUETOOTH_SERVICE_UUID "5a7d7c20-2d20-4a66-b52a-8c88c7f823b1"
#define BLUETOOTH_RX_CHAR_UUID "5a7d7c21-2d20-4a66-b52a-8c88c7f823b1"
#define BLUETOOTH_TX_CHAR_UUID "5a7d7c22-2d20-4a66-b52a-8c88c7f823b1"

enum frame_state : uint8_t {
    FRAME_STATE_IDLE = 0,
    FRAME_STATE_WAYPOINT_PAYLOAD = 1,
};

enum bluetooth_events : uint8_t {
    BLE_NONE = 0,
    BLE_NEW_WAYPOINT = 1,
    BLE_ENTER_GPS = 2,
    BLE_ENTER_CAL = 3,
    BLE_SAVE_CAL = 4,
};

// Initialize BLE GATT service/characteristics.
void bluetooth_init(const char* deviceName);

// Process any received RX data and update waypoint_position / events.
uint8_t read_bluetooth_data();

// Bike -> web notifications
void bluetooth_notify_calibration_point(int16_t x, int16_t y, int16_t z);
void bluetooth_notify_compensator(int16_t x_offset, int16_t y_offset);

#endif // BLUETOOTH_H
