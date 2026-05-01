#!/bin/zsh

# --- CONFIGURAÇÕES ---
SCRIPT_DIR="${0:A:h}"
ROOT_DIR="${SCRIPT_DIR:h}"  # fw_v2/
FINAL_BUILD_DIR="$ROOT_DIR/build_final"
TMP_DIR="$ROOT_DIR/tmp_bins"
RELEASE_FILE="bebopCORE_Full.bin"

# Origem dos builds
OTA_BUILD="$ROOT_DIR/blu-ota/build"
SWITCH_BUILD="$ROOT_DIR/blu-switch/build"
GENERIC_BUILD="$ROOT_DIR/blu-generic/build"

# Cores
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo "${BLUE}===> bebopCORE Merge Tool <===${NC}"

# Helper: verifica e copia, saindo com erro se não existir
check_and_copy() {
    local src="$1" dst="$2"
    if [ ! -f "$src" ]; then
        echo "${RED}Erro: Ficheiro não encontrado: $src${NC}"
        echo "  → Corre 'idf.py build' na pasta correspondente primeiro."
        exit 1
    fi
    cp "$src" "$dst"
}

# 1. Limpeza e preparação
rm -rf "$TMP_DIR" "$FINAL_BUILD_DIR"
mkdir -p "$TMP_DIR" "$FINAL_BUILD_DIR"

# 2. Bootloader, partition-table e ota_data vêm do blu-ota
#    (IMPORTANTE: ota_data_initial do blu-ota garante boot em ota_0 na 1ª vez)
echo "${YELLOW}[1/4] Coletando binários comuns (blu-ota)...${NC}"
check_and_copy "$OTA_BUILD/bootloader/bootloader.bin"         "$TMP_DIR/bootloader.bin"
check_and_copy "$OTA_BUILD/partition_table/partition-table.bin" "$TMP_DIR/partition-table.bin"
check_and_copy "$OTA_BUILD/ota_data_initial.bin"               "$TMP_DIR/ota_data_initial.bin"

# 3. OTA app + website (blu-ota)
echo "${YELLOW}[2/4] Coletando binários do OTA (blu-ota)...${NC}"
check_and_copy "$OTA_BUILD/blucontrol-ota.bin"   "$TMP_DIR/blucontrol-ota.bin"
check_and_copy "$OTA_BUILD/www.bin"              "$TMP_DIR/www.bin"

# 4. Switch mode (blu-switch)
echo "${YELLOW}[3/4] Coletando binário Switch (blu-switch)...${NC}"
check_and_copy "$SWITCH_BUILD/blucontrol-switch.bin"   "$TMP_DIR/blucontrol-switch.bin"

# 5. Generic mode (blu-generic)
echo "${YELLOW}[4/4] Coletando binário Generic (blu-generic)...${NC}"
check_and_copy "$GENERIC_BUILD/blucontrol-generic.bin" "$TMP_DIR/blucontrol-generic.bin"

# 6. Merge via esptool
echo "${YELLOW}[Merge] Gerando $RELEASE_FILE...${NC}"
esptool.py --chip esp32 merge_bin -o "$FINAL_BUILD_DIR/$RELEASE_FILE" \
     0x1000  "$TMP_DIR/bootloader.bin" \
     0x8000  "$TMP_DIR/partition-table.bin" \
     0xd000  "$TMP_DIR/ota_data_initial.bin" \
    0x10000  "$TMP_DIR/blucontrol-ota.bin" \
   0x110000  "$TMP_DIR/blucontrol-switch.bin" \
   0x210000  "$TMP_DIR/blucontrol-generic.bin" \
   0x310000  "$TMP_DIR/www.bin"

if [ $? -ne 0 ]; then
    echo "${RED}Erro no merge!${NC}"
    exit 1
fi

SIZE=$(du -h "$FINAL_BUILD_DIR/$RELEASE_FILE" | cut -f1)
echo "${GREEN}===> SUCESSO! <===${NC}"
echo "Ficheiro: ${BLUE}$FINAL_BUILD_DIR/$RELEASE_FILE${NC} ($SIZE)"
echo "Para flashar: ${YELLOW}./scripts/flash_monitor.sh${NC}"