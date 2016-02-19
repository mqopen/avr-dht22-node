# IoT node SW changelog

## v0.1

 - Initial version.
 - Functionality for reading data from DHT-22 sensor.
 - Sending data over MQTT
 - MQTT presence message with retain bit.
 - MQTT last will message.
 - Some basic code for DHCP client.
 - TCP/IP stack code take from uIP project.
 - Notification LED for signalling established TCP connection to MQTT broker and data transmit.
 - Implemented error codes: `E_CHECKSUM`, `E_TIMEOUT`, `E_CONNECT` and `E_ACK`.
