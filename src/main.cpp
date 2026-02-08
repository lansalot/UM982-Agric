#include <Arduino.h>
#include "UM982Parser.h"

UM982Parser parser;
elapsedMicros duration = 0;
UM982PAOGIData PAOGIData;
String PAOGISentence;
HardwareSerial &SerialGPS = Serial3;


void setup()
{
    Serial.begin(115200);
    SerialGPS.begin(460800);
    parser.begin(SerialGPS, 3); // 3 metres
}

void loop()
{
    duration = 0;
    if (parser.update())
    {
        const UM982Message &msg = parser.message();
        if (parser.decodeAgricToPAOGI(msg, PAOGIData))
        {
            // parser.formatPAOGISentence(PAOGIData, PAOGISentence);
            // Serial.print(PAOGISentence);
            // Serial.print(" Satellites: ");
            // Serial.print(PAOGIData.satellites);
            // Serial.print("  Parse duration: ");
            // Serial.print(duration);
            // Serial.println(" us");
            Serial.print(PAOGIData.altitudeMeters, 2);
            Serial.println(" m");
        }
        else
        {
            Serial.println("PAOGI decode failed");
        }
        parser.clearMessage();
    }
}