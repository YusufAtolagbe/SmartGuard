# SmartGuard

SmartGuard is an IoT-based home intrusion detection system built using the ESP32 microcontroller. It detects human movement using a Passive Infrared (PIR) sensor and instantly notifies the homeowner through a Telegram bot over a secure Wi-Fi connection.

The project demonstrates how low-cost embedded hardware can be combined with cloud messaging services to provide real-time home security alerts.

---

## Features

- Human motion detection using a PIR sensor
- Instant Telegram notifications when an intrusion is detected
- Secure communication with the Telegram Bot API using HTTPS/TLS
- Wi-Fi provisioning using WiFiManager (no hardcoded Wi-Fi credentials)
- Remote arming and disarming through Telegram commands
- Local audible alarm using a buzzer
- Interrupt-driven motion detection for efficient event handling

---

## Hardware Used

- ESP32 Development Board
- PIR Motion Sensor
- Active Buzzer
- Wi-Fi Network

---

## Software Libraries

- WiFiManager
- UniversalTelegramBot
- WiFiClientSecure
- ArduinoJson

---

## How It Works

When powered on, SmartGuard automatically attempts to connect to a previously saved Wi-Fi network. If no saved network is available, it creates a temporary Wi-Fi access point that allows the user to configure the device without modifying the source code.

After connecting to the internet, the ESP32 synchronizes its system time using an NTP server. This is required for validating Telegram's SSL certificate and establishing a secure HTTPS connection.

The user then sends the `/start` command to the Telegram bot. Once authenticated, SmartGuard stores the user's chat ID and enables remote control through Telegram.

The system can then be armed or disarmed using the following commands:

- `/on` – Enables intrusion detection
- `/off` – Disables intrusion detection

When the system is armed, the PIR sensor continuously monitors for human movement. Rather than constantly polling the sensor, SmartGuard uses hardware interrupts. As soon as motion is detected, the interrupt service routine sets a task flag, allowing the main program loop to safely handle the event.

When an intrusion is confirmed, SmartGuard:

1. Sends an intrusion alert to the authorized Telegram user.
2. Activates the local buzzer for an audible warning.
3. Returns to monitoring for future events.

This interrupt-driven architecture keeps the interrupt service routine lightweight while allowing network communication to occur safely in the main program.

---

## Project Structure

The firmware consists of four major components:

- **Wi-Fi Management** – Handles wireless network connection and configuration.
- **Telegram Interface** – Processes user commands and sends intrusion alerts.
- **Interrupt Handler** – Detects motion events from the PIR sensor.
- **Intrusion Task** – Executes the notification and alarm routine.

---

## Applications

SmartGuard can be used in:

- Homes
- Offices
- Small businesses
- Laboratories
- Warehouses
- Remote monitoring applications

---

## Future Improvements

Potential enhancements include:

- Camera integration using the ESP32-CAM
- Event logging
- Cloud dashboard
- Multiple authorized users
- Battery backup monitoring
- Mobile application
- Image capture upon intrusion detection

---

## License

This project is released under the MIT License.
