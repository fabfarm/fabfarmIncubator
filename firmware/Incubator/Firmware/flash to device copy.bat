:start
esptool.exe --chip esp32s3   --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x0 C:\Users\lpgn\OneDrive\Documentos\PlatformIO\Projects\Incubator\.pio\build\T-QT-Pro-N4R2\bootloader.bin 0x8000 C:\Users\lpgn\OneDrive\Documentos\PlatformIO\Projects\Incubator\.pio\build\T-QT-Pro-N4R2\partitions.bin 0xe000 boot_app0.bin 0x10000 C:\Users\lpgn\OneDrive\Documentos\PlatformIO\Projects\Incubator\.pio\build\T-QT-Pro-N4R2\firmware.bin
@echo Press any key,keey find device praogram
pause
goto start

