all: local

local:
	gcc -v -I~/Bureau/Code/I260319-122733-uno/src lcd_sim.h gps.h caracters.h  -lm -o truc && ./truc