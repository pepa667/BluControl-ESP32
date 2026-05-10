#!/bin/zsh
# Função rápida para pegar o tamanho do arquivo no macOS/Linux
get_size() {
    wc -c < "$1" | tr -d ' '
}

SIZE_HID=$(get_size "blucontrol-HID.bin")
echo "Tamanho do arquivo HID: $SIZE_HID bytes"

printf "\x01\x01%07d" $SIZE_HID | cat - "blucontrol-HID.bin" > "blucontrol-HID.upd"