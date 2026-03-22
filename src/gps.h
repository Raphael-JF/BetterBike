#ifndef GPS_H
#define GPS_H

#include <stdint.h>
#define W_gps 15
#define H_gps 16


void draw_line(int x0, int y0, int x1, int y1);
void clear_compass_for_gps() ;
void clear_compass_for_magnetometer() ;
void update_compass(float angle);



#endif // GPS_H