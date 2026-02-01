#include <Arduino.h>

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    Serial.print(millis());
    Serial.print(" ms elapsed\n");
    delay(1000);
}