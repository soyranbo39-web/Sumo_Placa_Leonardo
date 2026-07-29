#include <Arduino.h>
#include "Percepcion.H"
#include "Pines.H"

namespace {
constexpr uint8_t CICLOS_RETENCION_ENEMIGO = 2;
}

bool Percepcion::lecturaDigitalMayoritaria(int pin) {
    uint8_t activos = 0;
    activos += (digitalRead(pin) == HIGH) ? 1 : 0;
    activos += (digitalRead(pin) == HIGH) ? 1 : 0;
    activos += (digitalRead(pin) == HIGH) ? 1 : 0;
    return activos >= 2;
}

bool Percepcion::aplicarRetencion(bool detectado, uint8_t& ciclosRetencion) {
    if (detectado) {
        ciclosRetencion = CICLOS_RETENCION_ENEMIGO;
        return true;
    }

    if (ciclosRetencion > 0) {
        --ciclosRetencion;
        return true;
    }

    return false;
}

LecturasSensores Percepcion::leer() const {
    LecturasSensores lecturas;
    lecturas.lineaIzq = (analogRead(S_PISO_IZQ) < BLANCO);
    lecturas.lineaDer = (analogRead(S_PISO_DER) < BLANCO);
    lecturas.latIzq = aplicarRetencion(lecturaDigitalMayoritaria(S_LAT_IZQ), retLatIzq);
    lecturas.c45Izq = aplicarRetencion(lecturaDigitalMayoritaria(S_FRONT_IZQ), retC45Izq);
    lecturas.frontal = aplicarRetencion(lecturaDigitalMayoritaria(S_FRONT_CEN), retFrontal);
    lecturas.c45Der = aplicarRetencion(lecturaDigitalMayoritaria(S_FRONT_DER), retC45Der);
    lecturas.latDer = aplicarRetencion(lecturaDigitalMayoritaria(S_LAT_DER), retLatDer);
    return lecturas;
}
