#! /bin/sh
mosquitto_sub -d -h $AWS_HOST   \
              -t "\$aws/things/test/shadow/update" -p 8883        \
              --cafile certs/amazon-ca.pem.crt                          \
              --cert certs/certificate.pem.crt                          \
              --key certs/private.pem.key                               \
              -q 1