const int LED = 2;

unsigned long previousMillis = 0;
const unsigned long interval = 500;

bool ledState = LOW;

void setup()
{
    pinMode(LED, OUTPUT);
}

void loop()
{
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval)
    {
        previousMillis = currentMillis;

        ledState = !ledState;

        digitalWrite(LED, ledState);
    }
}