## TO DO

- Enhance the gps_grid to avoid using uint8_t for just bits.
- Add the support for the GPS module

## Setup LSP Reproductible (clangd)

Pour reproduire la meme config IntelliSense sur une autre machine:

1. Installer les dependances PlatformIO du projet.
2. Lancer:

```bash
make lsp-setup
```

Ce script:

- regenere `compile_commands.json` via PlatformIO,
- force le compilateur xtensa en chemin absolu dans la base de compilation,
- regenere `.clangd` avec des options de compatibilite pour eviter les erreurs `unknown argument`.

Ensuite, dans VS Code:

1. Relancer clangd (`Restart language server`), ou
2. Recharger la fenetre (`Reload Window`).

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
- Il est possible de réduire la fréquence de l'advertising bluetooth, mais cela n'est pas une priorité pour le moment: 
    ```c
    pAdvertising->setMinInterval(160); // ~100 ms
    pAdvertising->setMaxInterval(800); // ~500 ms
    ```
- Plus tard, on pourra créer une app mobile qui permet de parler avec l'ordinateur de bord. Pour l'instant ça se fait via l'appli web, mais ça dépend d'internet
- On doit maintenant pouvoir calibrer le magnétomètre pour compenser les perturbations magnétiques locales (hard iron et soft iron). On va se renseigner et ça va être cool à faire. 

### Lien utile

https://www.espboards.dev/esp32/esp32-hw-394/

### truc bluetooth

```c

    // logo bluetooth :
    uint8_t bluetooth_car[8] = {
        0b00110,
        0b10101,
        0b01101,
        0b00110,
        0b01101,
        0b10101,
        0b00110,
        0b00000
    };

    lcd.createChar(6, bluetooth_car);
```