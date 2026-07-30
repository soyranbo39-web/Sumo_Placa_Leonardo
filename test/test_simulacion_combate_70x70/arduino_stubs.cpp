#include "../support/Arduino.h"

class SimuladorCombate;

namespace {
SimuladorCombate* g_simulador = nullptr;
}

extern "C" void test_set_simulador(void* ptr) {
    g_simulador = static_cast<SimuladorCombate*>(ptr);
}

extern "C" void delay(unsigned long ms);

extern "C" int analogRead(uint8_t) { return 0; }
extern "C" int digitalRead(uint8_t) { return 0; }
extern "C" void digitalWrite(uint8_t, uint8_t) {}
extern "C" void analogWrite(uint8_t, int) {}
extern "C" void pinMode(uint8_t, uint8_t) {}
