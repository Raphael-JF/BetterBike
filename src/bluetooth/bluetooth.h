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
#define BLUETOOTH_WAYPOINT_PAYLOAD_SIZE (sizeof(double) * 2)

enum frame_state : uint8_t {
    FRAME_STATE_IDLE = 0,
    FRAME_STATE_WAYPOINT_PAYLOAD = 1,
};


enum bluetooth_events : uint8_t {
    BLUETOOTH_EVENT_NONE = 0,
    BLUETOOTH_EVENT_WAYPOINT_RECEIVED = 1,
    BLUETOOTH_EVENT_CALIBRATE_RECEIVED = 2,
};

extern BleSerial ble;



/*
	Lit les données reçues par Bluetooth et met à jour `waypoint_position`
	si une trame valide au format `latitude;longitude` est reçue.
	Retourne 1 si le waypoint a changé, 0 sinon.
*/
uint8_t read_bluetooth_data();

#endif // BLUETOOTH_H