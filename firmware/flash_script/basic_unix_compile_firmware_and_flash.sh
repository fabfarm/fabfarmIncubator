#!/bin/bash

# 1st - run `usbipd wsl list` first to find the BUSID of the device that looks like:
# USB Serial Device (COM3), USB JTAG/serial debug unit          Attached - Ubuntu

# 2nd - run this in powershell to connect device: `usbipd wsl attach -b <INSERT-BUSID-HERE> -d Ubuntu`

# this needs to be run in the shell script, make sure you have pip installed
pipx install esptool

while true; do
  # run PlatformIO build for T-QT-Pro-N4R2 environment
  platformio run --environment T-QT-Pro-N4R2

  # use esptool to flash the firmware to the ESP32-S3,
  # replace this with your own path to esptool
  esptool.py --chip esp32s3 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x10000 ./.pio/build/T-QT-Pro-N4R2/firmware.bin

  # monitor device using PlatformIO
  platformio device monitor --environment T-QT-Pro-N4R2 --port /dev/ttyACM0

  # wait for user input before restarting the loop
  read -n 1 -s -r -p "Press any key to keep flashing, or Ctrl+C to exit..."
done
