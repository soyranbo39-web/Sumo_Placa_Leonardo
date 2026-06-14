#ifndef ESTRATEGIA_COMBATE_H
#define ESTRATEGIA_COMBATE_H

#include "LecturasSensores.H"
#include "DecisionMovimiento.H"
#include "IEstrategiaCombate.H"

class EstrategiaCombate: public IEstrategiaCombate {
public:
    DecisionMovimiento decidir(const LecturasSensores& lecturas) const override;
};

#endif
