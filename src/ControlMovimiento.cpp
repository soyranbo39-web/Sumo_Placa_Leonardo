#include <Arduino.h>
#include "ControlMovimiento.H"
#include "Pines.H"
#include "UtilMatematica.H" // <-- ¡Aquí está la corrección!

namespace {
// -- TIEMPOS DE EVASIÓN ASÍNCRONA --
constexpr unsigned long RETROCESO_MS = 300;     
constexpr unsigned long GIRO_EVASION_MS = 250;  

// -- PID COMPETITIVO --
constexpr int16_t PID_ESCALA = 16;
constexpr int16_t PID_KP = 85;   
constexpr int16_t PID_KI = 0;    
constexpr int16_t PID_KD = 250;  
constexpr int16_t INTEGRAL_LIMITE = 400;
constexpr int16_t CORRECCION_LIMITE = 120;
constexpr uint8_t BUSQUEDA_SESGO_MASCARA = 0x80;

void moverSuave(IMotor& motor, int16_t baseIzq, int16_t baseDer, int16_t correccion) {
    const int16_t velocidadIzq = limitar<int16_t>(static_cast<int16_t>(baseIzq - correccion), -VelocidadMaxima, VelocidadMaxima);
    const int16_t velocidadDer = limitar<int16_t>(static_cast<int16_t>(baseDer + correccion), -VelocidadMaxima, VelocidadMaxima);
    motor.mover(static_cast<int>(velocidadIzq), static_cast<int>(velocidadDer));
}
}

int16_t ControlMovimiento::calcularPID(ReguladorPID& regulador, int8_t error) {
    regulador.integral = limitar<int16_t>(static_cast<int16_t>(regulador.integral + error), static_cast<int16_t>(-INTEGRAL_LIMITE), INTEGRAL_LIMITE);
    const int16_t derivada = static_cast<int16_t>(error - regulador.errorAnterior);
    regulador.errorAnterior = error;
    const int16_t salida = static_cast<int16_t>((PID_KP * error) + (PID_KI * regulador.integral) + (PID_KD * derivada));
    return limitar<int16_t>(static_cast<int16_t>(salida / PID_ESCALA), -CORRECCION_LIMITE, CORRECCION_LIMITE);
}

int8_t ControlMovimiento::obtenerSesgoBusqueda() const {
    return (estadoBusqueda & BUSQUEDA_SESGO_MASCARA) ? -1 : 1;
}

void ControlMovimiento::actualizarBusqueda(int8_t error) const {
    if (error < 0) estadoBusqueda &= static_cast<uint8_t>(~BUSQUEDA_SESGO_MASCARA);
    else if (error > 0) estadoBusqueda |= BUSQUEDA_SESGO_MASCARA;
}

void ControlMovimiento::ejecutarBusqueda(IMotor& motor) const {
    // -- NUEVA ESTRATEGIA: BÚSQUEDA EN ARCO CURVO --
    // El robot no se queda en su lugar. Avanza dibujando un arco amplio 
    // hacia la última dirección donde registró al enemigo.
    constexpr int16_t VelAvanceBase = 90;  // Velocidad de la rueda interior (avanza)
    constexpr int16_t VelGiroExterior = 180; // Velocidad de la rueda exterior (gira y empuja)

    if (obtenerSesgoBusqueda() > 0) {
        // Perdió al enemigo por la derecha: Arco agresivo hacia la derecha
        moverSuave(motor, VelGiroExterior, VelAvanceBase, 0);
    } else {
        // Perdió al enemigo por la izquierda: Arco agresivo hacia la izquierda
        moverSuave(motor, VelAvanceBase, VelGiroExterior, 0);
    }
}

