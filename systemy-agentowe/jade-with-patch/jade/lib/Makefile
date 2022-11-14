FILES = $(KEYSTORE) cichy-ca.jks main-cichy.cer cichy.cer 

KEYSTORE = cichy.jks
MY_ALIAS = cichy
MY_ALIAS_FED1 = $(MY_ALIAS)_fed1
MY_ALIAS_FED2 = $(MY_ALIAS)_fed2

MAIN_IP = 25.45.86.160
MY_IP = 25.36.78.210

STORE_PASS = storepass123
KEY_PASS = storepass123
ALIAS_1 = cellmer
ALIAS_2 = ziemiec

MAIN_PORT = 5656
AUXILIARY_PORT = 2137

help:
	@echo "available commands:"
	@echo "		gen-keys"
	@echo "		export-certs"
	@echo "		import-certs"
	@echo "		main-container" 
	@echo "		backup-container"

gen-keys:
	keytool -genkeypair -keyalg RSA -storetype JKS \
	-alias $(MY_ALIAS) \
	-keystore $(KEYSTORE) \
	-keypass $(KEY_PASS) -storepass $(STORE_PASS) \
	-dname "CN=$(MY_ALIAS),OU=Esovisco,O=Esoviscode,L=Gdansk,S=pomorskie,C=PL"
	keytool -genkeypair -keyalg RSA -storetype JKS \
	-alias $(MY_ALIAS) \
	-keystore $(KEYSTORE) \
	-keypass $(KEY_PASS) -storepass $(STORE_PASS) \
	-dname "CN=$(MY_ALIAS),OU=Esovisco,O=Esoviscode,L=Gdansk,S=pomorskie,C=PL"
	keytool -genkeypair -keyalg RSA -storetype JKS \
	-alias $(MY_ALIAS) \
	-keystore $(KEYSTORE) \
	-keypass $(KEY_PASS) -storepass $(STORE_PASS) \
	-dname "CN=$(MY_ALIAS),OU=Esovisco,O=Esoviscode,L=Gdansk,S=pomorskie,C=PL"

export-certs:
	keytool -export -rfc \
	-storetype JKS \
	-keystore $(KEYSTORE) \
	-storepass $(STORE_PASS) \
	-alias $(MY_ALIAS) \
	-file $(MY_ALIAS).cer

import-keys:
	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(ALIAS_1) \
	-file $(ALIAS_1).cer
	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(ALIAS_2) \
	-file $(ALIAS_2).cer

main-container:
	java \
	-Djava.security.properties=java.security \
	-Djavax.net.ssl.keyStore=$(MY_ALIAS).jks \
	-Djavax.net.ssl.keyStoreType=jks \
	-Djavax.net.ssl.keyStorePassword=$(STORE_PASS) \
	-Djavax.net.ssl.trustStore=$(MY_ALIAS)-ca.jks \
	-Djavax.net.ssl.trustStoreType=jks \
	-cp jade.jar jade.Boot \
	-local-host $(MY_IP) -local-port $(MAIN_PORT) \
	-gui \
	-name $(MY_ALIAS) -container-name $(MY_ALIAS) \
	-nomtp -icps jade.imtp.leap.JICP.JICPSPeer \
	-services jade.core.replication.MainReplicationService\;\
	jade.core.replication.AddressNotificationService\;\
	jade.core.event.NotificationService\;\
	jade.core.mobility.AgentMobilityService\;

backup-container:
	java \
	-Djavax.net.ssl.keyStore=$(MY_ALIAS).jks \
	-Djavax.net.ssl.keyStoreType=jks \
	-Djavax.net.ssl.keyStorePassword=$(STORE_PASS) \
	-Djavax.net.ssl.trustStore=$(MY_ALIAS)-ca.jks \
	-Djavax.net.ssl.trustStoreType=jks \
	-cp jade.jar jade.Boot \
	-container-name $(MY_ALIAS) \
	-backupmain \
	-nomtp -icps jade.imtp.leap.JICP.JICPSPeer\($(AUXILIARY_PORT)\) \
	-host $(MAIN_IP)­ -port $(MAIN_PORT) \
	-services jade.core.replication.MainReplicationService\;\
	jade.core.replication.AddressNotificationService\;\
	jade.core.event.NotificationService\;\
	jade.core.mobility.AgentMobilityService\;

