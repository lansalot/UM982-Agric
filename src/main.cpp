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
    parser.begin(SerialGPS, 1.5); // 1.5m height
}

void loop()
{
    duration = 0;
    if (parser.update())
    {
        const UM982Message &msg = parser.message();
        // Serial.print(millis());
        // Serial.print(" UM982 message ID: ");
        // Serial.print(msg.header.messageId);
        // Serial.print("  Payload length: ");
        // Serial.print(msg.payloadLength);
        // Serial.print(" ");
        // Serial.println(msg.validCrc ? "  CRC valid" : "  CRC invalid");
        if (parser.decodeAgricToPAOGI(msg, PAOGIData))
        {
            parser.formatPAOGISentence(PAOGIData, PAOGISentence);
            Serial.print(PAOGISentence);
            Serial.print(" Satellites: ");
            Serial.print(PAOGIData.satellites);
            Serial.print("  Parse duration: ");
            Serial.print(duration);
            Serial.println(" us");
            duration = 0;
        }
        else
        {
            Serial.println("PAOGI decode failed");
        }
        parser.clearMessage();
    }
}