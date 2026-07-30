#include "../support/Arduino.h"

namespace {
unsigned long g_tickAnalogIzq = 0;
unsigned long g_tickAnalogDer = 0;
unsigned long g_tickDigital = 0;
bool g_silencioDigital = false;
}

extern "C" void test_set_silencio_digital(int activo) {
    g_silencioDigital = (activo != 0);
}

extern "C" void delay(unsigned long) {}

extern "C" int analogRead(uint8_t pin) {
    if (pin == A1) {
        // Izquierda: mayormente detecta borde, con ruido intermitente.
        const int valor = ((g_tickAnalogIzq % 5u) == 0u) ? 61 : 56;
        ++g_tickAnalogIzq;
        return valor;
    }
    if (pin == A2) {
        // Derecha: mayormente fuera de borde, con picos de ruido.
        const int valor = ((g_tickAnalogDer % 4u) == 0u) ? 57 : 63;
        ++g_tickAnalogDer;
        return valor;
    }
    return 0;
}

extern "C" int digitalRead(uint8_t) {
    if (g_silencioDigital) {
        return LOW;
    }

    // Pulso corto periódico: permite mayorías ocasionales sin saturar en HIGH.
    const unsigned long fase = g_tickDigital % 9u;
    const int valor = (fase == 0u || fase == 1u) ? HIGH : LOW;
    ++g_tickDigital;
    return valor;
}

extern "C" void digitalWrite(uint8_t, uint8_t) {}
extern "C" void analogWrite(uint8_t, int) {}
extern "C" void pinMode(uint8_t, uint8_t) {}
