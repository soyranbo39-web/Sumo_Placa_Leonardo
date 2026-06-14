#include <Arduino.h>
#include "ControlMovimiento.H"
#include "Pines.H"

void ControlMovimiento::ejecutar(const DecisionMovimiento& decision, IMotor& motor) const {
    switch (decision.tipo) {
    case TipoAccion::EvadirBordeIzq:
        motor.mover(-VelocidadRetroceso, -VelocidadRetroceso);
        delay(150);
        motor.mover(VelocidadMaxima, -VelocidadMaxima);
        delay(350);
        break;
    case TipoAccion::EvadirBordeDer:
        motor.mover(-VelocidadRetroceso, -VelocidadRetroceso);
        delay(150);
        motor.mover(-VelocidadMaxima, VelocidadMaxima);
        delay(350);
        break;
    case TipoAccion::EvadirBordeAmbos:
        motor.mover(-VelocidadRetroceso, -VelocidadRetroceso);
        delay(150);
        motor.mover(VelocidadMaxima, -VelocidadMaxima);
        delay(350);
        break;
    case TipoAccion::AtaqueFrontal:
        motor.mover(VelocidadMaxima, VelocidadMaxima);
        break;
    case TipoAccion::CorregirIzq:
        motor.mover(VelocidadMinima, VelocidadMaxima);
        break;
    case TipoAccion::CorregirDer:
        motor.mover(VelocidadMaxima, VelocidadMinima);
        break;
    case TipoAccion::DefensaIzq:
        motor.mover(-VelocidadGiro, VelocidadGiro);
        break;
    case TipoAccion::DefensaDer:
        motor.mover(VelocidadGiro, -VelocidadGiro);
        break;
    case TipoAccion::Busqueda:
    default:
        motor.mover(VelocidadAvance, VelocidadBusquedaDer);
        break;
    }
}
