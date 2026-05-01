#!/bin/zsh

# --- CONFIGURAÇÕES ---
SCRIPT_DIR="${0:A:h}"
ROOT_DIR="${SCRIPT_DIR:h}"  # fw_v2/

IDF_PY="/Users/pepa/esp/v5.0/esp-idf/tools/idf.py"
PYTHON="/Users/pepa/.espressif/python_env/idf5.0_py3.8_env/bin/python"

# Ordem de build importa: blu-ota primeiro (gera ota_data_initial.bin e bootloader)
PROJECTS=(
    "blu-ota"
    "blu-switch"
    "blu-generic"
)

# Cores
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

# --- Flags ---
DO_CLEAN=0
if [[ "$1" == "--clean" || "$1" == "-c" ]]; then
    DO_CLEAN=1
fi

echo "${BLUE}===> bebopCORE Build All <===${NC}"
[[ $DO_CLEAN -eq 1 ]] && echo "${YELLOW}Modo: fullclean + build${NC}" || echo "${YELLOW}Modo: build incremental (usa --clean para fullclean)${NC}"

TOTAL=${#PROJECTS[@]}
FAILED=()
START_TOTAL=$SECONDS

for i in {1..$TOTAL}; do
    PROJECT="${PROJECTS[$i]}"
    PROJECT_DIR="$ROOT_DIR/$PROJECT"

    echo ""
    echo "${BLUE}[$i/$TOTAL] $PROJECT${NC}"
    echo "-------------------------------------------------------"

    if [ ! -d "$PROJECT_DIR" ]; then
        echo "${RED}Erro: pasta não encontrada: $PROJECT_DIR${NC}"
        FAILED+=("$PROJECT")
        continue
    fi

    cd "$PROJECT_DIR"
    START=$SECONDS

    if [[ $DO_CLEAN -eq 1 ]]; then
        echo "${YELLOW}  → fullclean...${NC}"
        "$PYTHON" "$IDF_PY" fullclean > /dev/null 2>&1
    fi

    echo "${YELLOW}  → build...${NC}"
    "$PYTHON" "$IDF_PY" build 2>&1

    EXIT_CODE=$?
    ELAPSED=$((SECONDS - START))

    if [ $EXIT_CODE -eq 0 ]; then
        BIN=$(find "$PROJECT_DIR/build" -maxdepth 1 -name "blucontrol-*.bin" 2>/dev/null | head -1)
        SIZE=""
        [[ -n "$BIN" ]] && SIZE=" ($(du -h "$BIN" | cut -f1))"
        echo "${GREEN}  ✓ $PROJECT OK${SIZE} — ${ELAPSED}s${NC}"
    else
        echo "${RED}  ✗ $PROJECT FALHOU (exit $EXIT_CODE) — ${ELAPSED}s${NC}"
        FAILED+=("$PROJECT")
    fi
done

# --- Resumo ---
TOTAL_ELAPSED=$((SECONDS - START_TOTAL))
echo ""
echo "======================================================="
if [ ${#FAILED[@]} -eq 0 ]; then
    echo "${GREEN}Todos os projetos compilados com sucesso! (${TOTAL_ELAPSED}s)${NC}"
    echo "Podes correr: ${YELLOW}./scripts/merge_firmware.sh${NC}"
else
    echo "${RED}Falhou: ${FAILED[*]}${NC}"
    echo "${YELLOW}OK: $(( TOTAL - ${#FAILED[@]} ))/$TOTAL${NC}"
    exit 1
fi
