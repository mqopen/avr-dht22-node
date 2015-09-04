# IoT Node

Network enabled device for data exchange using MQTT protocol. This device periodically
reads data from [DHT22](http://www.aosong.com/en/products/details.asp?id=117) sensor
and publish them to network over MQTT protocol.

### Wiring

![IoT node wiring diagram](https://raw.githubusercontent.com/buben19/iot-node/master/pcb/mqtt-node.png "IoT node wiring diagram")

### Configuration

Edit following values config.h configuration file:

- `ETH_ADDR0` ... `ETH_ADDR5` - edit those values to unique MAC address
- `CONFIG_IP_ADDR0` ... `CONFIG_IP_ADDR` - edit those values to assign LAN address
- `CONFIG_NETMASK0` ... `CONFIG_NETMASK3` - edit those values to assign netmask
- `MQTT_BROKER_IP_ADDR0` ... `MQTT_BROKER_IP_ADDR0` - edit those values to assign MQTT broker IP address
- `MQTT_BROKER_PORT` - configure MQTT broker port
- `MQTT_TOPIC_TEMPERATURE` - configure temperature topic name
- `MQTT_TOPIC_HUMIDITY` - configure humidity topic name
- `MQTT_KEEP_ALIVE` - MQTT keep alive interval
- `MQTT_CLIENT_ID` - MQTT client ID

### Data output

Device sends humidity and temperature measurements on topic `MQTT_TOPIC_HUMIDITY` and
`MQTT_TOPIC_TEMPERATURE` respectively. When reading from sensors is successful,
payload is real positive (humidity, temperature) or negative (temperature only) number.

When reading from sensor fails, payload for each topic is appropriate error code:

- `E_CHECKSUM` - Data checksum is incorrect.
- `E_TIMEOUT` - Data reading timeouted.
- `E_CONNECT` - Sensor connection was failed.
- `E_ACK_L` - Low state acknowledgement fails.
- `E_ACK_H` - High state acknowledgement fails.

### Building

After configuration is done, build IoT node software with command `make`

### Upload

To upload software into AVR use command `make avrdude`

### Development

Node has implemented code for DHCP client to dynamically assign IP address. This
feature is currently in experimental phase and it is not well tested. Future
versions should also include DNS client to obtain IP address of MQTT broker from
local DNS server.
