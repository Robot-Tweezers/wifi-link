#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <config.hpp>
#include <protobuf.hpp>
#include <HardwareSerial.h>

using namespace RobotTweezers;

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

uint8_t inbuffer[128];
uint8_t outbuffer[128];
bool status;
int message_length;

WiFiServer wifiServer(80);

HardwareSerial s1 = HardwareSerial(1);

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin("iPhone", "tylertylertyler");
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  wifiServer.begin();
}

void decode_orientation_msg(unsigned char *buffer, int len) {
	OrientationMsg d = OrientationMsg_init_zero;
	pb_istream_t istream = pb_istream_from_buffer(buffer, len);
	status = pb_decode(&istream, OrientationMsg_fields, &d);

	if (!status) {
		printf("Decoding Failed: %s\n", PB_GET_ERROR(&istream));
		return;
	}



    bool success = Protobuf::UartWrite(&s1, &d);

	if (!success) {
		printf("Decoding Failed: %s\n", PB_GET_ERROR(&istream));
		return;
	} else {
        OrientationMsg overserial = OrientationMsg_init_zero;
        Protobuf::UartRead(&s1, &overserial);
        Serial.println("Data:");
        Serial.write("Roll: ");
        Serial.println(d.roll);
        Serial.write("Pitch: ");
        Serial.println(d.pitch);
        Serial.write("Yaw: ");
        Serial.println(d.yaw);
    }
}

void wifi_loop() {
	WiFiClient client = wifiServer.available();

	int size;

	if (client) {
		while (client.connected()) {
			size = client.available();

			if (size > 0) {
                Serial.print("Connected. Size: ");
                Serial.println(size);

				client.readBytes(inbuffer, sizeof(inbuffer));

                decode_orientation_msg(inbuffer, size);

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
    s1.begin(9600, SERIAL_8N1, GPIO_NUM_16, GPIO_NUM_17);
    Serial.begin(9600);
	initWiFi();
}

void loop() {
	wifi_loop();
}