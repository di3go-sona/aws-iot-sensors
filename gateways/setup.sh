#!/bin/sh
export GATEWAY_CSR="gateway.csr.pem"
export GATEWAY_CERT="gateway.cert.pem"
export GATEWAY_KEY="gateway.privkey.pem"
export AWS_CA="aws-ca.cert.pem"
export AWS_HOST="a3m1d3k4nlux10-ats.iot.eu-central-1.amazonaws.com"

./gateway_certs.sh
./mqtt.sh  
# ./mqtt-sn.sh