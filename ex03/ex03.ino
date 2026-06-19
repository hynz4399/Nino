const int LED = 2;   // ESP32板载LED一般为GPIO2

struct FlashStep
{
    bool state;
    unsigned long duration;
};

FlashStep sosPattern[] =
{
    // S: ...
    {HIGH, 200}, {LOW, 200},
    {HIGH, 200}, {LOW, 200},
    {HIGH, 200}, {LOW, 200},

    // O: ---
    {HIGH, 600}, {LOW, 200},
    {HIGH, 600}, {LOW, 200},
    {HIGH, 600}, {LOW, 200},

    // S: ...
    {HIGH, 200}, {LOW, 200},
    {HIGH, 200}, {LOW, 200},
    {HIGH, 200}, {LOW, 1500}   // SOS结束后停顿1.5秒
};

const int totalSteps = sizeof(sosPattern) / sizeof(sosPattern[0]);

int currentStep = 0;
unsigned long previousMillis = 0;

void setup()
{
    pinMode(LED, OUTPUT);
    digitalWrite(LED, sosPattern[0].state);
}

void loop()
{
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= sosPattern[currentStep].duration)
    {
        previousMillis = currentMillis;

        currentStep++;

        if (currentStep >= totalSteps)
        {
            currentStep = 0;
        }

        digitalWrite(LED, sosPattern[currentStep].state);
    }
}