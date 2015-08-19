# IoT Node

Network enabled device for data exchange using MQTT protocol.

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

### Building

After configuration is done, build IoT node software with command `make`
