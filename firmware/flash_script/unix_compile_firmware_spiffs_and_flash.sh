#!/bin/bash
# run PlatformIO build target for building file system image
pip3 install esptool

while true; do
  platformio run --target buildfs --environment T-QT-Pro-N4R2

  # use esptool to flash the firmware and SPIFFS image to the ESP32-S3,
  # replace this with your own path to esptool
  # (use your own python package manager like conda, or `pip install esptool` then `python -m esptool ...`)
  esptool.py --chip esp32s3 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x0 ./.pio/build/T-QT-Pro-N4R2/bootloader.bin 0x8000 ./.pio/build/T-QT-Pro-N4R2/partitions.bin 0xe000 ./flash_script/boot_app0.bin 0x10000 ./.pio/build/T-QT-Pro-N4R2/firmware.bin 0x00290000 ./.pio/build/T-QT-Pro-N4R2/spiffs.bin

  # run PlatformIO build for T-QT-Pro-N4R2 environment
  platformio run --environment T-QT-Pro-N4R2

  # monitor device using PlatformIO
  platformio device monitor --environment T-QT-Pro-N4R2 --port /dev/ttyACM0

  # wait for user input before restarting the loop
  read -n 1 -s -r -p "Press any key to keep flashing, or Ctrl+C to exit..."
done
