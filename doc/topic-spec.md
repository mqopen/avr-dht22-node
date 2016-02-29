# IoT topic specification

This document describes brief specification of MQTT topic structure for **mqopen** IoT network.

## Data topics

 - `<location>/<quantity>` - Main device sensor measurement.
 - `<location>/<quantity>/error` - Error code indicating sensor read failure.

### Where

 - `<location>` - Location in designated area (example: `living-room`).
 - `<quantity>` - Measurement physical quantity (example: `temperature`).

## Service topics

All service topics should set retain flag.

 - `info/<devname>/hwversion` - HW version.
 - `info/<devname>/fwversion` - FW version.
 - `info/<devname>/presence` - Presence message.
   - `online` - Device is online.
   - `offline` - Device is offline. This message is sent by the broker using last will message.
 - `info/<devname>/arch` - Device architecture.
   - `avr` - AVR based devices.
   - `esp` - ESP based devices.
 - `info/<devname>/variant` - Procesor model.
   - `atmega328p` - ATMega 328p chip.
   - `esp8266` - ESP8266 chip.
 - `info/<devname>/link` - Link information.
   - `ethernet` - Ethernet LAN connection.
   - `wifi` - Wi-Fi connection.
   - `nrf24` - NRF24 wireless connection.
 - `info/<devname>/voltage` - Input voltage. For battery powered devices.
 - `info/<devname>/ip` - Device IP address.

### Where

 - `<devname>` - Device name.
