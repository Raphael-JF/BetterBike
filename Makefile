# ===== CONFIG =====
BOARD ?= esp32dev
BOARD_IDF ?= esp32dev-idf
PORT ?= /dev/ttyUSB0

# ===== COMMANDES =====

build:
	pio run -e $(BOARD)

build-idf:
	pio run -e $(BOARD_IDF)

upload:
	pio run -e $(BOARD) -t upload --upload-port $(PORT)

upload-idf:
	pio run -e $(BOARD_IDF) -t upload --upload-port $(PORT)

monitor:
	pio device monitor --port $(PORT)

# build + flash
flash: upload

# build + flash + monitor
run:
	pio run -e $(BOARD) -t upload --upload-port $(PORT)
	pio device monitor --port $(PORT)

run-idf:
	pio run -e $(BOARD_IDF) -t upload --upload-port $(PORT)
	pio device monitor --port $(PORT)

# live plot of magnetometer raw data (uses tools/plot_magnetometer.py)
plot-mag:
	python3 tools/plot_magnetometer.py --port $(PORT)

# clean
clean:
	pio run -t clean

# reset projet (bourrin)
purge:
	rm -rf .pio .pioenvs .piolibdeps

# liste ports
ports:
	pio device list

# bootstrap LSP/clangd config (portable)
lsp-setup:
	./scripts/setup_lsp.sh
