#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"

if ! command -v pio >/dev/null 2>&1; then
  echo "[setup_lsp] PlatformIO (pio) introuvable dans le PATH" >&2
  exit 1
fi

mkdir -p .vscode

echo "[setup_lsp] Regeneration de .vscode/settings.json..."
cat > .vscode/settings.json <<'EOF'
{
    "clangd.arguments": [
        "--query-driver=/home/raph/.platformio/packages/toolchain-xtensa-esp32/bin/xtensa-esp32-elf-g++",
    ]
}
EOF

echo "[setup_lsp] Regeneration de .clangd..."
cat > .clangd <<'EOF'
CompileFlags:
  Remove:
    - -mlongcalls
    - -fno-tree-switch-conversion
    - -fstrict-volatile-bitfields
    - -mtext-section-literals
EOF

echo "[setup_lsp] Regeneration de compile_commands.json..."
pio run -t compiledb

echo "[setup_lsp] OK"
echo "[setup_lsp] Redemarre clangd dans VS Code (ou reload window)."
