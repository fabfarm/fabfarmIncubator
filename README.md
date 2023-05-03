# Fabfarm Egg Incubator

This is the code repository for the Fabfarm Egg Incubator, a project for controlling and monitoring the temperature, humidity, and egg tray position inside an egg incubator.

## Features

- Temperature control using a relay
- Humidity control using a vent servo
- Egg tray rotation using a tray servo
- Sensor data display on a TFT screen
- Web interface for configuration and data visualization
- Data logging to SPIFFS
- PID control for temperature and humidity

## Hardware Requirements

- ESP32 microcontroller
- BME280 temperature, humidity and pressure sensor
- Relay module for temperature control
- Servo motor for vent control (humidity)
- Servo motor for tray rotation
- TFT display (ILI9341 based)

## Software Dependencies

- Arduino IDE
- Adafruit BME280 library
- ESP32Servo library
- TFT_eSPI library
- ESPAsyncWebServer library
- WiFiManager library
- FS.h, SPIFFS.h, and SPI.h libraries (built-in)
- PID_v1 library

## Getting Started

1. Clone this repository to your local machine.
2. Open the project folder ./firmware using platformio.
3. Platformio will install the required libraries mentioned in the Software Dependencies section.
4. Connect the hardware components as described in the Hardware Requirements section.
5. Upload the code to the ESP32 microcontroller.
6. In first time connect to the "AutoConnectAP" Wi-Fi network and configure the Wi-Fi settings.
7. Access the web interface by navigating to the IP address displayed on the TFT screen.
8. Configure temperature, humidity, and servo settings using the web interface.
9. Monitor the incubator data on the web interface and TFT screen.

## Functionality Overview

- The incubator monitors and controls temperature and humidity using a BME280 sensor and a relay and servo motor, respectively.
- The egg tray position is controlled by a servo motor, which rotates the tray at regular intervals.
- Sensor data is displayed on the TFT screen and logged to the ESP32's SPIFFS memory.
- A web interface is provided for configuring temperature, humidity, and servo settings, as well as for visualizing logged data.
- The system can be paused or resumed using the web interface.

## Contributing

Feel free to fork this repository and submit pull requests for bug fixes, improvements, or new features. Please ensure that your code follows the existing style and structure.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.

## Acknowledgements

This project was written by Lucio.