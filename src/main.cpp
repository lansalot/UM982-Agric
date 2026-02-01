#include <Arduino.h>
#include "UM982Parser.h"

UM982Parser parser;
elapsedMicros duration = 0;
UM982PandaData pandaData;
String pandaSentence;

void setup()
{
    Serial.begin(115200);
    Serial3.begin(460800);
    parser.begin(Serial3);
}

void loop()
{
    duration = 0;
    if (parser.update())
    {
        const UM982Message &msg = parser.message();
        Serial.print(millis());
        Serial.print(" UM982 message ID: ");
        Serial.print(msg.header.messageId);
        Serial.print("  Payload length: ");
        Serial.print(msg.payloadLength);
        Serial.print(" ");
        Serial.println(msg.validCrc ? "  CRC valid" : "  CRC invalid");
        if (parser.decodeAgricToPanda(msg, pandaData))
        {
            parser.formatPandaSentence(pandaData, pandaSentence);
            Serial.print(pandaSentence);
            Serial.print(" Satellites: ");
            Serial.print(pandaData.satellites);
            Serial.print("  Parse duration: ");
            Serial.print(duration);
            Serial.println(" us");
            duration = 0;
        }
        else
        {
            Serial.println("PANDA decode failed");
        }
        parser.clearMessage();
    }
}