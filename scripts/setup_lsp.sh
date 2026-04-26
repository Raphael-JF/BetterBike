#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"

if ! command -v pio >/dev/null 2>&1; then
  echo "[setup_lsp] PlatformIO (pio) introuvable dans le PATH" >&2
  exit 1
fi

PLATFORMIO_CORE_DIR="${PLATFORMIO_CORE_DIR:-$HOME/.platformio}"
XTENSA_GXX="$PLATFORMIO_CORE_DIR/packages/toolchain-xtensa-esp32/bin/xtensa-esp32-elf-g++"
XTENSA_ROOT="$PLATFORMIO_CORE_DIR/packages/toolchain-xtensa-esp32"
GCC_INCLUDE_DIR="$XTENSA_ROOT/lib/gcc/xtensa-esp32-elf/8.4.0/include"
NEWLIB_INCLUDE_DIR="$XTENSA_ROOT/xtensa-esp32-elf/include"
CXX_INCLUDE_DIR="$XTENSA_ROOT/xtensa-esp32-elf/include/c++/8.4.0"
CXX_TARGET_INCLUDE_DIR="$CXX_INCLUDE_DIR/xtensa-esp32-elf"
CXX_BACKWARD_INCLUDE_DIR="$CXX_INCLUDE_DIR/backward"

if [[ ! -x "$XTENSA_GXX" ]]; then
  echo "[setup_lsp] Compilateur introuvable: $XTENSA_GXX" >&2
  echo "[setup_lsp] Lance d'abord: pio pkg install -e esp32dev" >&2
  exit 1
fi

echo "[setup_lsp] Regeneration de compile_commands.json..."
pio run -t compiledb

# Force un chemin absolu du compilateur dans compile_commands.json pour clangd.
sed -i "s#\"command\": \"xtensa-esp32-elf-g++ #\"command\": \"$XTENSA_GXX #g" compile_commands.json

echo "[setup_lsp] Generation de .clangd..."
cat > .clangd <<EOF
CompileFlags:
  Compiler: $XTENSA_GXX
  Remove:
    - -mlongcalls
    - -fstrict-volatile-bitfields
    - -fno-tree-switch-conversion
  Add:
    - --gcc-toolchain=$XTENSA_ROOT
    - --sysroot=$XTENSA_ROOT/xtensa-esp32-elf
    - -nostdlibinc
    - -nostdinc++
    - -Qunused-arguments
    - -Wno-unknown-warning-option
    - -Wno-unused-command-line-argument
    - -isystem
    - $GCC_INCLUDE_DIR
    - -isystem
    - $NEWLIB_INCLUDE_DIR
    - -isystem
    - $CXX_INCLUDE_DIR
    - -isystem
    - $CXX_TARGET_INCLUDE_DIR
    - -isystem
    - $CXX_BACKWARD_INCLUDE_DIR
EOF

echo "[setup_lsp] OK"
echo "[setup_lsp] Redemarre clangd dans VS Code (ou reload window)."
