#!/bin/bash
curl -m 20 --silent --output /dev/null --request POST http://localhost:5000/star
exit $?
