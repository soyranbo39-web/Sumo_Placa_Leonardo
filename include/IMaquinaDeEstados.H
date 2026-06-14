
#ifndef IMaquinaDeEstados_H
#define IMaquinaDeEstados_H
#include "IMotor.H"
class IMaquinaDeEstados {
public:
    virtual void actualizarEstado() = 0;
    virtual void setMotor(IMotor* motor) = 0;
};
#endif
