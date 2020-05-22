#!/bin/sh

set +x


GATEWAY_POLICY="iot-all"
GATEWAY_POLICY_ARN=$(aws iot list-policies --output text | grep -o 'arn.*/iot-all')


rm -rf certs
mkdir certs
cd certs


touch $GATEWAY_CERT
openssl genrsa -out $GATEWAY_KEY

openssl req -new -key $GATEWAY_KEY -out $GATEWAY_CSR -subj "/C=IT/ST=IT/L=Rome/O=Company, Inc./OU=IT/CN=iot-system.net"

GATEWAY_CERT_ARN=$( \
	aws iot create-certificate-from-csr \
		--certificate-signing-request=file://$GATEWAY_CSR \
		--certificate-pem-outfile=$GATEWAY_CERT \
		--set-as-active \
		| grep -o 'arn:aws:iot:eu-central-1:.*:cert/[0-9a-f]*' )

aws iot attach-policy \
	--policy-name $GATEWAY_POLICY\
	--target $GATEWAY_CERT_ARN

echo $GATEWAY_CERT_ARN
curl "https://www.amazontrust.com/repository/AmazonRootCA1.pem" > $AWS_CA