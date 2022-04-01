#include <Arduino.h>
#include <HardwareSerial.h>
#include <WiFi.h>

#include <stdlib.h>

#include <pb_encode.h>
#include <pb_decode.h>

#include "remote.pb.h"

uint8_t inbuffer[128];
uint8_t outbuffer[128];
bool status;
int message_length;

WiFiServer wifiServer(80);

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin("BurstYe", "RawTrut5ch");
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  wifiServer.begin();
}

void wifi_loop() {
	WiFiClient client = wifiServer.available();

	int size;

	if (client) {
		while (client.connected()) {

			size = client.available();

			if (size > 0) {
				client.readBytes(inbuffer, sizeof(inbuffer));
				Serial.println(size);

				client.write(inbuffer, sizeof(inbuffer));
			}

			delay(10);
		}

		client.stop();
		Serial.println("Client disconnected");
	}

	Serial.print("Waiting for client... IP address: ");
	Serial.println(WiFi.localIP());
	delay(1000);
}

void setup() {
	initWiFi();
}

void loop() {
	wifi_loop()
}
