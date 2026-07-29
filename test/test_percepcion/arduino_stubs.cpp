#include "../support/Arduino.h"

namespace {
int g_digitalSequence[32] = {0};
int g_digitalSequenceCount = 0;
int g_digitalSequenceIndex = 0;
int g_analogA1 = 0;
int g_analogA2 = 0;
}

extern "C" void test_reset_percepcion_stubs(void) {
    g_digitalSequenceCount = 0;
    g_digitalSequenceIndex = 0;
    g_analogA1 = 0;
    g_analogA2 = 0;
    for (int i = 0; i < 32; ++i) {
        g_digitalSequence[i] = 0;
    }
}

extern "C" void test_set_digital_sequence(const int* values, int count) {
    g_digitalSequenceCount = (count > 32) ? 32 : count;
    g_digitalSequenceIndex = 0;
    for (int i = 0; i < g_digitalSequenceCount; ++i) {
        g_digitalSequence[i] = values[i];
    }
}

extern "C" void test_set_analog_value(uint8_t pin, int value) {
    if (pin == A1) {
        g_analogA1 = value;
    } else if (pin == A2) {
        g_analogA2 = value;
    }
}

extern "C" void delay(unsigned long) {}

extern "C" int analogRead(uint8_t pin) {
    if (pin == A1) {
        return g_analogA1;
    }
    if (pin == A2) {
        return g_analogA2;
    }
    return 0;
}

extern "C" int digitalRead(uint8_t) {
    if (g_digitalSequenceIndex < g_digitalSequenceCount) {
        return g_digitalSequence[g_digitalSequenceIndex++];
    }
    return 0;
}

extern "C" void digitalWrite(uint8_t, uint8_t) {}
extern "C" void analogWrite(uint8_t, int) {}
extern "C" void pinMode(uint8_t, uint8_t) {}
