#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <BleSerial.h>
#define BLUETOOTH_WAYPOINT_BUFFER_SIZE 64

extern BleSerial ble;


/*
	Lit les données reçues par Bluetooth et met à jour `waypoint_position`
	si une trame valide au format `latitude;longitude` est reçue.
	Retourne 1 si le waypoint a changé, 0 sinon.
*/
uint8_t bluetooth_update_waypoint_from_stream();

#endif // BLUETOOTH_H