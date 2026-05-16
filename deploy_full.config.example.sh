# Example configuration for deploy_full.sh
# Copy this file to deploy_full.config.sh and adjust it for your setup.

ROOT_DIR="/Users/pepa/Desktop/bebopCORE/BluControl-ESP32"
IDF_PY="/Users/pepa/esp/v5.0/esp-idf/tools/idf.py"
PYTHON="/Users/pepa/.espressif/python_env/idf5.0_py3.8_env/bin/python"
ESPTOOL_CMD="esptool.py"

BUILD_PROJECTS=("blu-ota" "blu-switch" "blu-generic")

MERGE_ITEMS=(
    "blu-ota|build/bootloader/bootloader.bin|bootloader.bin|0x1000"
    "blu-ota|build/partition_table/partition-table.bin|partition-table.bin|0x8000"
    "blu-ota|build/ota_data_initial.bin|ota_data_initial.bin|0xd000"
    "blu-ota|build/blucontrol-ota.bin|blucontrol-ota.bin|0x10000"
    "blu-switch|build/blucontrol-switch.bin|blucontrol-switch.bin|0x110000"
    "blu-generic|build/blucontrol-generic.bin|blucontrol-generic.bin|0x210000"
    "blu-ota|build/www.bin|www.bin|0x310000"
)

UPDATE_ITEMS=(
    "blucontrol-switch.upd|\\x01\\x00|blucontrol-switch.bin"
    "blucontrol-generic.upd|\\x01\\x01|blucontrol-generic.bin"
)

UPDPKG_FILE="$ROOT_DIR/build_final/blucontrol.updpkg"
MONITOR_PROJECT="blu-ota"
FLASH_PORT="/dev/cu.usbserial-XXXX"
FLASH_BAUD="460800"
AUTO_MONITOR=1