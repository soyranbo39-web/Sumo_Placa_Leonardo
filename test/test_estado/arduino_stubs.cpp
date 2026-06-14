#include "../support/Arduino.h"

int g_delayCallCount = 0;
unsigned long g_delayValues[16] = {0};

extern "C" void delay(unsigned long ms) {
    if (g_delayCallCount < 16) {
        g_delayValues[g_delayCallCount] = ms;
    }
    g_delayCallCount++;
}

extern "C" int analogRead(uint8_t) { return 0; }
extern "C" int digitalRead(uint8_t) { return 0; }
extern "C" void digitalWrite(uint8_t, uint8_t) {}
extern "C" void analogWrite(uint8_t, int) {}
extern "C" void pinMode(uint8_t, uint8_t) {}
