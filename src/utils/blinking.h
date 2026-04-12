#ifndef BLINKING_H
#define BLINKING_H

#include <stdint.h>


struct blinking {
    uint8_t working; // flag pour indiquer que ce blinking est en marche
    uint8_t blink_state; // 0 = off, 1 = on
    uint16_t period; // période totale du cycle de clignotement en ms
    unsigned long last_toggle_millis; // dernière fois où l'état a été changé
    uint8_t initial_blink_state; // état initial du clignotement, utilisé pour réinitialiser le clignotement si besoin
    uint8_t just_started; // flag pour indiquer que le clignotement vient d'être initialisé, pour réagir immédiatement.
};


/*
    Crée une structure de clignotement avec la période et l'état initial spécifiés.
*/
struct blinking blinking_create(uint16_t period, uint8_t initial_blink_state) ;


/*
    Met à jour l'état de b en fonction du temps écoulé. Retourne 1 si le blinking est actif et l'état a changé, ou si le clignotement vient d'être démarré, 0 sinon.
*/
uint8_t blinking_update(struct blinking b) ;


/*
    Arrête le clignotement du blinking d'indice donné.
*/
void blinking_stop(struct blinking b);


/*
    Démarre le clignotement du blinking d'indice donné.
*/
void blinking_start(struct blinking b);



#endif // BLINKING_H