#include <Arduino.h>
#include "ControlMovimiento.H"
#include "Pines.H"
#include "UtilMatematica.H"

namespace {
// -- TIEMPOS DE EVASIÓN Y PREDICCIÓN --
constexpr unsigned long RETROCESO_MS = 300;     
constexpr unsigned long GIRO_EVASION_MS = 250;  

// ODOMETRÍA CONTEXTUAL (Ajusta estos dos valores en tus pruebas)
constexpr unsigned long TIEMPO_CRUCE_RADIO = 200;    // Tiempo del centro al borde
constexpr unsigned long TIEMPO_CRUCE_DIAMETRO = 450; // Tiempo de un borde al otro borde

unsigned long limiteCruceActual = TIEMPO_CRUCE_RADIO; // Inicia asumiendo que está en el centro

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
    static unsigned long ultimaVezLlamado = 0;
    static unsigned long inicioAvanceRecto = 0;
    static unsigned long ultimoCambioAleatorio = 0;
    
    static int16_t velIzqAleatoria = VelocidadMaxima;
    static int16_t velDerAleatoria = VelocidadMaxima;
    static bool modoPanicoAnticipado = false;

    if (millis() - ultimaVezLlamado > 50) {
        inicioAvanceRecto = millis();
        modoPanicoAnticipado = false;
    }
    ultimaVezLlamado = millis();

    // 1. RADAR DE TIEMPO INTELIGENTE (Usa el límite actual según dónde esté)
    if (!modoPanicoAnticipado && velIzqAleatoria == VelocidadMaxima && velDerAleatoria == VelocidadMaxima) {
        if (millis() - inicioAvanceRecto > limiteCruceActual) {
            modoPanicoAnticipado = true;
            ultimoCambioAleatorio = millis(); 
            
            if (obtenerSesgoBusqueda() > 0) { velIzqAleatoria = VelocidadMaxima; velDerAleatoria = -100; }
            else { velIzqAleatoria = -100; velDerAleatoria = VelocidadMaxima; }
        }
    }

    // 2. BÚSQUEDA ALEATORIA NORMAL
    if (millis() - ultimoCambioAleatorio > (modoPanicoAnticipado ? 250 : 400)) {
        ultimoCambioAleatorio = millis();
        modoPanicoAnticipado = false;
        
        long suerte = random(0, 100);
        
        if (suerte < 60) {
            velIzqAleatoria = VelocidadMaxima;
            velDerAleatoria = VelocidadMaxima;
            inicioAvanceRecto = millis(); 
        } else if (suerte < 80) {
            velIzqAleatoria = VelocidadMaxima / 2;
            velDerAleatoria = VelocidadMaxima;
        } else {
            velIzqAleatoria = VelocidadMaxima;
            velDerAleatoria = VelocidadMaxima / 2;
        }
    }

    moverSuave(motor, velIzqAleatoria, velDerAleatoria, 0);
}

void ControlMovimiento::ejecutar(const DecisionMovimiento& decision, IMotor& motor) const {
    if (decision.error != 0) actualizarBusqueda(decision.error);

    // 1. CANCELACIÓN DE EVASIÓN (INTERRUPCIÓN TÁCTICA)
    bool esAtaque = (decision.tipo == TipoAccion::AtaqueFrontal || 
                     decision.tipo == TipoAccion::CorregirIzq ||
                     decision.tipo == TipoAccion::CorregirDer ||
                     decision.tipo == TipoAccion::AtaqueLateralIzq ||
                     decision.tipo == TipoAccion::AtaqueLateralDer);

    if (esAtaque && faseEvasion > 0) {
        faseEvasion = 0; 
    }

    // 2. INICIAR MÁQUINA DE ESTADOS (FSM) DE EVASIÓN FÍSICA
    if ((decision.tipo == TipoAccion::EvadirBordeIzq || 
         decision.tipo == TipoAccion::EvadirBordeDer || 
         decision.tipo == TipoAccion::EvadirBordeAmbos) && faseEvasion == 0) {
        
        faseEvasion = 1;
        tiempoInicioEvasion = millis();
        tipoEvasionActual = decision.tipo;

        // ¡IMPORTANTE! El robot tocó físicamente la línea.
        // Ahora sabemos con certeza que está en la orilla del dojo.
        // Le damos permiso para usar el temporizador largo en su siguiente avance.
        limiteCruceActual = TIEMPO_CRUCE_DIAMETRO;
    }

    // 3. EJECUTAR EVASIÓN DINÁMICA ASÍNCRONA
    if (faseEvasion > 0) {
        unsigned long transcurrido = millis() - tiempoInicioEvasion;
        
        if (faseEvasion == 1) { // FASE 1: RETROCESO INTELIGENTE
            if (tipoEvasionActual == TipoAccion::EvadirBordeIzq) {
                moverSuave(motor, -VelocidadMaxima, -100, 0); 
            } 
            else if (tipoEvasionActual == TipoAccion::EvadirBordeDer) {
                moverSuave(motor, -100, -VelocidadMaxima, 0);
            } 
            else { 
                motor.mover(-VelocidadMaxima, -VelocidadMaxima);
            }

            if (transcurrido >= RETROCESO_MS) {
                faseEvasion = 2; 
                tiempoInicioEvasion = millis(); 
            }
            
        } else if (faseEvasion == 2) { // FASE 2: GIRO DE POSICIONAMIENTO
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