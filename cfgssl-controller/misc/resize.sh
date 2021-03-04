#!/bin/bash
curl -m 10 --silent --output /dev/null --request POST http://localhost:5000/resize
exit $?
