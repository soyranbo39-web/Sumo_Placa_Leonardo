#include <Arduino.h>
#include "Percepcion.H"
#include "Pines.H"

// -- AJUSTES DE PERSISTENCIA EXTREMA --
constexpr uint8_t RETENCION_CORTA = 4;  // Aumentado a 4. Evita que el robot deje de empujar si hay rebotes físicos.
constexpr uint8_t RETENCION_LARGA = 12; // Aumentado a 12. Persistencia letal para atrapar enemigos rápidos a los lados.

constexpr uint8_t MUESTRAS_LINEA = 2;
constexpr uint8_t MAYORIA_LINEA = 1;
constexpr int MARGEN_BORDE = 18;

// Usamos 4 bits por sensor (permite guardar hasta 15 ciclos)
constexpr uint8_t RETENCION_BITS = 4;
constexpr uint32_t RETENCION_MASK = (1u << RETENCION_BITS) - 1u;

constexpr uint8_t RETENCION_LAT_IZQ = 0;
constexpr uint8_t RETENCION_C45_IZQ = 1;
constexpr uint8_t RETENCION_FRONTAL = 2;
constexpr uint8_t RETENCION_C45_DER = 3;
constexpr uint8_t RETENCION_LAT_DER = 4;


bool Percepcion::detectarLineaSeguro(uint8_t pin) {
    uint8_t activas = 0;
    for (uint8_t muestra = 0; muestra < MUESTRAS_LINEA; ++muestra) {
        activas += (analogRead(pin) <= (BLANCO + MARGEN_BORDE)) ? 1 : 0;
        if (activas >= MAYORIA_LINEA) {
            return true;
        }

        const uint8_t restantes = static_cast<uint8_t>((MUESTRAS_LINEA - 1u) - muestra);
        if (static_cast<uint8_t>(activas + restantes) < MAYORIA_LINEA) {
            return false;
        }
    }
    return activas >= MAYORIA_LINEA;
}

bool Percepcion::lecturaDigitalMayoritaria(int pin) {
    uint8_t activos = 0;
    for (uint8_t muestra = 0; muestra < MUESTRAS_LINEA; ++muestra) {
        activos += (digitalRead(pin) == HIGH) ? 1 : 0;
        if (activos >= MAYORIA_LINEA) {
            return true;
        }

        const uint8_t restantes = static_cast<uint8_t>((MUESTRAS_LINEA - 1u) - muestra);
        if (static_cast<uint8_t>(activos + restantes) < MAYORIA_LINEA) {
            return false;
        }
    }
    return activos >= MAYORIA_LINEA;
}

uint8_t Percepcion::obtenerRetencion(uint8_t indice) const {
    const uint8_t desplazamiento = indice * RETENCION_BITS;
    return (retenciones >> desplazamiento) & RETENCION_MASK;
}

void Percepcion::establecerRetencion(uint8_t indice, uint8_t valor) const {
    const uint8_t desplazamiento = indice * RETENCION_BITS;
    const uint32_t mascara = static_cast<uint32_t>(RETENCION_MASK) << desplazamiento;
    retenciones = (retenciones & ~mascara) | ((static_cast<uint32_t>(valor) & RETENCION_MASK) << desplazamiento);
}

bool Percepcion::aplicarRetencion(uint8_t indice, bool detectado) const {
    if (detectado) {
        uint8_t ciclos = (indice == RETENCION_LAT_IZQ || indice == RETENCION_LAT_DER) ? 
                         RETENCION_LARGA : RETENCION_CORTA;
                         
        establecerRetencion(indice, ciclos);
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
    lecturas.lineaIzq = detectarLineaSeguro(S_PISO_IZQ);
    lecturas.lineaDer = detectarLineaSeguro(S_PISO_DER);

    lecturas.latIzq = aplicarRetencion(RETENCION_LAT_IZQ, lecturaDigitalMayoritaria(S_LAT_IZQ));
    lecturas.c45Izq = aplicarRetencion(RETENCION_C45_IZQ, lecturaDigitalMayoritaria(S_FRONT_IZQ));
    lecturas.frontal = aplicarRetencion(RETENCION_FRONTAL, lecturaDigitalMayoritaria(S_FRONT_CEN));
    lecturas.c45Der = aplicarRetencion(RETENCION_C45_DER, lecturaDigitalMayoritaria(S_FRONT_DER));
    lecturas.latDer = aplicarRetencion(RETENCION_LAT_DER, lecturaDigitalMayoritaria(S_LAT_DER));
    return lecturas;
}