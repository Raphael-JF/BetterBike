#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <BleSerial.h>


#include "gps/gps_core.h"

#define BLUETOOTH_WAYPOINT_BUFFER_SIZE 64


#define BLUETOOTH_FRAME_WAYPOINT  0x01
#define BLUETOOTH_FRAME_CALIBRATE 0x02
#define BLUETOOTH_FRAME_STOP_SAVE_CALIBRATE 0x03

#define BLUETOOTH_WAYPOINT_PAYLOAD_SIZE (sizeof(double) * 2)

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

extern BleSerial ble;



/*
	Lit les données reçues par Bluetooth et met à jour `waypoint_position`
	si une trame valide au format `latitude;longitude` est reçue.
	Retourne 1 si le waypoint a changé, 0 sinon.
*/
uint8_t read_bluetooth_data();

#endif // BLUETOOTH_H
