#ifndef CONTROL_MOVIMIENTO_H
#define CONTROL_MOVIMIENTO_H

#include "IControlMovimiento.H"

class ControlMovimiento: public IControlMovimiento {
public:
    void ejecutar(const DecisionMovimiento& decision, IMotor& motor) const override;
};

#endif
