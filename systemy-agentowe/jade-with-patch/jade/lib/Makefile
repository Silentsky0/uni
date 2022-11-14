FILES = $(KEYSTORE) $(KEYSTORE_FED1) $(KEYSTORE_FED2) $(MY_ALIAS)-ca.jks $(MY_ALIAS).cer $(MY_ALIAS_FED1).cer $(MY_ALIAS_FED2).cer

KEYSTORE = cichy.jks
KEYSTORE_FED1 = cichy_fed1.jks
KEYSTORE_FED2 = cichy_fed2.jks

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

IMPORT_KEYS_MAIN = $(MY_ALIAS_FED1).cer $(MY_ALIAS_FED2).cer $(ALIAS_1).cer $(ALIAS_2).cer $(ALIAS_1)_fed1.cer $(ALIAS_1)_fed2.cer $(ALIAS_2)_fed1.cer $(ALIAS_2)_fed2.cer

help:
	@echo "available commands:"
	@echo "		gen-keys"
	@echo "		export-certs"
	@echo "		import-certs"
	@echo "		main-container" 
	@echo "		backup-container"

initial-configuration: create-java-security gen-keys export-certs

create-java-security:
	@echo "jdk.tls.disabledAlgorithms=" > java.security

gen-keys:
	keytool -genkeypair -keyalg RSA -storetype JKS \
	-alias $(MY_ALIAS) \
	-keystore $(KEYSTORE) \
	-keypass $(KEY_PASS) -storepass $(STORE_PASS) \
	-dname "CN=$(MY_ALIAS),OU=Esovisco,O=Esoviscode,L=Gdansk,S=pomorskie,C=PL"

	keytool -genkeypair -keyalg RSA -storetype JKS \
	-alias $(MY_ALIAS_FED1) \
	-keystore $(KEYSTORE_FED1) \
	-keypass $(KEY_PASS) -storepass $(STORE_PASS) \
	-dname "CN=$(MY_ALIAS_FED1),OU=Esovisco,O=Esoviscode,L=Gdansk,S=pomorskie,C=PL"

	keytool -genkeypair -keyalg RSA -storetype JKS \
	-alias $(MY_ALIAS_FED2) \
	-keystore $(KEYSTORE_FED2) \
	-keypass $(KEY_PASS) -storepass $(STORE_PASS) \
	-dname "CN=$(MY_ALIAS_FED2),OU=Esovisco,O=Esoviscode,L=Gdansk,S=pomorskie,C=PL"

export-certs:
	keytool -export -rfc \
	-storetype JKS \
	-keystore $(KEYSTORE) \
	-storepass $(STORE_PASS) \
	-alias $(MY_ALIAS) \
	-file $(MY_ALIAS).cer

	keytool -export -rfc \
	-storetype JKS \
	-keystore $(KEYSTORE_FED1) \
	-storepass $(STORE_PASS) \
	-alias $(MY_ALIAS_FED1) \
	-file $(MY_ALIAS_FED1).cer

	keytool -export -rfc \
	-storetype JKS \
	-keystore $(KEYSTORE_FED2) \
	-storepass $(STORE_PASS) \
	-alias $(MY_ALIAS_FED2) \
	-file $(MY_ALIAS_FED2).cer

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
	-Djavax.net.ssl.keyStore=$(MY_ALIAS).jks \
	-Djavax.net.ssl.keyStoreType=jks \
	-Djavax.net.ssl.keyStorePassword=$(STORE_PASS) \
	-Djavax.net.ssl.trustStore=$(MY_ALIAS)-ca.jks \
	-Djavax.net.ssl.trustStoreType=jks \
	-Djava.security.properties=java.security \
	-cp jade.jar jade.Boot \
	-container-name $(MY_ALIAS) \
	-name $(MY_ALIAS) \
	-host $(MAIN_IP) \
	-port $(MAIN_PORT) \
	-nomtp -icps jade.imtp.leap.JICP.JICPSPeer\($(MAIN_PORT)\) \
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
	-Djava.security.properties=java.security \
	-cp jade.jar jade.Boot \
	-container-name $(MY_ALIAS) \
	-backupmain \
	-host $(MAIN_IP) \
	-port $(MAIN_PORT) \
	-nomtp -icps jade.imtp.leap.JICP.JICPSPeer\(2137\) \
	-services jade.core.replication.MainReplicationService\;\
	jade.core.replication.AddressNotificationService\;\
	jade.core.event.NotificationService\;\
	jade.core.mobility.AgentMobilityService\;

