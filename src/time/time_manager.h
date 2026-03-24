#ifndef TIME_MANAGER__H
#define TIME_MANAGER


struct time_base{
    int hours;
    int minutes;
};

// Base temporelle actuellement affichée à l'écran (en UTC)
extern struct time_base current_time;

// Dernière base temporelle GPS reçue
extern struct time_base last_got_gps_time;

// Dernière base temporelle GPS prise en compte pour faire avancer l'heure avec millis()
extern struct time_base last_used_gps_time;


// Valeur de millis() au moment où gps_base_hour et gps_base_minutes ont été synchronisées
extern unsigned long gps_sync_millis;


extern int utc_year;
extern int utc_month;
extern int utc_day;


/*
    Actualise en coulisse l'heure à afficher, en fuseau local Français. Ne gère pas l'affichage
*/
void update_time(void);
struct time_base utc_to_local(int day, int month, int year, struct time_base time_utc);

#endif // TIME_MANAGER__H
