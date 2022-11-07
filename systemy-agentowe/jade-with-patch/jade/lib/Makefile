FILES = cichy.jks cichy-ca.jks main-cichy.cer

help:
	@echo "available commands:"
	@echo "		gen-keys"
	@echo "		export-certs"
	@echo "		import-certs"
	@echo "		main-container" 
	@echo "		backup-container"

gen-keys:
	keytool -genkeypair -keyalg RSA -alias cichy -keystore cichy.jks -keypass keypass123 -storepass storepass123 -dname "CN=Giada,OU=JADE,O=FooBarLtd,L=Naples,ST=na,C=IT"

export-certs:
	keytool -export -keystore cichy.jks -storepass storepass123 -alias cichy -rfc -file main-cichy.cer

import-keys:
	keytool -import -keystore cichy-ca.jks -storepass storepass123 -alias ziemiec -file main-ziemiec.cer -noprompt

main-container:
	java \
	-Djavax.net.ssl.keyStore=cichy.jks \
	-Djavax.net.ssl.keyStorePassword=storepass123 \
	-Djavax.net.ssl.trustStore=cichy-ca.jks \
	-cp jade.jar jade.Boot \
	-host 25.36.78.210­ -port 5656 \
	-local-host 25.36.78.210 -local-port 5656 \
	-gui \
	-name cichy-host -container-name cichy \
	-nomtp -icps jade.imtp.leap.JICP.JICPSPeer \
	-services jade.core.replication.MainReplicationService\; \
	jade.core.replication.AddressNotificationService

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
	-host 25.36.78.210 -port 5656 \
	-local-host 25.36.78.210 -local-port 5656 \
	-gui \
	-name cichy-host -container-name cichy \
	-services jade.core.replication.MainReplicationService\; \
	jade.core.replication.AddressNotificationService

clean:
	rm -f $(FILES)


