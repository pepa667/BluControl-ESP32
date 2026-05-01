#!/bin/zsh

# --- CONFIGURAÇÕES ---
SCRIPT_DIR="${0:A:h}"
ROOT_DIR="${SCRIPT_DIR:h}"  # fw_v2/
FINAL_BUILD_DIR="$ROOT_DIR/build_final"
RELEASE_FILE="bebopCORE_Full.bin"

# Cores
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo "${BLUE}===> bebopCORE Flash Tool <===${NC}"

# 1. Verifica binário
if [ ! -f "$FINAL_BUILD_DIR/$RELEASE_FILE" ]; then
    echo "${RED}Erro: $FINAL_BUILD_DIR/$RELEASE_FILE não encontrado.${NC}"
    echo "Corre primeiro: ${YELLOW}./scripts/merge_firmware.sh${NC}"
    exit 1
fi

SIZE=$(du -h "$FINAL_BUILD_DIR/$RELEASE_FILE" | cut -f1)
echo "Binário: ${BLUE}$FINAL_BUILD_DIR/$RELEASE_FILE${NC} ($SIZE)"

# 2. Descoberta de portas seriais
echo "${BLUE}Procurando dispositivos serial...${NC}"
ports=($(find /dev -maxdepth 1 \( -name "cu.usbserial*" -o -name "cu.usbmodem*" -o -name "cu.wchusbserial*" \) 2>/dev/null | sort))

if [ ${#ports[@]} -eq 0 ]; then
    echo "${YELLOW}Nenhuma porta encontrada automaticamente.${NC}"
    printf "Porta manual (ex: /dev/cu.usbserial-1420): "
    read SELECTED_PORT
else
    echo "Portas encontradas:"
    for i in {1..${#ports[@]}}; do
        echo "  $i) ${ports[$i]}"
    done
    printf "Escolha [1-${#ports[@]}] (padrão 1): "
    read choice
    choice=${choice:-1}
    SELECTED_PORT="${ports[$choice]}"
fi

# 3. Validação
if [ -z "$SELECTED_PORT" ]; then
    echo "${RED}Porta inválida.${NC}"
    exit 1
fi

echo "${GREEN}Porta: $SELECTED_PORT${NC}"

# 4. Comando flash + monitor (usa blu-switch para símbolos de debug)
FLASH_CMD="esptool.py --chip esp32 --port $SELECTED_PORT --baud 460800 write_flash 0x0 $FINAL_BUILD_DIR/$RELEASE_FILE && cd $ROOT_DIR/blu-switch && idf.py -p $SELECTED_PORT monitor"

echo "-------------------------------------------------------"
echo "${YELLOW}Comando (Cmd+V para colar):${NC}"
echo "-------------------------------------------------------"
echo "\n$FLASH_CMD\n"
echo "-------------------------------------------------------"

echo -n "$FLASH_CMD" | pbcopy
echo "${BLUE}Copiado para clipboard.${NC}"