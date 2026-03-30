#ifndef TIME_MANAGER__H
#define TIME_MANAGER


struct time_base{
    uint8_t hours;
    uint8_t minutes;
};

// Base temporelle actuellement affichée à l'écran (en UTC)
extern struct time_base current_time;

// Dernière base temporelle GPS reçue
extern struct time_base last_got_gps_time;

// Dernière base temporelle GPS prise en compte pour faire avancer l'heure avec millis()
extern struct time_base last_used_gps_time;


// Valeur de millis() au moment où gps_base_hour et gps_base_minutes ont été synchronisées
extern unsigned long last_time_sync_millis;


extern uint16_t utc_year;
extern uint8_t utc_month;
extern uint8_t utc_day;


/*
    Actualise en coulisse l'heure à afficher, en fuseau local Français. Ne gère pas l'affichage.
    Renvoie 1 si l'heure a été mise à jour, 0 sinon (utile pour savoir s'il faut rafraîchir l'affichage).
*/
int update_time(void);
struct time_base utc_to_local(uint8_t day, uint8_t month, uint16_t year, struct time_base time_utc);


#endif // TIME_MANAGER__H
