
:start
C:\Users\lpgn\.platformio\penv\Scripts\platformio.exe run --environment T-QT-Pro-N4R2 
C:\Users\lpgn\.platformio\penv\Scripts\platformio.exe run --target buildfs --environment T-QT-Pro-N4R2 
.\Firmware\esptool.exe --chip esp32s3  --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x0 .\.pio\build\T-QT-Pro-N4R2\bootloader.bin 0x8000 .\.pio\build\T-QT-Pro-N4R2\partitions.bin 0xe000 .\Firmware\boot_app0.bin 0x10000 .\.pio\build\T-QT-Pro-N4R2\firmware.bin 0x00290000 .\.pio\build\T-QT-Pro-N4R2\spiffs.bin
C:\Users\lpgn\.platformio\penv\Scripts\platformio.exe device monitor --environment T-QT-Pro-N4R2 --port COM4
@echo Press any key,keey find device praogram
pause
goto start

