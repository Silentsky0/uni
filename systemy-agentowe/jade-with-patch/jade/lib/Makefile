
help:
	@echo "available commands:"
	@echo "gen-keys"
	@echo "export-certs"
	@echo "main-container" 

gen-keys:
	keytool -genkey -alias cichy -keyalg RSA -keystore cichy.jks -keypass keypass123 -storepass storepass123 -dname "CN=Giada,OU=JADE,O=FooBarLtd,L=Naples,ST=na,C=IT"

export-certs:
	keytool -export -keystore cichy.jks -storepass storepass123 -alias cichy -rfc -file main-cichy.cer

import-keys:
	keytool -import -keystore cichy-ca.jks -storepass storepass123 -alias cimek -file $(name) -noprompt

main-container:
	java \
	-Djavax.net.ssl.keyStore=cichy.jks \
	-Djavax.net.ssl.keyStorePassword=storepass123 \
	-Djavax.net.ssl.trustStore=cichy-ca.jks \
	-cp jade.jar jade.Boot \
	-host 25.36.78.210­ -port 5656 \
	-local-host 25.36.78.210 -local-port 5656 \
	-gui \
	-name cichy -container-name cichy-main-container \
	-nomtp -icps jade.imtp.leap.JICP.JICPSPeer \
	-services jade.core.replication.MainReplicationService\; \
	jade.core.replication.AddressNotificationService

