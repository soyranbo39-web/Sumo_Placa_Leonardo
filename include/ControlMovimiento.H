#ifndef CONTROL_MOVIMIENTO_H
#define CONTROL_MOVIMIENTO_H

#include "IControlMovimiento.H"
#include "IMotor.H"

class ControlMovimiento : public IControlMovimiento {
public:
    void ejecutar(const DecisionMovimiento& decision, IMotor& motor) const override;

private:
    struct ReguladorPID {
        int16_t integral = 0;
        int8_t errorAnterior = 0;
    };

    static int16_t calcularPID(ReguladorPID& regulador, int8_t error);
    void ejecutarBusqueda(IMotor& motor) const;
    void actualizarBusqueda(int8_t error) const;
    int8_t obtenerSesgoBusqueda() const;

    mutable TipoAccion ultimaAccion = TipoAccion::Busqueda;
    mutable ReguladorPID regulador;
    mutable uint8_t estadoBusqueda = 0;
    
    // Variables de la Máquina de Estados Asíncrona
    mutable unsigned long tiempoInicioEvasion = 0;
    mutable uint8_t faseEvasion = 0;
    mutable TipoAccion tipoEvasionActual = TipoAccion::Busqueda;
};

#endif