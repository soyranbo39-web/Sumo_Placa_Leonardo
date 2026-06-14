#include <Arduino.h>
#include "Estados.H"

Estado::Estado(IPercepcion& percepcionRef, IEstrategiaCombate& estrategiaRef, IControlMovimiento& controlRef)
    : motor(nullptr),
            percepcion(percepcionRef), estrategia(estrategiaRef), controlMovimiento(controlRef) {}

void Estado::setMotor(IMotor* nuevoMotor) {
    motor = nuevoMotor;
}

void Estado::actualizarEstado() {
    if (motor == nullptr) {
        return;
    }

    LecturasSensores lecturas = percepcion.leer();
    DecisionMovimiento decision = estrategia.decidir(lecturas);
    controlMovimiento.ejecutar(decision, *motor);
}
