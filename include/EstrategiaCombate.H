#ifndef ESTRATEGIA_COMBATE_H
#define ESTRATEGIA_COMBATE_H

#include "LecturasSensores.H"
#include "DecisionMovimiento.H"
#include "IEstrategiaCombate.H"

class EstrategiaCombate: public IEstrategiaCombate {
public:
    DecisionMovimiento decidir(const LecturasSensores& lecturas) const override;

private:
    static int8_t calcularErrorDireccion(const LecturasSensores& lecturas);
    mutable int8_t ultimoErrorObjetivo = 0;
    mutable uint8_t retencionObjetivo = 0;
};

#endif
