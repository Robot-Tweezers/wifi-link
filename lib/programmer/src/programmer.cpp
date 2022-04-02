#include <programmer.hpp>

HardwareSerial *RobotTweezers::Programmer::uart = nullptr;

void RobotTweezers::Programmer::OnStart(void)
{
    if (uart)
    {
        String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        uart->println("Start updating " + type);
    }
}

void RobotTweezers::Programmer::OnEnd(void)
{
    if (uart)
    {
        uart->println("\nEnd");
    }
}

void RobotTweezers::Programmer::OnProgress(unsigned int progress, unsigned int total)
{
    if (uart)
    {
        uart->printf("Progress: %u%%\r", (progress / (total / 100)));
    }
}

void RobotTweezers::Programmer::OnError(ota_error_t error)
{
    if (uart)
    {
        uart->printf("Error[%u]: ", error);
        switch (error)
        {
        case OTA_AUTH_ERROR:
            uart->println("Auth Failed");
            break;
        case OTA_BEGIN_ERROR:
            uart->println("Begin Failed");
            break;
        case OTA_CONNECT_ERROR:
            uart->println("Connect Failed");
            break;
        case OTA_RECEIVE_ERROR:
            uart->println("Receive Failed");
            break;
        case OTA_END_ERROR:
            uart->println("End Failed");
            break;
        }
    }
}

void RobotTweezers::Programmer::Start(HardwareSerial *uart)
{
    Programmer::uart = uart;

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
}

void RobotTweezers::Programmer::Handle(void)
{
    ArduinoOTA.handle();
}