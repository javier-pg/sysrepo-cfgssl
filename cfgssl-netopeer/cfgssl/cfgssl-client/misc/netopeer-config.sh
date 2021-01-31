#!/bin/bash
set -x

control_network_ip=$(ip route | tail -2 | head -1 | awk '{print $9}')
data_network_ip=$(ip route | tail -1 | awk '{print $9}')

cfg="{\"netopeer_type\":\"tls-client\",\"control_network_ip\":\"$control_network_ip\",\"data_network_ip\":\"$data_network_ip\"}"

curl --header "Content-Type: application/json" \
  --request POST \
  --data $cfg \
  http://10.0.1.200:5000/netopeerlist
