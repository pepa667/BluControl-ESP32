#!/bin/zsh

# [PROJ] bebopCORE - Chimera Series
# [DEV] Full Automation Deployment Script (Build -> Merge -> Flash -> Monitor)

# --- CONFIGURAÇÕES ---
SCRIPT_DIR="${0:A:h}"
# Ajuste se o script estiver dentro de /scripts ou na raiz
[[ "$SCRIPT_DIR" == *"/scripts" ]] && ROOT_DIR="${SCRIPT_DIR:h}" || ROOT_DIR="$SCRIPT_DIR"

IDF_PY="/Users/pepa/esp/v5.0/esp-idf/tools/idf.py"
PYTHON="/Users/pepa/.espressif/python_env/idf5.0_py3.8_env/bin/python"
FINAL_BUILD_DIR="$ROOT_DIR/build_final"
TMP_DIR="$ROOT_DIR/tmp_bins"
RELEASE_FILE="bebopCORE_Full.bin"

# Ordem de build: blu-ota primeiro (essencial para bootloader/partition table)
PROJECTS=("blu-ota" "blu-switch" "blu-generic")

# Cores
GREEN='\033[0;32m'; BLUE='\033[0;34m'; YELLOW='\033[1;33m'; RED='\033[0;31m'; NC='\033[0m'

echo "${BLUE}===> [GROUND-ZERO] bebopCORE AUTO-DEPLOY <===${NC}"

# 1. BUILD PHASE
START_TOTAL=$SECONDS
for i in {1..${#PROJECTS[@]}}; do
    PROJECT="${PROJECTS[$i]}"
    PROJECT_DIR="$ROOT_DIR/$PROJECT"
    
    echo "${BLUE}[Build $i/${#PROJECTS[@]}] $PROJECT${NC}"
    if [ ! -d "$PROJECT_DIR" ]; then
        echo "${RED}Pasta não encontrada: $PROJECT_DIR${NC}"; exit 1
    fi

    cd "$PROJECT_DIR"
    # Build incremental para ser rápido, mas você pode passar -c se precisar de fullclean
    "$PYTHON" "$IDF_PY" build || { echo "${RED}Falha no build de $PROJECT${NC}"; exit 1 }
done

# 2. MERGE PHASE
echo "${YELLOW}===> Mesclando Binários...${NC}"
rm -rf "$TMP_DIR" "$FINAL_BUILD_DIR" && mkdir -p "$TMP_DIR" "$FINAL_BUILD_DIR"

cp "$ROOT_DIR/blu-ota/build/bootloader/bootloader.bin" "$TMP_DIR/"
cp "$ROOT_DIR/blu-ota/build/partition_table/partition-table.bin" "$TMP_DIR/"
cp "$ROOT_DIR/blu-ota/build/ota_data_initial.bin" "$TMP_DIR/"
cp "$ROOT_DIR/blu-ota/build/blucontrol-ota.bin" "$TMP_DIR/"
cp "$ROOT_DIR/blu-ota/build/www.bin" "$TMP_DIR/"
cp "$ROOT_DIR/blu-switch/build/blucontrol-switch.bin" "$TMP_DIR/"
cp "$ROOT_DIR/blu-generic/build/blucontrol-generic.bin" "$TMP_DIR/"

esptool.py --chip esp32 merge_bin -o "$FINAL_BUILD_DIR/$RELEASE_FILE" \
     0x1000  "$TMP_DIR/bootloader.bin" \
     0x8000  "$TMP_DIR/partition-table.bin" \
     0xd000  "$TMP_DIR/ota_data_initial.bin" \
    0x10000  "$TMP_DIR/blucontrol-ota.bin" \
   0x110000  "$TMP_DIR/blucontrol-switch.bin" \
   0x210000  "$TMP_DIR/blucontrol-generic.bin" \
   0x310000  "$TMP_DIR/www.bin" || { echo "${RED}Erro no Merge!${NC}"; exit 1 }

# 3. FLASH & MONITOR PHASE
echo "${BLUE}===> Localizando ESP32...${NC}"
ports=($(find /dev -maxdepth 1 \( -name "cu.usbserial*" -o -name "cu.usbmodem*" -o -name "cu.wchusbserial*" \) 2>/dev/null))

if [ ${#ports[@]} -eq 1 ]; then
    SELECTED_PORT="${ports[1]}"
    echo "${GREEN}Apenas um dispositivo detectado: $SELECTED_PORT (Confirmado automaticamente)${NC}"
elif [ ${#ports[@]} -gt 1 ]; then
    echo "Múltiplas portas detectadas, escolha uma:"
    for i in {1..${#ports[@]}}; do echo "  $i) ${ports[$i]}"; done
    read "choice?Escolha [1-${#ports[@]}]: "
    SELECTED_PORT="${ports[$choice:-1]}"
else
    echo "${RED}Nenhum ESP32 encontrado! Plugue o cabo e tente de novo.${NC}"; exit 1
fi

echo "${YELLOW}Subindo firmware para $SELECTED_PORT a 460800 baud...${NC}"
esptool.py --chip esp32 --port "$SELECTED_PORT" --baud 460800 write_flash 0x0 "$FINAL_BUILD_DIR/$RELEASE_FILE"

if [ $? -eq 0 ]; then
    echo "${GREEN}✓ Flash OK! Entrando em modo Monitor...${NC}"
    # Entra no monitor usando o diretório do switch para carregar os símbolos de debug (ELF)
    cd "$ROOT_DIR/blu-switch" && "$PYTHON" "$IDF_PY" -p "$SELECTED_PORT" monitor
else
    echo "${RED}✗ Falha no Flash.${NC}"
    exit 1
fi