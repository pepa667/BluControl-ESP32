#!/bin/zsh

# [PROJ] bebopCORE - Chimera Series
# [DEV] Full Automation Deployment Script (Build -> Merge -> Flash -> Monitor)

# --- CONFIGURAÇÕES ---
SCRIPT_DIR="${0:A:h}"
[[ "$SCRIPT_DIR" == *"/scripts" ]] && ROOT_DIR="${SCRIPT_DIR:h}" || ROOT_DIR="$SCRIPT_DIR"

IDF_PY="/Users/pepa/esp/v5.2.3/esp-idf/tools/idf.py"
PYTHON="/Users/pepa/.espressif/python_env/idf5.2_py3.9_env/bin/python"

# Per-project IDF overrides (project name → "IDF_PY|PYTHON")
declare -A PROJECT_IDF
PROJECT_IDF["blu-ota"]="/Users/pepa/esp/v5.0/esp-idf/tools/idf.py|/Users/pepa/.espressif/python_env/idf5.0_py3.8_env/bin/python"
PROJECT_IDF["blu-generic"]="/Users/pepa/esp/v5.2.3/esp-idf/tools/idf.py|/Users/pepa/.espressif/python_env/idf5.2_py3.9_env/bin/python"
PROJECT_IDF["blu-switch"]="/Users/pepa/esp/v5.2.3/esp-idf/tools/idf.py|/Users/pepa/.espressif/python_env/idf5.2_py3.9_env/bin/python"
FINAL_BUILD_DIR="$ROOT_DIR/build_final"
TMP_DIR="$ROOT_DIR/tmp_bins"
RELEASE_FILE="bebopCORE_Full.bin"

PROJECTS=("blu-ota" "blu-switch" "blu-generic")

# Cores
GREEN='\033[0;32m'; BLUE='\033[0;34m'; YELLOW='\033[1;33m'; RED='\033[0;31m'; NC='\033[0m'

# --- FUNÇÃO FULL CLEAN ---
full_clean() {
    echo "${RED}===> [CLEAN] Resetando todas as builds e temporários...${NC}"
    rm -rf "$FINAL_BUILD_DIR" "$TMP_DIR"
    
    for PROJECT in "${PROJECTS[@]}"; do
        PROJECT_DIR="$ROOT_DIR/$PROJECT"
        if [ -d "$PROJECT_DIR" ]; then
            local override=("${(s:|:)PROJECT_IDF[$PROJECT]}")
            local _idf="${override[1]:-$IDF_PY}"
            local _py="${override[2]:-$PYTHON}"
            local _idf_root="${_idf:h:h}"
            local _py_env="${_py:h:h}"
            echo "${YELLOW}Limpando $PROJECT (IDF: ${_idf_root:t})...${NC}"
            (cd "$PROJECT_DIR" && IDF_PATH="$_idf_root" IDF_PYTHON_ENV_PATH="$_py_env" "$_py" "$_idf" fullclean)
        fi
    done
    echo "${GREEN}✔ Limpeza concluída!${NC}"
}

# Verifica se o argumento é fullclean
if [[ "$1" == "fullclean" ]]; then
    full_clean
    # Se quiser que ele pare após limpar, use exit 0. 
    # Se quiser que ele limpe e já comece o build, apague a linha abaixo:
    exit 0
fi

echo "${BLUE}===> [GROUND-ZERO] bebopCORE AUTO-DEPLOY <===${NC}"

# 1. BUILD PHASE
# ... (restante do seu código de build original aqui)
# 1. BUILD PHASE
START_TOTAL=$SECONDS
for i in {1..${#PROJECTS[@]}}; do
    PROJECT="${PROJECTS[$i]}"
    PROJECT_DIR="$ROOT_DIR/$PROJECT"

    local override=("${(s:|:)PROJECT_IDF[$PROJECT]}")
    local _idf="${override[1]:-$IDF_PY}"
    local _py="${override[2]:-$PYTHON}"
    local _idf_root="${_idf:h:h}"
    local _py_env="${_py:h:h}"

    echo "${BLUE}[Build $i/${#PROJECTS[@]}] $PROJECT  [IDF ${_idf_root:t} / ${_py_env:t}]${NC}"
    if [ ! -d "$PROJECT_DIR" ]; then
        echo "${RED}Pasta não encontrada: $PROJECT_DIR${NC}"; exit 1
    fi

    (cd "$PROJECT_DIR" && IDF_PATH="$_idf_root" IDF_PYTHON_ENV_PATH="$_py_env" "$_py" "$_idf" build) || { echo "${RED}Falha no build de $PROJECT${NC}"; exit 1 }
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

# 2.1 Gerar pacotes de atualização incremental (opcional, para OTA)
echo "${YELLOW}===> Gerando pacotes de atualização incremental...${NC}"

# Função rápida para pegar o tamanho do arquivo no macOS/Linux
get_size() {
    wc -c < "$1" | tr -d ' '
}

SIZE_SWITCH=$(get_size "$TMP_DIR/blucontrol-switch.bin")
SIZE_GENERIC=$(get_size "$TMP_DIR/blucontrol-generic.bin")

# Gerando os headers compatíveis com o seu parser no ESP32
printf "\x01\x00%07d" $SIZE_SWITCH | cat - "$TMP_DIR/blucontrol-switch.bin" > "$FINAL_BUILD_DIR/blucontrol-switch.upd"
printf "\x01\x01%07d" $SIZE_GENERIC | cat - "$TMP_DIR/blucontrol-generic.bin" > "$FINAL_BUILD_DIR/blucontrol-generic.upd"

# Empacotando tudo
cat "$FINAL_BUILD_DIR/blucontrol-switch.upd" "$FINAL_BUILD_DIR/blucontrol-generic.upd" > "$FINAL_BUILD_DIR/blucontrol.updpkg"

echo "${GREEN}✔ Pacote .updpkg gerado com sucesso!${NC}"


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
    # Entra no monitor usando o diretório do OTA para carregar os símbolos de debug (ELF)
    local ota_override=("${(s:|:)PROJECT_IDF[blu-ota]}")
    local _ota_idf="${ota_override[1]:-$IDF_PY}"
    local _ota_py="${ota_override[2]:-$PYTHON}"
    local _ota_idf_root="${_ota_idf:h:h}"
    local _ota_py_env="${_ota_py:h:h}"
    cd "$ROOT_DIR/blu-ota" && IDF_PATH="$_ota_idf_root" IDF_PYTHON_ENV_PATH="$_ota_py_env" "$_ota_py" "$_ota_idf" -p "$SELECTED_PORT" monitor
else
    echo "${RED}✗ Falha no Flash.${NC}"
    exit 1
fi