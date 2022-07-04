#!/bin/bash
# ifconfig eth0 192.168.42.101
echo "setting interface for client"
ip route add 192.168.43.0/24 via 192.168.42.201
ip route add 192.168.44.0/24 via 192.168.42.201
