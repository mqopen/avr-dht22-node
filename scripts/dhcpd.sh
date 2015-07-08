#!/bin/bash
ip addr add 192.168.7.111/24 dev eth1
ip link set dev eth1 up
ip route add dev eth1
rm dhcpd.leases*
touch dhcpd.leases
dhcpd -f -d -cf dhcpd.conf -lf dhcpd.leases eth1
