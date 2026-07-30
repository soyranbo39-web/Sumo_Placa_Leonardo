
#ifndef ESTRATEGIAS_H
#define ESTRATEGIAS_H

#include "Estados.h"

class Motor;

class Estrategias : public Estados {
    void ejecutarEstrategia(Estados::Estado estadoActual, Motor& motor) override;
};

#endif


