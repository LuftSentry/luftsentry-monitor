void InitMqtt()
{
	espClient.setCACert(pRead_rootca);
	espClient.setCertificate(pRead_cert);
	espClient.setPrivateKey(pRead_privatekey);
	mqttClient.setServer(MQTT_BROKER_ADRESS, MQTT_PORT);
	mqttClient.setCallback(OnMqttReceived);
}

void ConnectMqtt()
{
	while (!mqttClient.connected())
	{
		Serial.println("Starting MQTT connection...");
		if (mqttClient.connect(MQTT_CLIENT_NAME))
		{
			//mqttClient.subscribe(OTA_TOPIC);
			mqttClient.subscribe(DATA_TOPIC);
			Serial.println("Success MQTT connection");
		}
		else
		{
			Serial.print("Failed MQTT connection, rc=");
			Serial.print(mqttClient.state());
			Serial.println(" try again in 5 seconds");

			delay(5000);
		}
	}
}

void HandleMqtt()
{
	if (!mqttClient.connected())
	{
		ConnectMqtt();
	}
	mqttClient.loop();
}