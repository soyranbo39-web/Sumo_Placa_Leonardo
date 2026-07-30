#include <Arduino.h>
#include "ControlMovimiento.H"
#include "Pines.H"

namespace {
constexpr unsigned long RETROCESO_MS = 300;
constexpr unsigned long GIRO_EVASION_MS = 360;
constexpr int16_t PID_ESCALA = 16;
constexpr int16_t PID_KP = 42;
constexpr int16_t PID_KI = 3;
constexpr int16_t PID_KD = 18;
constexpr int16_t INTEGRAL_LIMITE = 400;
constexpr int16_t CORRECCION_LIMITE = 120;
constexpr int16_t BUSQUEDA_BASE = 64;
constexpr int8_t BUSQUEDA_CORRECCIONES[] = {-20, -14, -8, 0, 0, 8, 14, 20};
constexpr uint8_t BUSQUEDA_FASES = 8;
constexpr uint8_t BUSQUEDA_FASE_MASK = BUSQUEDA_FASES - 1u;
constexpr uint8_t BUSQUEDA_FASE_MASCARA = 0x07;
constexpr uint8_t BUSQUEDA_SESGO_MASCARA = 0x80;

void ejecutarEvasion(IMotor& motor, int giroIzq, int giroDer) {
    motor.mover(-VelocidadRetroceso, -VelocidadRetroceso);
    delay(RETROCESO_MS);
    motor.mover(giroIzq, giroDer);
    delay(GIRO_EVASION_MS);
}

void moverSuave(IMotor& motor, int16_t baseIzq, int16_t baseDer, int16_t correccion) {
    const int16_t velocidadIzq = limitar<int16_t>(static_cast<int16_t>(baseIzq - correccion), -VelocidadMaxima, VelocidadMaxima);
    const int16_t velocidadDer = limitar<int16_t>(static_cast<int16_t>(baseDer + correccion), -VelocidadMaxima, VelocidadMaxima);
    motor.mover(static_cast<int>(velocidadIzq), static_cast<int>(velocidadDer));
}
}

int16_t ControlMovimiento::calcularPID(ReguladorPID& regulador, int8_t error) {
    regulador.integral = limitar<int16_t>(
        static_cast<int16_t>(regulador.integral + error),
        static_cast<int16_t>(-INTEGRAL_LIMITE),
        INTEGRAL_LIMITE
    );

    const int16_t derivada = static_cast<int16_t>(error - regulador.errorAnterior);
    regulador.errorAnterior = error;

    const int16_t salida = static_cast<int16_t>(
        (PID_KP * error) + (PID_KI * regulador.integral) + (PID_KD * derivada)
    );

    return limitar<int16_t>(static_cast<int16_t>(salida / PID_ESCALA), -CORRECCION_LIMITE, CORRECCION_LIMITE);
}

int8_t ControlMovimiento::obtenerSesgoBusqueda() const {
    return (estadoBusqueda & BUSQUEDA_SESGO_MASCARA) ? -1 : 1;
}

void ControlMovimiento::actualizarBusqueda(int8_t error) const {
    if (error < 0) {
        estadoBusqueda &= static_cast<uint8_t>(~BUSQUEDA_SESGO_MASCARA);
    } else if (error > 0) {
        estadoBusqueda |= BUSQUEDA_SESGO_MASCARA;
    }
}

void ControlMovimiento::ejecutarBusqueda(IMotor& motor) const {
    const uint8_t fase = estadoBusqueda & BUSQUEDA_FASE_MASCARA;
    const int16_t correccion = static_cast<int16_t>(BUSQUEDA_CORRECCIONES[fase] * obtenerSesgoBusqueda());
    moverSuave(motor, BUSQUEDA_BASE, BUSQUEDA_BASE, correccion);
    estadoBusqueda = static_cast<uint8_t>((estadoBusqueda & BUSQUEDA_SESGO_MASCARA) | ((fase + 1u) & BUSQUEDA_FASE_MASK));
}

void ControlMovimiento::ejecutar(const DecisionMovimiento& decision, IMotor& motor) const {
    if (decision.error != 0) {
        actualizarBusqueda(decision.error);
    }

    switch (decision.tipo) {
    case TipoAccion::EvadirBordeIzq:
        regulador.reiniciar();
        motor.mover(-VelocidadRetroceso, -VelocidadRetroceso);
        delay(RETROCESO_MS);
        motor.mover(0, VelocidadMaxima);
        delay(GIRO_EVASION_MS);
        break;
    case TipoAccion::EvadirBordeDer:
        regulador.reiniciar();
        motor.mover(-VelocidadRetroceso, -VelocidadRetroceso);
        delay(RETROCESO_MS);
        motor.mover(VelocidadMaxima, 0);
        delay(GIRO_EVASION_MS);
        break;
    case TipoAccion::EvadirBordeAmbos:
        regulador.reiniciar();
        motor.mover(-VelocidadRetroceso, -VelocidadRetroceso);
        delay(RETROCESO_MS);
        if (obtenerSesgoBusqueda() > 0) {
            motor.mover(0, VelocidadMaxima);
        } else {
            motor.mover(VelocidadMaxima, 0);
        }
        delay(GIRO_EVASION_MS);
        break;
    case TipoAccion::AtaqueFrontal:
        moverSuave(motor, VelocidadAtaqueFrontal, VelocidadAtaqueFrontal, calcularPID(regulador, decision.error));
        break;
    case TipoAccion::CorregirIzq:
        moverSuave(motor, VelocidadCurva, VelocidadMaxima, calcularPID(regulador, decision.error));
        break;
    case TipoAccion::CorregirDer:
        moverSuave(motor, VelocidadMaxima, VelocidadCurva, calcularPID(regulador, decision.error));
        break;
    case TipoAccion::DefensaIzq:
        regulador.reiniciar();
        motor.mover(VelocidadMinima, VelocidadPivoteLateral);
        break;
    case TipoAccion::DefensaDer:
        regulador.reiniciar();
        motor.mover(VelocidadPivoteLateral, VelocidadMinima);
        break;
    case TipoAccion::AtaqueLateralIzq:
        regulador.reiniciar();
        motor.mover(VelocidadCurvaAtaqueLateral, VelocidadMaxima);
        break;
    case TipoAccion::AtaqueLateralDer:
        regulador.reiniciar();
        motor.mover(VelocidadMaxima, VelocidadCurvaAtaqueLateral);
        break;
    case TipoAccion::Busqueda:
        if (ultimaAccion == TipoAccion::AtaqueFrontal ||
            ultimaAccion == TipoAccion::CorregirIzq ||
            ultimaAccion == TipoAccion::CorregirDer ||
            ultimaAccion == TipoAccion::DefensaIzq ||
            ultimaAccion == TipoAccion::DefensaDer ||
            ultimaAccion == TipoAccion::AtaqueLateralIzq ||
            ultimaAccion == TipoAccion::AtaqueLateralDer) {
            motor.mover(-VelocidadMaxima, -VelocidadMaxima);
            break;
        }
        ejecutarBusqueda(motor);
        break;
    default:
        ejecutarBusqueda(motor);
        break;
    }

    ultimaAccion = decision.tipo;
}
