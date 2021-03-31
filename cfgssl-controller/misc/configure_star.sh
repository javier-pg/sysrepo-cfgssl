#!/bin/bash
curl -m 300 --silent --output /dev/null --request POST http://localhost:5000/star
exit $?
