# Environmental Monitoring Station with ESP32 and Blynk

This project is an environmental monitoring station that uses the ESP32, Blynk, and several sensors to measure various weather parameters, including temperature, humidity, air quality, wind speed, and weather conditions. The device can automatically connect to Wi-Fi, gather weather data from OpenWeatherMap, and alert users of changes in weather patterns, such as rain or sunshine.

## Features
- **Real-Time Monitoring**: Continuously reads and updates data for temperature, humidity, wind speed, air quality index (AQI), and rainfall status.
- **Weather Alerts**: Notifies users via Blynk when there is a weather change (e.g., sunny to rainy).
- **Auto Wi-Fi Connection**: Uses WiFiManager for easy configuration and automatic reconnection.
- **MQ135 Sensor for Air Quality**: Measures the concentration of CO2 and calculates AQI.
- **AHT20 for Temperature and Humidity**: Provides accurate readings for environmental temperature and humidity.
- **Wind Speed Measurement**: Calculates wind speed using a wind sensor based on pulse counting.

## Components Used
- **ESP32**: Main microcontroller for connecting sensors and communicating with Blynk and OpenWeatherMap API.
- **MQ135 Air Quality Sensor**: Measures air quality and calculates AQI based on CO2 levels.
- **AHT20 Temperature and Humidity Sensor**: Measures ambient temperature and humidity.
- **Rain Sensor**: Detects the presence of rain and provides information on weather conditions.
- **Wind Speed Sensor**: Measures wind speed by counting pulses generated by the wind sensor.
- **Blynk**: Used for remote data monitoring and event logging.

## Setup and Configuration
1. **Library Installation**: Install the following libraries using the Arduino IDE Library Manager:
   - `WiFiManager`
   - `HTTPClient`
   - `Arduino_JSON`
   - `MQ135`
   - `AHT20`
   - `BlynkSimpleEsp32`
2. **WiFiManager Setup**: The program uses WiFiManager to set up a configuration portal for the ESP32. If the ESP32 fails to connect to Wi-Fi, it will start a Wi-Fi access point named "AutoConnectAP" with the password "12345678" to allow you to configure your network.
3. **Blynk Authentication Token**: Replace the `BLYNK_AUTH_TOKEN` in the code with your own Blynk token.
4. **API Key**: Register on OpenWeatherMap and replace the `API_KEY` in the code with your own key.
5. **GPS Coordinates**: Update the `LATITUDE` and `LONGITUDE` values with your device's location.

## Code Walkthrough

### 1. Initialization
In the `setup()` function:
   - The device initializes all sensor pins and starts serial communication.
   - It uses WiFiManager to connect to Wi-Fi, and then connects to Blynk using stored credentials.

### 2. Main Loop
In the `loop()` function:
   - Sensor data is read and updated in the Blynk app every second.
   - The `Blynk.run()` function processes communication with the Blynk server.

### 3. Sensor Data Collection
In the `readSensorData()` function:
   - Reads temperature and humidity from the AHT20 sensor.
   - Collects data from the MQ135 sensor to calculate the AQI using temperature and humidity for accurate readings.
   - Reads wind speed using a pulse counter and rain status using a digital read.

### 4. Weather Change Detection
In the `checkWeatherChange()` function:
   - Monitors the weather data from OpenWeatherMap API.
   - Sends an alert to Blynk if there is a change from sunny to rainy weather or vice versa.

## Example API Call
The device constructs the OpenWeatherMap API URL as follows:
http://api.openweathermap.org/data/2.5/weather?lat=LATITUDE&lon=LONGITUDE&units=metric&exclude=hourly,daily&appid=YOUR_API_KEY


## Troubleshooting
- **Wi-Fi Issues**: If the device fails to connect to Wi-Fi, check WiFiManager's settings. It may be necessary to clear stored network credentials by resetting WiFiManager.
- **Blynk Connection**: Ensure you have a stable internet connection and your Blynk token is correct.

## Acknowledgements
This project is made possible by open-source libraries and APIs from OpenWeatherMap, WiFiManager, and Blynk.
