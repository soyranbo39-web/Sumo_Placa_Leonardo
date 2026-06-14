#ifndef ICONTROL_MOVIMIENTO_H
#define ICONTROL_MOVIMIENTO_H

#include "IMotor.H"
#include "DecisionMovimiento.H"

class IControlMovimiento {
public:
    virtual ~IControlMovimiento() {}
    virtual void ejecutar(const DecisionMovimiento& decision, IMotor& motor) const = 0;
};

#endif
