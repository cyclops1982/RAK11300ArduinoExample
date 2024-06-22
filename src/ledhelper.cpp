#include "ledhelper.h"

bool LedHelper::isInitialized = false;

void LedHelper::init()
{
    if (!isInitialized)
    {
        pinMode(LED_BLUE, OUTPUT);
        pinMode(LED_GREEN, OUTPUT);
        delay(100);
        digitalWrite(LED_BLUE, LOW);
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_BLUE, HIGH);
        digitalWrite(LED_GREEN, HIGH);
        delay(500);
        digitalWrite(LED_BLUE, LOW);
        digitalWrite(LED_GREEN, LOW);

        isInitialized = true;
    }
}

void LedHelper::BlinkDelay(int ledpin, int waittime)
{
    init();
    digitalWrite(ledpin, HIGH);
    delay(waittime);
    digitalWrite(ledpin, LOW);
    delay(waittime);
}

void LedHelper::BlinkHalt(uint blickcount)
{
    init();
    while (1)
    {
        for (short i = 0; i < blickcount; i++)
        {
            digitalWrite(LED_BLUE, HIGH);
            delay(200);
            digitalWrite(LED_BLUE, LOW);
            delay(200);
        }
        delay(1000);
    }
}
