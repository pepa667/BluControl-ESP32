# BluControl-ESP32

A project to emulate any Retro controller as a modern one with ESP32, compatible with Nintendo Switch and with any other Bluetooth based console/adapter (like [BlueRetro](https://github.com/darthcloud/BlueRetro)).

Requires ESP-IDF 5.0 release currently.

## Special Thanks

[dpedu.io](https://dpedu.io/article/2015-03-11/nintendo-64-joystick-pinout-arduino) for his blog about N64 analogs with Arduino.

## Deploy

The full deploy flow is controlled by [deploy_full.sh](deploy_full.sh). For reuse in other setups, copy [deploy_full.config.example.sh](deploy_full.config.example.sh) to [deploy_full.config.sh](deploy_full.config.sh) and adjust the root path, toolchain paths, project list, merge map, and flash port.

The script also accepts environment overrides, so you can keep the same file and point it at a different checkout or board without editing the shell script itself.
