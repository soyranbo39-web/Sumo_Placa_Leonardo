
#ifndef Sensores_de_piso_H
#define Sensores_de_piso_H

#include <stdint.h>
class ISensores_de_piso {
public:
    virtual void inicializarSensoresDePiso() = 0;
    virtual void Lecturas_de_piso_con_filtro() = 0;
    virtual void obtenerLecturasSensoresDePiso(uint8_t& lecturas) = 0;
    virtual void FiltrosDeKlaman() = 0;
};


#endif