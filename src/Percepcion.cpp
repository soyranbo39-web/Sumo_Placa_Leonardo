#include <Arduino.h>
#include "Percepcion.H"
#include "Pines.H"

namespace {
constexpr uint8_t CICLOS_RETENCION_ENEMIGO = 2;
constexpr uint8_t RETENCION_BITS = 2;
constexpr uint16_t RETENCION_MASK = (1u << RETENCION_BITS) - 1u;
constexpr uint8_t RETENCION_LAT_IZQ = 0;
constexpr uint8_t RETENCION_C45_IZQ = 1;
constexpr uint8_t RETENCION_FRONTAL = 2;
constexpr uint8_t RETENCION_C45_DER = 3;
constexpr uint8_t RETENCION_LAT_DER = 4;
}

bool Percepcion::lecturaDigitalMayoritaria(int pin) {
    uint8_t activos = 0;
    for (uint8_t muestra = 0; muestra < 3; ++muestra) {
        activos += (digitalRead(pin) == HIGH) ? 1 : 0;
    }
    return activos >= 2;
}

uint8_t Percepcion::obtenerRetencion(uint8_t indice) const {
    const uint8_t desplazamiento = indice * RETENCION_BITS;
    return (retenciones >> desplazamiento) & RETENCION_MASK;
}

void Percepcion::establecerRetencion(uint8_t indice, uint8_t valor) const {
    const uint8_t desplazamiento = indice * RETENCION_BITS;
    const uint16_t mascara = static_cast<uint16_t>(RETENCION_MASK << desplazamiento);
    retenciones = static_cast<uint16_t>((retenciones & ~mascara) | ((valor & RETENCION_MASK) << desplazamiento));
}

bool Percepcion::aplicarRetencion(uint8_t indice, bool detectado) const {
    if (detectado) {
        establecerRetencion(indice, CICLOS_RETENCION_ENEMIGO);
        return true;
    }

    const uint8_t ciclosRetencion = obtenerRetencion(indice);
    if (ciclosRetencion > 0) {
        establecerRetencion(indice, ciclosRetencion - 1);
        return true;
    }

    return false;
}

LecturasSensores Percepcion::leer() const {
    LecturasSensores lecturas;
    lecturas.lineaIzq = (analogRead(S_PISO_IZQ) < BLANCO);
    lecturas.lineaDer = (analogRead(S_PISO_DER) < BLANCO);
    lecturas.latIzq = aplicarRetencion(RETENCION_LAT_IZQ, lecturaDigitalMayoritaria(S_LAT_IZQ));
    lecturas.c45Izq = aplicarRetencion(RETENCION_C45_IZQ, lecturaDigitalMayoritaria(S_FRONT_IZQ));
    lecturas.frontal = aplicarRetencion(RETENCION_FRONTAL, lecturaDigitalMayoritaria(S_FRONT_CEN));
    lecturas.c45Der = aplicarRetencion(RETENCION_C45_DER, lecturaDigitalMayoritaria(S_FRONT_DER));
    lecturas.latDer = aplicarRetencion(RETENCION_LAT_DER, lecturaDigitalMayoritaria(S_LAT_DER));
    return lecturas;
}
