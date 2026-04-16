
#include "bluetooth.h"


static enum frame_state state = FRAME_STATE_IDLE;
static uint8_t waypoint_payload[BLUETOOTH_WAYPOINT_PAYLOAD_SIZE];
static size_t waypoint_payload_length = 0;

// ---- Instance BLE (à adapter selon ton driver C) ----
extern BleSerial ble;

// ---- Parsing ----
int parse_waypoint_message(const uint8_t* payload, double* latitude, double* longitude) {
    if (payload == NULL || latitude == NULL || longitude == NULL) {
        return 0;
    }

    memcpy(latitude, payload, sizeof(double));
    memcpy(longitude, payload + sizeof(double), sizeof(double));
    return 1;
}

// ---- Lecture Bluetooth ----
uint8_t read_bluetooth_data(void) {
    uint8_t res = BLUETOOTH_EVENT_NONE;

    while (ble.available()) {
        uint8_t byte = (uint8_t)ble.read();

        switch (state) {

            case FRAME_STATE_IDLE:
                if (byte == BLUETOOTH_FRAME_WAYPOINT) {
                    state = FRAME_STATE_WAYPOINT_PAYLOAD;
                    waypoint_payload_length = 0;
                } else if (byte == BLUETOOTH_FRAME_CALIBRATE) {
                    return BLUETOOTH_EVENT_CALIBRATE_RECEIVED;
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
                            res = BLUETOOTH_EVENT_WAYPOINT_RECEIVED;
                        }
                    }

                    state = FRAME_STATE_IDLE;
                    waypoint_payload_length = 0;
                }
                break;
    }

    return BLUETOOTH_EVENT_CALIBRATE_RECEIVED;
}
}