void ControlMovimiento::ejecutar(const DecisionMovimiento& decision, IMotor& motor) const {
    if (decision.error != 0) actualizarBusqueda(decision.error);

    // 1. CORRECCIÓN: CANCELACIÓN DE EVASIÓN (INTERRUPCIÓN TÁCTICA)
    // Si el robot estaba evadiendo, PERO ya dejó de pisar la línea blanca 
    // y un enemigo se le cruza, cancela la maniobra de inmediato y ataca.
    bool esAtaque = (decision.tipo == TipoAccion::AtaqueFrontal || 
                     decision.tipo == TipoAccion::CorregirIzq ||
                     decision.tipo == TipoAccion::CorregirDer ||
                     decision.tipo == TipoAccion::AtaqueLateralIzq ||
                     decision.tipo == TipoAccion::AtaqueLateralDer);

    if (esAtaque && faseEvasion > 0) {
        faseEvasion = 0; // Cancela el retroceso o giro restante
    }

    // 2. INICIAR MÁQUINA DE ESTADOS (FSM) DE EVASIÓN
    if ((decision.tipo == TipoAccion::EvadirBordeIzq || 
         decision.tipo == TipoAccion::EvadirBordeDer || 
         decision.tipo == TipoAccion::EvadirBordeAmbos) && faseEvasion == 0) {
        faseEvasion = 1;
        tiempoInicioEvasion = millis();
        tipoEvasionActual = decision.tipo;
    }

    // 3. EJECUTAR EVASIÓN ASÍNCRONA
    if (faseEvasion > 0) {
        unsigned long transcurrido = millis() - tiempoInicioEvasion;
        
        if (faseEvasion == 1) { // Fase de Reversa
            motor.mover(-VelocidadMaxima, -VelocidadMaxima);
            if (transcurrido >= RETROCESO_MS) {
                faseEvasion = 2; 
                tiempoInicioEvasion = millis(); 
            }
        } else if (faseEvasion == 2) { // Fase de Giro Rápido
            if (tipoEvasionActual == TipoAccion::EvadirBordeIzq) {
                moverSuave(motor, VelocidadMaxima, -VelocidadMaxima, 0);
            } else if (tipoEvasionActual == TipoAccion::EvadirBordeDer) {
                moverSuave(motor, -VelocidadMaxima, VelocidadMaxima, 0);
            } else { 
                if (obtenerSesgoBusqueda() > 0) moverSuave(motor, VelocidadMaxima, -VelocidadMaxima, 0);
                else moverSuave(motor, -VelocidadMaxima, VelocidadMaxima, 0);
            }
            
            if (millis() - tiempoInicioEvasion >= GIRO_EVASION_MS) {
                faseEvasion = 0; 
            }
        }
        
        ultimaAccion = tipoEvasionActual;
        return; 
    }

    // 4. MODO ATAQUE Y BÚSQUEDA
    switch (decision.tipo) {
    case TipoAccion::AtaqueFrontal:
        moverSuave(motor, VelocidadMaxima, VelocidadMaxima, calcularPID(regulador, decision.error));
        break;
    case TipoAccion::CorregirIzq:
        moverSuave(motor, VelocidadCurva, VelocidadMaxima, calcularPID(regulador, decision.error));
        break;
    case TipoAccion::CorregirDer:
        moverSuave(motor, VelocidadMaxima, VelocidadCurva, calcularPID(regulador, decision.error));
        break;
    case TipoAccion::DefensaIzq:
        moverSuave(motor, -VelocidadPivoteLateral, VelocidadPivoteLateral, calcularPID(regulador, decision.error));
        break;
    case TipoAccion::DefensaDer:
        moverSuave(motor, VelocidadPivoteLateral, -VelocidadPivoteLateral, calcularPID(regulador, decision.error));
        break;
    case TipoAccion::AtaqueLateralIzq:
        moverSuave(motor, -VelocidadMaxima, VelocidadMaxima, calcularPID(regulador, decision.error));
        break;
    case TipoAccion::AtaqueLateralDer:
        moverSuave(motor, VelocidadMaxima, -VelocidadMaxima, calcularPID(regulador, decision.error));
        break;

    case TipoAccion::Busqueda:
        calcularPID(regulador, 0); 
        ejecutarBusqueda(motor);
        break;
    default:
        break; 
    }

    ultimaAccion = decision.tipo;
}