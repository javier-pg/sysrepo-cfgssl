#!/bin/bash

control_network_ip=$(ip route | tail -2 | head -1 | awk '{print $9}')
data_network_ip=$(ip route | tail -1 | awk '{print $9}')
request=$(cat cert-request.pem | head -n9 | tail -n8 | tr -d '\n')

cfg="{\"control_network_ip\":\"$control_network_ip\",\"data_network_ip\":\"$data_network_ip\",\"cert_req\":\"$request\"}"

curl --silent --output /dev/null --header "Content-Type: application/json" \
  --request POST \
  --data $cfg \
  http://10.0.1.200:5000/register
