/* Web project constants (loaded before app.js). Refer to src/bluetooth/bluetooth.h to see the supported trame headers*/

/*
 * Trame : [Header (1 byte)] [Payload (8+8 bytes for lat/lon)]   
 * */

const TYPE_WAYPOINT = 0x01;
const TYPE_CALIBRATION = 0x02;
