#!/usr/bin/env bash

# [PROJ] bebopCORE - Chimera Series
# [DEV] Full Automation Deployment Script (Build -> Merge -> Flash -> Monitor)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]:-$0}")" && pwd)"
if [[ "$SCRIPT_DIR" == *"/scripts" ]]; then
    DEFAULT_ROOT_DIR="$(dirname "$SCRIPT_DIR")"
else
    DEFAULT_ROOT_DIR="$SCRIPT_DIR"
fi

DEFAULT_CONFIG_FILE="$SCRIPT_DIR/deploy_full.config.sh"
if [[ -f "$DEFAULT_CONFIG_FILE" ]]; then
    source "$DEFAULT_CONFIG_FILE"
fi

ROOT_DIR="${ROOT_DIR:-$DEFAULT_ROOT_DIR}"
FINAL_BUILD_DIR="${FINAL_BUILD_DIR:-$ROOT_DIR/build_final}"
TMP_DIR="${TMP_DIR:-$ROOT_DIR/tmp_bins}"
RELEASE_FILE="${RELEASE_FILE:-bebopCORE_Full.bin}"
CHIP="${CHIP:-esp32}"
FLASH_BAUD="${FLASH_BAUD:-460800}"
FLASH_ADDRESS="${FLASH_ADDRESS:-0x0}"
MONITOR_PROJECT="${MONITOR_PROJECT:-blu-ota}"
AUTO_MONITOR="${AUTO_MONITOR:-1}"
FLASH_PORT="${FLASH_PORT:-}"
UPDPKG_FILE="${UPDPKG_FILE:-$FINAL_BUILD_DIR/blucontrol.updpkg}"

if [[ -n "${IDF_PY:-}" ]]; then
    :
elif command -v idf.py >/dev/null 2>&1; then
    IDF_PY="$(command -v idf.py)"
else
    IDF_PY="/Users/pepa/esp/v5.0/esp-idf/tools/idf.py"
fi

if [[ -n "${PYTHON:-}" ]]; then
    :
elif command -v python3 >/dev/null 2>&1; then
    PYTHON="$(command -v python3)"
elif command -v python >/dev/null 2>&1; then
    PYTHON="$(command -v python)"
else
    PYTHON="/Users/pepa/.espressif/python_env/idf5.0_py3.8_env/bin/python"
fi

if [[ -n "${ESPTOOL_CMD:-}${ESPTOOL_PY:-}" ]]; then
    ESPTOOL_CMD="${ESPTOOL_CMD:-$ESPTOOL_PY}"
elif command -v esptool.py >/dev/null 2>&1; then
    ESPTOOL_CMD="$(command -v esptool.py)"
else
    ESPTOOL_CMD="esptool.py"
fi

if ! declare -p BUILD_PROJECTS >/dev/null 2>&1; then
    BUILD_PROJECTS=("blu-ota" "blu-switch" "blu-generic")
fi

if ! declare -p MERGE_ITEMS >/dev/null 2>&1; then
    MERGE_ITEMS=(
        "blu-ota|build/bootloader/bootloader.bin|bootloader.bin|0x1000"
        "blu-ota|build/partition_table/partition-table.bin|partition-table.bin|0x8000"
        "blu-ota|build/ota_data_initial.bin|ota_data_initial.bin|0xd000"
        "blu-ota|build/blucontrol-ota.bin|blucontrol-ota.bin|0x10000"
        "blu-switch|build/blucontrol-switch.bin|blucontrol-switch.bin|0x110000"
        "blu-generic|build/blucontrol-generic.bin|blucontrol-generic.bin|0x210000"
        "blu-ota|build/www.bin|www.bin|0x310000"
    )
fi

if ! declare -p UPDATE_ITEMS >/dev/null 2>&1; then
    UPDATE_ITEMS=(
        "blucontrol-switch.upd|\\x01\\x00|blucontrol-switch.bin"
        "blucontrol-generic.upd|\\x01\\x01|blucontrol-generic.bin"
    )
fi

GREEN=$'\033[0;32m'
BLUE=$'\033[0;34m'
YELLOW=$'\033[1;33m'
RED=$'\033[0;31m'
NC=$'\033[0m'

usage() {
    cat <<EOF
Uso: $(basename "$0") [fullclean|--help]

Variáveis que podem ser sobrescritas via config ou ambiente:
  ROOT_DIR, BUILD_PROJECTS, MERGE_ITEMS, UPDATE_ITEMS, UPDPKG_FILE
  IDF_PY, PYTHON, ESPTOOL_CMD, CHIP, FLASH_BAUD, FLASH_ADDRESS
  FLASH_PORT, MONITOR_PROJECT, AUTO_MONITOR, RELEASE_FILE

Config opcional:
  $DEFAULT_CONFIG_FILE
EOF
}

die() {
    echo "${RED}$*${NC}" >&2
    exit 1
}

ensure_dir() {
    mkdir -p "$1"
}

get_size() {
    wc -c < "$1" | tr -d ' '
}

copy_artifact() {
    local project="$1"
    local relative_path="$2"
    local output_name="$3"
    local source_path="$ROOT_DIR/$project/$relative_path"

    [[ -f "$source_path" ]] || die "Artefato não encontrado: $source_path"
    cp "$source_path" "$TMP_DIR/$output_name"
}