federated-container-1:
	java \
	-Djavax.net.ssl.keyStore=$(MY_ALIAS).jks \
	-Djavax.net.ssl.keyStoreType=jks \
	-Djavax.net.ssl.keyStorePassword=$(STORE_PASS) \
	-Djavax.net.ssl.trustStore=$(MY_ALIAS)-ca.jks \
	-Djavax.net.ssl.trustStoreType=jks \
	-Djava.security.properties=java.security \
	-cp jade.jar jade.Boot \
	-container-name $(MY_ALIAS_FED1) \
	-container \
	-host $(MAIN_IP) \
	-port $(MAIN_PORT) \
	-nomtp -icps jade.imtp.leap.JICP.JICPSPeer\(2138\) \
	-services jade.core.replication.AddressNotificationService\;\
	jade.core.event.NotificationService\;\
	jade.core.mobility.AgentMobilityService\;

federated-container-2:
	java \
	-Djavax.net.ssl.keyStore=$(MY_ALIAS).jks \
	-Djavax.net.ssl.keyStoreType=jks \
	-Djavax.net.ssl.keyStorePassword=$(STORE_PASS) \
	-Djavax.net.ssl.trustStore=$(MY_ALIAS)-ca.jks \
	-Djavax.net.ssl.trustStoreType=jks \
	-Djava.security.properties=java.security \
	-cp jade.jar jade.Boot \
	-container-name $(MY_ALIAS_FED2) \
	-container \
	-host $(MAIN_IP) \
	-port $(MAIN_PORT) \
	-nomtp -icps jade.imtp.leap.JICP.JICPSPeer\(2139\) \
	-services jade.core.replication.AddressNotificationService\;\
	jade.core.event.NotificationService\;\
	jade.core.mobility.AgentMobilityService\;

import-all-keys: import-all-keys-main import-all-keys-fed-1 import-all-keys-fed-2

import-all-keys-main:
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

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(MY_ALIAS_FED1) \
	-file $(MY_ALIAS_FED1).cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(MY_ALIAS_FED2) \
	-file $(MY_ALIAS_FED2).cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(ALIAS_1)_fed1 \
	-file $(ALIAS_1)_fed1.cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(ALIAS_1)_fed2 \
	-file $(ALIAS_1)_fed2.cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(ALIAS_2)_fed1 \
	-file $(ALIAS_2)_fed1.cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(ALIAS_2)_fed2 \
	-file $(ALIAS_2)_fed2.cer

import-all-keys-fed-1:
	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS_FED1)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(ALIAS_1) \
	-file $(ALIAS_1).cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS_FED1)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(ALIAS_2) \
	-file $(ALIAS_2).cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS_FED1)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(MY_ALIAS) \
	-file $(MY_ALIAS).cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS_FED1)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(MY_ALIAS_FED2) \
	-file $(MY_ALIAS_FED2).cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS_FED1)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(ALIAS_1)_fed1 \
	-file $(ALIAS_1)_fed1.cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS_FED1)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(ALIAS_1)_fed2 \
	-file $(ALIAS_1)_fed2.cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS_FED1)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(ALIAS_2)_fed1 \
	-file $(ALIAS_2)_fed1.cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS_FED1)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(ALIAS_2)_fed2 \
	-file $(ALIAS_2)_fed2.cer

import-all-keys-fed-2:
	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS_FED2)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(ALIAS_1) \
	-file $(ALIAS_1).cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS_FED2)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(ALIAS_2) \
	-file $(ALIAS_2).cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS_FED2)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(MY_ALIAS) \
	-file $(MY_ALIAS).cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS_FED2)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(MY_ALIAS_FED1) \
	-file $(MY_ALIAS_FED1).cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS_FED2)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(ALIAS_1)_fed1 \
	-file $(ALIAS_1)_fed1.cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS_FED2)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(ALIAS_1)_fed2 \
	-file $(ALIAS_1)_fed2.cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS_FED2)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(ALIAS_2)_fed1 \
	-file $(ALIAS_2)_fed1.cer

	keytool -import -noprompt -storetype JKS \
	-keystore $(MY_ALIAS_FED2)-ca.jks \
	-storepass $(STORE_PASS) \
	-alias $(ALIAS_2)_fed2 \
	-file $(ALIAS_2)_fed2.cer

clean:
	rm -f $(FILES)


