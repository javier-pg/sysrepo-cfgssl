#!/bin/bash
curl --silent --output /dev/null --header "Content-Type: application/json" --data "{\"arrival_time\":\"$1\"}" --request POST http://localhost:5000/resize
exit $?