full_clean() {
    echo "${RED}===> [CLEAN] Resetando todas as builds e temporários...${NC}"
    rm -rf "$FINAL_BUILD_DIR" "$TMP_DIR"

    local project project_dir
    for project in "${BUILD_PROJECTS[@]}"; do
        project_dir="$ROOT_DIR/$project"
        if [[ -d "$project_dir" ]]; then
            echo "${YELLOW}Limpando $project...${NC}"
            (cd "$project_dir" && "$PYTHON" "$IDF_PY" fullclean)
        fi
    done

    echo "${GREEN}✔ Limpeza concluída!${NC}"
}

build_projects() {
    local total_projects="${#BUILD_PROJECTS[@]}"
    local index=1
    local project project_dir

    for project in "${BUILD_PROJECTS[@]}"; do
        project_dir="$ROOT_DIR/$project"
        echo "${BLUE}[Build $index/$total_projects] $project${NC}"
        [[ -d "$project_dir" ]] || die "Pasta não encontrada: $project_dir"
        (cd "$project_dir" && "$PYTHON" "$IDF_PY" build)
        (( index++ ))
    done
}

merge_firmware() {
    echo "${YELLOW}===> Mesclando Binários...${NC}"
    rm -rf "$TMP_DIR" "$FINAL_BUILD_DIR"
    ensure_dir "$TMP_DIR"
    ensure_dir "$FINAL_BUILD_DIR"

    local merge_args entry project relative_path output_name offset
    merge_args=(--chip "$CHIP" merge_bin -o "$FINAL_BUILD_DIR/$RELEASE_FILE")

    for entry in "${MERGE_ITEMS[@]}"; do
        project="${entry%%|*}"
        entry="${entry#*|}"
        relative_path="${entry%%|*}"
        entry="${entry#*|}"
        output_name="${entry%%|*}"
        offset="${entry##*|}"

        copy_artifact "$project" "$relative_path" "$output_name"
        merge_args+=("$offset" "$TMP_DIR/$output_name")
    done

    "$ESPTOOL_CMD" "${merge_args[@]}"
}

build_update_packages() {
    echo "${YELLOW}===> Gerando pacotes de atualização incremental...${NC}"

    local upd_files=()
    local entry output_name header_bytes source_name size output_path

    for entry in "${UPDATE_ITEMS[@]}"; do
        output_name="${entry%%|*}"
        entry="${entry#*|}"
        header_bytes="${entry%%|*}"
        source_name="${entry##*|}"
        output_path="$FINAL_BUILD_DIR/$output_name"

        [[ -f "$TMP_DIR/$source_name" ]] || die "Artefato de update não encontrado: $TMP_DIR/$source_name"
        size="$(get_size "$TMP_DIR/$source_name")"
        printf '%b%07d' "$header_bytes" "$size" | cat - "$TMP_DIR/$source_name" > "$output_path"
        upd_files+=("$output_path")
    done

    if (( ${#upd_files[@]} > 0 )); then
        cat "${upd_files[@]}" > "$UPDPKG_FILE"
        echo "${GREEN}✔ Pacote .updpkg gerado com sucesso: $UPDPKG_FILE${NC}"
    fi
}

select_port() {
    if [[ -n "$FLASH_PORT" ]]; then
        echo "$FLASH_PORT"
        return 0
    fi

    local ports=()
    while IFS= read -r -d $'\0' port; do
        ports+=("$port")
    done < <(find /dev -maxdepth 1 \( -name "cu.usbserial*" -o -name "cu.usbmodem*" -o -name "cu.wchusbserial*" \) -print0 2>/dev/null | sort -z)

    if [[ ${#ports[@]} -eq 1 ]]; then
        echo "${GREEN}Apenas um dispositivo detectado: ${ports[0]} (Confirmado automaticamente)${NC}" >&2
        echo "${ports[0]}"
    elif [[ ${#ports[@]} -gt 1 ]]; then
        local choice=1
        local i
        echo "Múltiplas portas detectadas, escolha uma:" >&2
        for (( i=0; i<${#ports[@]}; i++ )); do
            echo "  $((i+1))) ${ports[$i]}" >&2
        done
        read -r -p "Escolha [1-${#ports[@]}]: " choice
        echo "${ports[$((choice-1))]}"
    else
        die "Nenhum ESP32 encontrado! Plugue o cabo e tente de novo."
    fi
}

flash_and_monitor() {
    local selected_port="$1"

    echo "${YELLOW}Subindo firmware para $selected_port a $FLASH_BAUD baud...${NC}"
    "$ESPTOOL_CMD" --chip "$CHIP" --port "$selected_port" --baud "$FLASH_BAUD" write_flash "$FLASH_ADDRESS" "$FINAL_BUILD_DIR/$RELEASE_FILE"

    echo "${GREEN}✓ Flash OK!${NC}"
    if [[ "$AUTO_MONITOR" == "1" ]]; then
        echo "${GREEN}Entrando em modo Monitor...${NC}"
        (cd "$ROOT_DIR/$MONITOR_PROJECT" && "$PYTHON" "$IDF_PY" -p "$selected_port" monitor)
    fi
}

case "${1:-}" in
    -h|--help)
        usage
        exit 0
        ;;
    fullclean|--fullclean)
        full_clean
        exit 0
        ;;
esac

echo "${BLUE}===> [GROUND-ZERO] bebopCORE AUTO-DEPLOY <===${NC}"

build_projects
merge_firmware
build_update_packages
selected_port="$(select_port)"
flash_and_monitor "$selected_port"