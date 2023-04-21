# FabFarm Incubator

This project is an incubator controller for maintaining temperature and humidity levels based on the user's requirements. It uses an Arduino and ESP32 board with various sensors and modules to achieve this goal.

## Features

- Temperature and Humidity Sensing
- Heater Control (relay)
- Ventilation Control (servo)
- LCD Display
- Web-based User Interface
- OTA Updates

## Hardware Components

- ESP32 Board
- DHT11 or BME280 Temperature and Humidity Sensor
- TFT_eSPI Display
- Servo Motor
- Relay Module

## Libraries Used

- Wire
- DHT
- Adafruit_BME280
- WiFi
- ESP32Servo
- AsyncElegantOTA
- TFT_eSPI
- SPI
- FS
- SPIFFS
- ESPAsyncWebServer

## Getting Started

1. Clone this repository.
2. Install the required libraries.
3. Open the project in the Arduino IDE.
4. Modify the `WIFI_SSID` and `WIFI_PASSWORD` constants to match your network credentials.
5. Change the `deviceName` constant to a unique name for your device.
6. Connect your ESP32 board and upload the code.
7. Open the Serial Monitor to check if the ESP32 is connected to the WiFi network.
8. Access the web interface by entering the IP address displayed in the Serial Monitor in your browser.

## Usage

1. Set the desired temperature and humidity values in the web interface.
2. Monitor the actual temperature and humidity values on the LCD display.
3. The system will control the heater and ventilation based on the desired values.
4. Use the web interface to pause or resume the incubator.

## Troubleshooting

Error codes may be displayed on the TFT screen. Check the `errorWithCode` function in the code for more information on specific error codes.

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for more information.

## To Do

1. A PID (Proportional-Integral-Derivative) control algorithm for more accurate temperature and humidity control.
2. Data logging to an external storage like an SD card, or sending data to a remote server for long-term storage and analysis.
3. Email or push notifications when the incubator temperature or humidity goes out of range or there is a system error.
4. Incorporate a real-time clock (RTC) module to maintain accurate timekeeping, which can be helpful for time-stamped data logging.
5. A battery backup system to ensure continuous operation during power outages.
6. A user interface to change settings directly on the device without using a web server.
7. Security improvements, such as password protection or authentication, for the web server.
8. Manual controls: buttons to manually increase or decrease the temperature and humidity settings
9. 


