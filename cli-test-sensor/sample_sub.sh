#! /bin/sh
mosquitto_sub -d -h a3m1d3k4nlux10-ats.iot.eu-central-1.amazonaws.com   \
              -t "\$aws/things/test/shadow/update" -p 8883        \
              --cafile certs/amazon-ca.pem.crt                          \
              --cert certs/certificate.pem.crt                          \
              --key certs/private.pem.key                               \
              -q 1