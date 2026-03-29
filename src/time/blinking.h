 #include <stdint.h>





struct blinking {
    uint8_t state; // 0 = off, 1 = on
    uint16_t period; // période totale du cycle de clignotement en ms
    uint16_t last_toggle_millis; // dernière fois où l'état a été changé
};

extern blinking* blinkings_to_update[];
extern blinking compass_frame_blinking;
#define COMPASS_FRAME_BLINKING_INIT blinking_create(500, 1) 
#define NB_BLINKINGS_TO_UPDATE 1
#define IDX_COMPASS_FRAME_BLINKING 0


/*
    Crée une structure de clignotement avec la période et l'état initial spécifiés.
*/
struct blinking blinking_create(uint16_t period, uint8_t initial_state);


/*
    Met à jour l'état de clignotement en fonction du temps écoulé. Retourne 1 si l'état a changé, 0 sinon.
*/
uint8_t update_one_blinking(struct blinking* b);

/* 
    Met à jour les états de tous les clignotements utilisés dans ce projet. À appeler dans la loop() principale.
*/
void update_all_blinkings(void);
