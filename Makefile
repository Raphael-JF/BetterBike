all:
	gcc main/lcd_sim.cpp main/gps.cpp main/caracters.cpp  -lm -o truc && ./truc
