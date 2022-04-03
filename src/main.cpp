#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <config.hpp>
#include <programmer.hpp>
#include <protobuf.hpp>

using namespace RobotTweezers;

OrientationMsg msg;

WiFiServer wifiServer(80);
WiFiClient client;
size_t bytes;

static void InitializeWiFi(void)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    logger.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED)
    {
        logger.print('.');
        delay(1000);
    }
    
    logger.println(WiFi.localIP());
    wifiServer.begin();
}

static void PollWifiConnection(void)
{
    if (client && client.connected())
    {
        bytes = client.available();
        while (bytes-- != 0)
        {
            controller_serial.write(client.read());
        }
    }
    else
    {
        client.stop();
        logger.println("Client disconnected");
        logger.print("Waiting for client... IP address: ");
        logger.println(WiFi.localIP());
        client = wifiServer.available();
    }
}

/**
 * @brief Wait for an ackowledgement from the Teensy before beginning program
 *
 */
static void WaitForControllerConnection(void)
{
    UartConnection connection_msg, response;
    int8_t attempts = 5;

    response.id = ESP32_ID;

    while (attempts--)
    {
        if (Protobuf::UartRead(&controller_serial, &connection_msg) && (connection_msg.id == TEENSY_ID))
        {
            digitalWrite(LED_BUILTIN, HIGH);
            Protobuf::UartWrite(&controller_serial, &response);
            break;
        }

        delay(1000);
    }
}

void setup()
{
    controller_serial.begin(PROTOBUF_INTERFACE_BAUDRATE);
    logger.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    delay(1000);
    // Test connect with Teensy 4.0
    WaitForControllerConnection();
    // Initialize wifi connection
    InitializeWiFi();
    // Start OTA programming utility
    Programmer::Start(&logger);

    client = wifiServer.available();

    logger.println("Ready");
    logger.print("IP address: ");
    logger.println(WiFi.localIP());
}

void loop()
{
    PollWifiConnection();
    Programmer::Handle();
    delay(10);
}
