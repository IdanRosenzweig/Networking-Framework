#!/bin/bash

sudo ip link set dev tap0 up

sudo ip link add br0 type bridge
sudo ip link set up br0

sudo ip addr flush enp0s3

sudo ip link set enp0s3 master br0

sudo dhclient -i br0

sudo ip link set tap0 master br0


#sudo ip link add link enp0s3 name vnic0 address 00:11:22:33:44:55 type macvlan mode bridge
#sudo ip link set vnic0 up


#sudo ip tuntap add name tap1 mode tap
#sudo ip link set tap1 up
#sudo ip link set tap1 promisc on
#sudo ip addr add 10.100.102.33/24 dev tap1
#sudo ip route add dev tap1 10.100.102.0/24
#
#sudo ip link add name br10 type bridge
#sudo ip link set dev br10 up
#
#sudo ip link set enp0s3 master br10
#sudo ip link set tap1 master br10
