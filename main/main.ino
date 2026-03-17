void setup() {
    Serial.begin(115200);
    Serial.println("Mon premier programme");
    pinMode(2,OUTPUT);
}

void loop() {
    digitalWrite(2, HIGH); //On allume la led bleue
    delay(500); //On attend pendant 500 ms
    digitalWrite(2, LOW); //On eteinds la led bleue
    delay(500); //On attend pendant 500 ms
}