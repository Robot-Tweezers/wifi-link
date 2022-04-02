#ifndef PROGRAMMER_HPP
#define PROGRAMMER_HPP

#include <ArduinoOTA.h>

namespace RobotTweezers
{
    class Programmer
    {
    private:
        static HardwareSerial *uart;

        static void OnStart(void);

        static void OnEnd(void);

        static void OnProgress(unsigned int progress, unsigned int total);

        static void OnError(ota_error_t error);
    
    public:
        static void Start(HardwareSerial *uart);

        static void Handle(void);
    };
}

#endif // PROGRAMMER_HPP