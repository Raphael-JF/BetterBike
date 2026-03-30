# ===== CONFIG =====
BOARD ?= esp32dev
PORT ?= /dev/ttyUSB0

# ===== COMMANDES =====

build:
	pio run

upload:
	pio run -t upload --upload-port $(PORT)

monitor:
	pio device monitor --port $(PORT)

# build + flash
flash: upload

# build + flash + monitor
run:
	pio run -t upload --upload-port $(PORT)
	pio device monitor --port $(PORT)

# clean
clean:
	pio run -t clean

# reset projet (bourrin)
purge:
	rm -rf .pio .pioenvs .piolibdeps

# liste ports
ports:
	pio device list