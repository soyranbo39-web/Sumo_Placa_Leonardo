#ifndef IESTRATEGIA_COMBATE_H
#define IESTRATEGIA_COMBATE_H

#include "LecturasSensores.H"
#include "DecisionMovimiento.H"

class IEstrategiaCombate {
public:
    virtual ~IEstrategiaCombate() {}
    virtual DecisionMovimiento decidir(const LecturasSensores& lecturas) const = 0;
};

#endif
