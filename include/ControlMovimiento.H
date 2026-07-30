#ifndef CONTROL_MOVIMIENTO_H
#define CONTROL_MOVIMIENTO_H

#include "IControlMovimiento.H"
#include "UtilMatematica.H"

struct ReguladorPID {
    int16_t integral = 0;
    int8_t errorAnterior = 0;

    void reiniciar() {
        integral = 0;
        errorAnterior = 0;
    }
};

class ControlMovimiento: public IControlMovimiento {
public:
    void ejecutar(const DecisionMovimiento& decision, IMotor& motor) const override;

private:
    static int16_t calcularPID(ReguladorPID& regulador, int8_t error);
    void ejecutarBusqueda(IMotor& motor) const;
    void actualizarBusqueda(int8_t error) const;
    int8_t obtenerSesgoBusqueda() const;

    mutable ReguladorPID regulador;
    mutable uint8_t estadoBusqueda = 0;
    mutable TipoAccion ultimaAccion = TipoAccion::Busqueda;
};

#endif
