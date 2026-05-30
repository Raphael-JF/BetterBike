/* Web project constants (loaded before app.js). Refer to src/bluetooth/bluetooth.h to see the supported trame headers*/

/*
 * Trame : [Header (1 byte)] [Payload (8+8 bytes for lat/lon)]
 * */

const TYPE_WAYPOINT = 0x01;
const TYPE_CALIBRATION = 0x02;
const TYPE_STOP_SAVE_CALIBRATION = 0x03;
const TYPE_STOP_CALIBRATION = 0x04;

// Notifications (bike -> web)
const TYPE_TX_CAL_POINT = 0x11;     // [0x11][int16 x][int16 y][int16 z]
const TYPE_TX_COMPENSATOR = 0x12;   // [0x12][int16 x_offset][int16 y_offset]

// Must match firmware NimBLE UUIDs in src/bluetooth/bluetooth.h
const BLE_SERVICE_UUID = "5a7d7c20-2d20-4a66-b52a-8c88c7f823b1";
const BLE_RX_CHAR_UUID = "5a7d7c21-2d20-4a66-b52a-8c88c7f823b1";
const BLE_TX_CHAR_UUID = "5a7d7c22-2d20-4a66-b52a-8c88c7f823b1";
