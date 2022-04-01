#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <config.hpp>
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

static void OnStart(void)
{
    //String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    //Serial.println("Start updating " + type);
}

static void OnEnd(void)
{
    //Serial.println("\nEnd");
}

static void OnProgress(unsigned int progress, unsigned int total)
{
    //Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
}

static void OnError(ota_error_t error)
{
    /*
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
        Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
        Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
        Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
        Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
        Serial.println("End Failed");
    */
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

    // Port defaults to 3232
    // ArduinoOTA.setPort(3232);

    // Hostname defaults to esp3232-[MAC]
    // ArduinoOTA.setHostname("myesp32");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA
        .onStart(OnStart)
        .onEnd(OnEnd)
        .onProgress(OnProgress)
        .onError(OnError);

    ArduinoOTA.begin();

    //Serial.println("Ready");
    //Serial.print("IP address: ");
    //Serial.println(WiFi.localIP());
}

void loop()
{
    ArduinoOTA.handle();
    delay(100);
}
