 #include <stdint.h>

struct blinking {
    uint8_t state; // 0 = off, 1 = on
    uint16_t period; // période totale du cycle de clignotement en ms
    uint16_t last_toggle_millis; // dernière fois où l'état a été changé
};


/*
    Crée une structure de clignotement avec la période et l'état initial spécifiés.
*/
struct blinking blinking_create(uint16_t period, uint8_t initial_state);


/*
    Met à jour l'état de clignotement en fonction du temps écoulé. Retourne 1 si l'état a changé, 0 sinon.
*/
uint8_t blinking_update(struct blinking* b);