#!/bin/bash
curl -m 60 --silent --output /dev/null --request POST http://localhost:5000/star
exit $?
