#include "../support/Arduino.h"

namespace {
unsigned long g_digitalCall = 0;
bool g_silencioDigital = false;
}

extern "C" void test_set_silencio_digital_enemigo(int activo) {
    g_silencioDigital = (activo != 0);
}

extern "C" void delay(unsigned long) {}

extern "C" int analogRead(uint8_t pin) {
    if (pin == A1 || pin == A2) {
        return 80;
    }
    return 0;
}

extern "C" int digitalRead(uint8_t) {
    if (g_silencioDigital) {
        ++g_digitalCall;
        return LOW;
    }

    // 15 lecturas digitales por ciclo de Percepcion::leer (5 sensores x 3 muestras)
    const unsigned long indice = g_digitalCall % 15u;
    const unsigned long sensor = indice / 3u;   // 0..4
    const unsigned long muestra = indice % 3u;  // 0..2
    const unsigned long ciclo = g_digitalCall / 15u;
    const unsigned long objetivo = ciclo % 5u;

    ++g_digitalCall;

    if (sensor == objetivo) {
        // Mayoría positiva para el sensor objetivo del ciclo.
        return (muestra < 2u) ? HIGH : LOW;
    }

    // Ruido bajo en sensores no objetivo.
    return (muestra == 2u && ((ciclo + sensor) % 11u == 0u)) ? HIGH : LOW;
}

extern "C" void digitalWrite(uint8_t, uint8_t) {}
extern "C" void analogWrite(uint8_t, int) {}
extern "C" void pinMode(uint8_t, uint8_t) {}
