## TO DO

- Enhance the gps_grid to avoid using uint8_t for just bits.
- Add the support for the GPS module

## Configuration ESP32

Le projet est maintenant compatible ESP32 (env `esp32dev`) avec:

- LCD en I2C via `Wire` sur `SDA=21` et `SCL=22`
- GPS en UART matériel (`Serial1`) sur `RX=GPIO16` et `TX=GPIO17`

## Branchement (ESP32)

### LCD RGB (I2C)

- `VCC` -> `5V` (ou `3V3` si votre module LCD l'accepte)
- `GND` -> `GND`
- `SDA` -> `GPIO21`
- `SCL` -> `GPIO22`

### GPS (UART)

- `GPS TX` -> `GPIO16` (RX ESP32)
- `GPS RX` -> `GPIO17` (TX ESP32)
- `VCC` -> selon le module (`3V3` ou `5V`)
- `GND` -> `GND`

### Important

- Toujours relier les masses (`GND`) ensemble.
- Croiser TX/RX pour l'UART (`TX` d'un côté vers `RX` de l'autre).
- Si un module sort du 5V logique sur RX/TX ou I2C, utiliser un level shifter pour protéger l'ESP32 (3.3V logique).


### Comportement souhaité

- Si la position GPS est encore valide, actualiser l'aiguille comme il se doit. Afficher alors la vitesse
- Si la position GPS devient vieille (> 5 secondes), commencer à faire clignoter le cadran pour indiquer que les données ne sont plus fraîches.
- Si la position GPS devient invalide (> 10 secondes), laisser le cadran allumé avec l'aiguille toujours actualisée, mais ne plus faire clignoter (indication que les données sont vieilles).



### Lien utile

https://www.espboards.dev/esp32/esp32-hw-394/