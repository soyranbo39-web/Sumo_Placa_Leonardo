
#ifndef EsTADOS_H
#define EsTADOS_H
#include "IMaquinaDeEstados.H"
#include "IMotor.H"
#include "IPercepcion.H"
#include "IEstrategiaCombate.H"
#include "IControlMovimiento.H"

class Estado: public IMaquinaDeEstados {
    IMotor* motor;
    IPercepcion& percepcion;
    IEstrategiaCombate& estrategia;
    IControlMovimiento& controlMovimiento;
public:
    Estado(IPercepcion& percepcion, IEstrategiaCombate& estrategia, IControlMovimiento& controlMovimiento);
    void actualizarEstado() override;
    void setMotor(IMotor* motor) override;
};

#endif