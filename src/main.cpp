#include <Arduino.h>
#include "UM982Parser.h"

UM982Parser parser;

void setup()
{
    Serial.begin(115200);
    Serial3.begin(460800);
    parser.begin(Serial3);
}

void loop()
{
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
        parser.clearMessage();
    }
}