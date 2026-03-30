#include <TinyGPS++.h>
#include <SoftwareSerial.h>

TinyGPSPlus gps;
SoftwareSerial gpsSerial(3, 4); // RX, TX


void setup() {
    Serial.begin(9600);     // Pour le moniteur série
    gpsSerial.begin(9600);    // UART GPS
}


void loop() {
    while (gpsSerial.available()) {
        char c = gpsSerial.read();
        if(gps.encode(c)){
            if (gps.location.isValid())
            {
                Serial.print(gps.location.lat(), 6);
                Serial.print(F(","));
                Serial.println(gps.location.lng(), 6);
            }
            else
            {
                Serial.println(F("INVALID"));
            }
            
            if (gps.date.isValid())
            {
                Serial.print(gps.date.month());
                Serial.print(F("/"));
                Serial.print(gps.date.day());
                Serial.print(F("/"));
                Serial.print(gps.date.year());
                Serial.print(" ");
            }
            if (gps.time.isValid())
            {
                Serial.print(gps.time.hour());
                Serial.print(":");
                Serial.print(gps.time.minute());

            }
            
        }
    }
}