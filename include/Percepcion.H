#ifndef PERCEPCION_H
#define PERCEPCION_H

#include "LecturasSensores.H"
#include "IPercepcion.H"

class Percepcion: public IPercepcion {
public:
    LecturasSensores leer() const override;

private:
    static bool lecturaDigitalMayoritaria(int pin);
    static bool aplicarRetencion(bool detectado, uint8_t& ciclosRetencion);

    mutable uint8_t retLatIzq = 0;
    mutable uint8_t retC45Izq = 0;
    mutable uint8_t retFrontal = 0;
    mutable uint8_t retC45Der = 0;
    mutable uint8_t retLatDer = 0;
};

#endif
