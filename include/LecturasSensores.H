#ifndef LECTURAS_SENSORES_H
#define LECTURAS_SENSORES_H

#include <stdint.h>

struct LecturasSensores {
    uint8_t lineaIzq : 1;
    uint8_t lineaDer : 1;
    uint8_t latIzq : 1;
    uint8_t c45Izq : 1;
    uint8_t frontal : 1;
    uint8_t c45Der : 1;
    uint8_t latDer : 1;
};

#endif
