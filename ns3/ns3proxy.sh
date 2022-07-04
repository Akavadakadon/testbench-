#!/bin/bash
## INFO
## eth0 - interface for client
## eth1 - custom unused
## eth2 - interface for server
brctl addbr br_sim1
ifconfig br_sim1 0.0.0.0 promisc up

brctl addbr br_sim2
ifconfig br_sim2 0.0.0.0 promisc up

tunctl -t tap_sim1
ifconfig tap_sim1 0.0.0.0 promisc up

tunctl -t tap_sim2
ifconfig tap_sim2 0.0.0.0 promisc up

ifconfig eth0 0.0.0.0 promisc up

ifconfig eth2 0.0.0.0 promisc up

brctl addif br_sim1 tap_sim1
brctl addif br_sim1 eth0

brctl addif br_sim2 tap_sim2
brctl addif br_sim2 eth2

ifconfig br_sim1 up

ifconfig br_sim2 up
