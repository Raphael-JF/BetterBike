#include <BleSerial.h>

#include <Arduino.h>
#include <cstdlib>

#include "bluetooth.h"
#include "gps/gps_core.h"

BleSerial ble;


char waypoint_buffer[BLUETOOTH_WAYPOINT_BUFFER_SIZE];
size_t waypoint_buffer_length = 0;

bool parse_waypoint_message(const char* message, double* latitude, double* longitude) {
	char* end_lat = nullptr;
	char* end_lng = nullptr;

	*latitude = strtod(message, &end_lat);
	if (end_lat == message || *end_lat != ';') {
		return false;
	}

	*longitude = strtod(end_lat + 1, &end_lng);
	if (end_lng == end_lat + 1) {
		return false;
	}

	while (*end_lng == ' ' || *end_lng == '\t') {
		++end_lng;
	}

	if (*end_lng != '\0') {
		return false;
	}
	return true;
}

uint8_t bluetooth_update_waypoint_from_stream() {
	uint8_t waypoint_updated = 0;
	char c;
	while (ble.available()) {
		c = ble.read();

		if (c == '\r' || c == '\n') {
			if (waypoint_buffer_length == 0) {
				continue;
			}

			waypoint_buffer[waypoint_buffer_length] = '\0';

			double latitude = 0.0;
			double longitude = 0.0;
			if (parse_waypoint_message(waypoint_buffer, &latitude, &longitude)) {
				if (latitude != waypoint_position.lat || longitude != waypoint_position.lng) {
					waypoint_position.lat = latitude;
					waypoint_position.lng = longitude;
					waypoint_updated = 1;
				}
			}
			waypoint_buffer_length = 0;
			continue;
		}
		if (waypoint_buffer_length < BLUETOOTH_WAYPOINT_BUFFER_SIZE - 1) {
			waypoint_buffer[waypoint_buffer_length] = c;
			waypoint_buffer_length++;
		} else {
			waypoint_buffer_length = 0;
		}
	}

	return waypoint_updated;
}
