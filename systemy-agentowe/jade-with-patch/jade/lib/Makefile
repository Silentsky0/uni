FILES = cichy.jks cichy-ca.jks main-cichy.cer cichy.cer 

help:
	@echo "available commands:"
	@echo "		gen-keys"
	@echo "		export-certs"
	@echo "		import-certs"
	@echo "		main-container" 
	@echo "		backup-container"

gen-keys:
	keytool -genkeypair -keyalg RSA \
	-alias cichy \
	-keystore cichy.jks \
	-keypass storepass123 -storepass storepass123 \
	-dname "CN=cichy,OU=Esovisco,O=FooBarLtd,L=Gdansk,S=pomorskie,C=PL"

export-certs:
	keytool -export -rfc \
	-keystore cichy.jks \
	-storepass storepass123 \
	-alias cichy \
	-file cichy.cer

import-keys:
	keytool -import -noprompt \
	-keystore cichy-ca.jks \
	-storepass storepass123 \
	-alias ziemiec \
	-file ziemiec.cer

main-container:
	java \
	-Djavax.net.ssl.keyStore=cichy.jks \
	-Djavax.net.ssl.keyStorePassword=storepass123 \
	-Djavax.net.ssl.trustStore=cichy-ca.jks \
	-cp jade.jar jade.Boot \
	-host 192.168.31.224 -port 5656 \
	-local-host 192.168.31.224 -local-port 5656 \
	-gui \
	-name cichy -container-name cichy \
	-nomtp -icps jade.imtp.leap.JICP.JICPSPeer \
	-services jade.core.replication.MainReplicationService\;\
	jade.core.replication.AddressNotificationService\;\
	jade.core.event.NotificationService\;\
	jade.core.mobility.AgentMobilityService\;\

main-container-not-secure:
	java \
	-cp jade.jar jade.Boot \
	-host 25.36.78.210­ -port 5656 \
	-local-host 25.36.78.210 -local-port 5656 \
	-gui \
	-name cichy-host -container-name cichy \
	-services jade.core.replication.MainReplicationService\; \
	jade.core.replication.AddressNotificationService

backup-container:
	java \
	-Djavax.net.ssl.keyStore=cichy.jks \
	-Djavax.net.ssl.keyStorePassword=storepass123 \
	-Djavax.net.ssl.trustStore=cichy-ca.jks \
	-cp jade.jar jade.Boot \
	-host 25.36.78.210­ -port 5656 \
	-local-host 25.32.8.82 -local-port 4646 \
	-name cichy -container-name cichy-backup-container \
	-container -backupmain \
	-services jade.core.replication.MainReplicationService

federated-container:
	java \
    -Djavax.net.ssl.keyStore=cichy.jks \
    -Djavax.net.ssl.keyStorePassword=storepass123 \
    -Djavax.net.ssl.trustStore=cichy-ca.jks \
    -cp jade.jar jade.Boot \
    -host 25.32.8.82 -port 5656 \
    -local-host 25.36.78.210 -local-port 4646 \
    -container \
    -container-name cichy \
    -nomtp -icps jade.imtp.leap.JICP.JICPSPeer \
    -services jade.core.replication.MainReplicationService


main-container-lab-1:
	java -cp jade.jar jade.Boot \
	-host 192.168.31.224 -port 5656 \
	-local-host 192.168.31.224 -local-port 5656 \
	-gui \
	-name cichy-host -container-name cichy \
	-services jade.core.replication.MainReplicationService\; \
	jade.core.replication.AddressNotificationService

clean:
	rm -f $(FILES)


