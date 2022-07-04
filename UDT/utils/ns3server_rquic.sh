#!/bin/bash
echo "updating interface"
#ifconfig eth1 192.168.44.101
echo "setting route"
ip route add 192.168.42.0/24 via 192.168.44.201
ip route add 192.168.43.0/24 via 192.168.44.201

