# Low Power Temperature/Humidity Sensor

## Overview

This project is designed for creating a low-power temperature/humidity sensor using the ESP-IDF framework. The sensor used in this project is the SHT31 temperature/humidity sensor. The collected data is transmitted to an MQTT broker. Sensor can easily be integrated to homeassistant or any other platform of your choice.

### Sensor data plotted in homeassistant

![image](https://github.com/LauriAlanen/ESP-SHT31/assets/80245457/da741b91-d5f3-4207-9b38-5248b818e7c3)

## Hardware Requirements

- Any ESP board, prerably mini version without power leds.
- SHT31 Temperature/Humidity Sensor
- 18650 Battery + Holder
- Li-on Charging circuit eg. TP4056
- Resistors for voltage divider (optional). Voltage divider is used for voltage monitoring.
- N-Chanell Mosfet for switching the voltage monitoring on/off, I used ST70A

## Schematic 
![image](https://github.com/LauriAlanen/ESP-SHT31/assets/80245457/358f4ad7-6e31-40f0-a15e-2094c3f8ea4b)


## Features

- **Low Power Operation:** The project is optimized for low power consumption to maximize the sensor's battery life. This is achieved by using one shot mode for reading the measurements from the SHT31. The major power save comes from ESP entering deep sleep for `X` amount of time. More about one shot mode can be read from [SHT31 Datasheet](https://sensirion.com/media/documents/213E6A3B/63A5A569/Datasheet_SHT3x_DIS.pdf)

- **ESP-IDF Framework:** Developed using the ESP-IDF (Espressif IoT Development Framework) for efficient programming and hardware control.

- **SHT31 Sensor:** Utilizes the SHT31 temperature/humidity sensor for accurate environmental data.

- **MQTT Communication:** Sends temperature and humidity data to an MQTT broker over Wi-Fi.

- **I2C Communication:** Utilizes I2C communication to read data from the SHT31 sensor.

- **Voltage Monitoring:** Voltage monitoring is achieved by using a voltage divider mosfet combo. Voltage monitoring can be easily turned on/off to save power. 

## Getting Started

1. **Hardware Setup:** Connect the ESP to the SHT31 sensor using I2C communication.

2. **Software Setup:**
   For these steps it's prefellable to use platform.io
   - Install the ESP-IDF development environment.
   - Clone this repository.
   - Configure the project for your ESP board.
   - Configure the MQTT and WIFI settings using kconfig
   - The deep sleep time can easily be adjusted in `main.c`.
   - Build and flash the project to the ESP32S2 Mini.

4. **MQTT Configuration:** Set up your MQTT broker and configure the project with the broker's credentials.

5. **Operation:** After the software is running ESP will switch between deep sleep mode and running mode according the interval set in the `main.c`.