federated-container:
	java \
	-Djavax.net.ssl.keyStore=$(MY_ALIAS).jks \
	-Djavax.net.ssl.keyStoreType=jks \
	-Djavax.net.ssl.keyStorePassword=$(STORE_PASS) \
	-Djavax.net.ssl.trustStore=$(MY_ALIAS)-ca.jks \
	-Djavax.net.ssl.trustStoreType=jks \
	-cp jade.jar jade.Boot \
	-container-name $(MY_ALIAS) \
	-container \
	-host $(MAIN_IP)­ -port $(MAIN_PORT) \
	-nomtp -icps jade.imtp.leap.JICP.JICPSPeer\($(AUXILIARY_PORT)\) \
	-services jade.core.replication.AddressNotificationService\;\
	jade.core.event.NotificationService\;\
	jade.core.mobility.AgentMobilityService\;

test:
	java -cp jade.jar jade.Boot \
	-host $(MY_IP) -port 5656 \
	-local-host $(MY_IP) -local-port 5656 \
	-gui \
	-name cichy-host -container-name cichy

test2:
	java -cp jade.jar jade.Boot \
	-Djavax.net.ssl.keyStore=$(MY_ALIAS).jks \
	-Djavax.net.ssl.keyStoreType=jks \
	-Djavax.net.ssl.trustStoreType=jks \
	-Djavax.net.ssl.keyStorePassword=$(STORE_PASS) \
	-Djavax.net.ssl.trustStore=$(MY_ALIAS)-ca.jks \
	-container-name $(MY_ALIAS) \
	-host $(MAIN_IP) -port 5656 \
	-local-host $(MY_IP) -local-port 4646 \
	-container \
	-services jade.core.replication.AddressNotificationService\;\
	jade.core.event.NotificationService\;\
	jade.core.mobility.AgentMobilityService\;

test3:
	java \
	-Djavax.net.ssl.keyStore=$(MY_ALIAS).jks \
	-Djavax.net.ssl.keyStoreType=jks \
	-Djavax.net.ssl.keyStorePassword=$(STORE_PASS) \
	-Djavax.net.ssl.trustStore=$(MY_ALIAS)-ca.jks \
	-Djavax.net.ssl.trustStoreType=jks \
	-Djava.security.properties=java.security \
	-cp jade.jar jade.Boot \
	-container-name cichy \
	-container \
	-local-host 25.36.78.210 \
	-local-port 7777 \
	-host 25.45.86.160 \
	-port 5656 \
	-nomtp -icps jade.imtp.leap.JICP.JICPSPeer \
	-services jade.core.replication.MainReplicationService\;\
	jade.core.replication.AddressNotificationService\;\
	jade.core.event.NotificationService\;\
	jade.core.mobility.AgentMobilityService\; \

test4:
	java \
	-Djavax.net.ssl.keyStore=$(MY_ALIAS).jks \
	-Djavax.net.ssl.keyStoreType=jks \
	-Djavax.net.ssl.keyStorePassword=$(STORE_PASS) \
	-Djavax.net.ssl.trustStore=$(MY_ALIAS)-ca.jks \
	-Djavax.net.ssl.trustStoreType=jks \
	-Djava.security.properties=java.security \
	-cp jade.jar jade.Boot \
	-container-name cichy \
	-container \
	-host $(MAIN_IP) \
	-port $(MAIN_PORT) \
	-nomtp -icps jade.imtp.leap.JICP.JICPSPeer\(2137\) \
	-services jade.core.replication.AddressNotificationService\;

test5:
	java \
	-Djavax.net.ssl.keyStore=$(MY_ALIAS).jks \
	-Djavax.net.ssl.keyStoreType=jks \
	-Djavax.net.ssl.keyStorePassword=$(STORE_PASS) \
	-Djavax.net.ssl.trustStore=$(MY_ALIAS)-ca.jks \
	-Djavax.net.ssl.trustStoreType=jks \
	-Djava.security.properties=java.security \
	-cp jade.jar jade.Boot \
	-container-name cichy \
	-backupmain \
	-host $(MAIN_IP) \
	-port $(MAIN_PORT) \
	-nomtp -icps jade.imtp.leap.JICP.JICPSPeer\(2137\) \
	-services jade.core.replication.MainReplicationService\;\
	jade.core.replication.AddressNotificationService\;\
	jade.core.event.NotificationService\;\
	jade.core.mobility.AgentMobilityService\;

clean:
	rm -f $(FILES)


