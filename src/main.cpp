#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <config.hpp>
#include <programmer.hpp>
#include <protobuf.hpp>

using namespace RobotTweezers;

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

/**
 * @brief Wait for an ackowledgement from the Teensy before beginning program
 *
 */
static void WaitForConnection(void)
{
    UartConnection connection_msg, response;
    int8_t attempts = 5;

    response.id = ESP32_ID;

    while (attempts--)
    {
        if (Protobuf::UartRead(&Serial, &connection_msg))
        {
            if (connection_msg.id == TEENSY_ID)
            {
                digitalWrite(LED_BUILTIN, HIGH);
                Protobuf::UartWrite(&Serial, &response);
                break;
            }
        }

        delay(1000);
    }
}

void setup()
{
    Serial.begin(PROTOBUF_INTERFACE_BAUDRATE);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    delay(1000);
    WaitForConnection();

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        //Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    Programmer::Start(&Serial2);

    //Serial.println("Ready");
    //Serial.print("IP address: ");
    //Serial.println(WiFi.localIP());
}

void loop()
{
    Programmer::Handle();
    delay(100);
}
