#include <unity.h>
#include <cmath>
#include <cstdint>
#include <cstdio>

#include "ControlMovimiento.H"
#include "EstrategiaCombate.H"
#include "Pines.H"

extern "C" void test_set_simulador(void* ptr);

namespace {

constexpr float ARENA_CM = 70.0f;
constexpr float BORDE_CM = 2.0f;
constexpr float PREALERTA_BORDE_CM = 20.0f;
constexpr float DT_CONTROL_MS = 20.0f;
constexpr float DT_INTEGRACION_MS = 5.0f;
constexpr float SEMI_EJE_LATERAL = 5.0f;
constexpr float NARIZ_FRONTAL = 7.0f;
constexpr float OFFSET_SENSORES_PISO = 4.6f;
constexpr float COLA_TRASERA = 4.5f;
constexpr float DISTANCIA_RUEDAS_CM = 10.5f;
constexpr float PWM_A_VEL_CM_S = 0.255f;
constexpr float TAU_LINEAL_FRENADO = 0.24f;   // peso frontal alto: tarda en frenar
constexpr float TAU_LINEAL_ACEL = 0.15f;
constexpr float TAU_ANGULAR = 0.12f;
constexpr float AMORTIGUACION_TRASERA = 0.82f; // menor traccion trasera

struct EstadoFisico {
    float x = 35.0f;
    float y = 35.0f;
    float theta = 0.0f;
    float v = 0.0f;
    float w = 0.0f;
};

struct ConteoAcciones {
    uint32_t evadir = 0;
    uint32_t frontal = 0;
    uint32_t corregir = 0;
    uint32_t defensa = 0;
    uint32_t busqueda = 0;
};

struct ResultadoSimulacion {
    bool salio = false;
    uint32_t tickSalida = 0;
    EstadoFisico estadoFinal{};
    ConteoAcciones conteo{};
    TipoAccion ultimaAccion = TipoAccion::Busqueda;
};

class SimuladorCombate;
SimuladorCombate* g_sim = nullptr;

class MotorSim : public IMotor {
public:
    int velIzq = 0;
    int velDer = 0;

    void avanzar(int velocidad) override { mover(velocidad, velocidad); }
    void retroceder(int velocidad) override { mover(-velocidad, -velocidad); }
    void detener() override { mover(0, 0); }
    void girar(int velocidad) override { mover(velocidad, -velocidad); }
    void curva(int velocidad) override { mover(velocidad / 2, velocidad); }
    void mover(int izq, int der) override {
        velIzq = izq;
        velDer = der;
    }
};

class SimuladorCombate {
public:
    EstadoFisico estado;
    bool salio = false;
    MotorSim motor;
    ControlMovimiento control;
    EstrategiaCombate estrategia;
    ConteoAcciones conteo;
    TipoAccion ultimaAccion = TipoAccion::Busqueda;

    void integrarMs(unsigned long ms) {
        const uint32_t pasos = static_cast<uint32_t>(ms / static_cast<unsigned long>(DT_INTEGRACION_MS));
        const uint32_t totalPasos = (pasos == 0u) ? 1u : pasos;

        for (uint32_t i = 0; i < totalPasos; ++i) {
            const float dt = DT_INTEGRACION_MS / 1000.0f;
            const float vIzq = static_cast<float>(motor.velIzq) * PWM_A_VEL_CM_S;
            const float vDer = static_cast<float>(motor.velDer) * PWM_A_VEL_CM_S;
            const float objetivoV = 0.5f * (vIzq + vDer) * AMORTIGUACION_TRASERA;
            const float objetivoW = ((vDer - vIzq) / DISTANCIA_RUEDAS_CM) * AMORTIGUACION_TRASERA;

            const float tauLineal = (std::fabs(objetivoV) < std::fabs(estado.v)) ? TAU_LINEAL_FRENADO : TAU_LINEAL_ACEL;
            estado.v += (objetivoV - estado.v) * (dt / (tauLineal + dt));
            estado.w += (objetivoW - estado.w) * (dt / (TAU_ANGULAR + dt));

            estado.theta += estado.w * dt;
            estado.x += estado.v * std::cos(estado.theta) * dt;
            estado.y += estado.v * std::sin(estado.theta) * dt;

            if (fueraArenaFisica()) {
                salio = true;
                return;
            }
        }
    }

    LecturasSensores leerSensores(uint32_t tick) const {
        LecturasSensores lecturas{};

        const float c = std::cos(estado.theta);
        const float s = std::sin(estado.theta);

        const float fx = estado.x + (NARIZ_FRONTAL * c);
        const float fy = estado.y + (NARIZ_FRONTAL * s);
        const float lx = estado.x + (OFFSET_SENSORES_PISO * c) - (SEMI_EJE_LATERAL * s);
        const float ly = estado.y + (OFFSET_SENSORES_PISO * s) + (SEMI_EJE_LATERAL * c);
        const float rx = estado.x + (OFFSET_SENSORES_PISO * c) + (SEMI_EJE_LATERAL * s);
        const float ry = estado.y + (OFFSET_SENSORES_PISO * s) - (SEMI_EJE_LATERAL * c);

        lecturas.lineaIzq = enBorde(lx, ly);
        lecturas.lineaDer = enBorde(rx, ry);

        // Escenario circular del enemigo para forzar todos los movimientos.
        const uint32_t fase = tick % 600u;
        if (fase < 100u) {
            lecturas.frontal = 1;
        } else if (fase < 200u) {
            lecturas.c45Izq = 1;
        } else if (fase < 300u) {
            lecturas.c45Der = 1;
        } else if (fase < 400u) {
            lecturas.latIzq = 1;
        } else if (fase < 500u) {
            lecturas.latDer = 1;
        } else {
            // fase de busqueda
        }

        return lecturas;
    }

private:
    static bool enBorde(float x, float y) {
        const float limiteInterior = BORDE_CM + PREALERTA_BORDE_CM;
        const float limiteExterior = ARENA_CM - limiteInterior;
        return (x <= limiteInterior) || (x >= limiteExterior) || (y <= limiteInterior) || (y >= limiteExterior);
    }

    bool fueraArenaFisica() const {
        const float c = std::cos(estado.theta);
        const float s = std::sin(estado.theta);

        const float puntaX = estado.x + (NARIZ_FRONTAL * c);
        const float puntaY = estado.y + (NARIZ_FRONTAL * s);
        const float colaX = estado.x - (COLA_TRASERA * c);
        const float colaY = estado.y - (COLA_TRASERA * s);

        return (puntaX < 0.0f || puntaX > ARENA_CM || puntaY < 0.0f || puntaY > ARENA_CM ||
                colaX < 0.0f || colaX > ARENA_CM || colaY < 0.0f || colaY > ARENA_CM);
    }
};

extern "C" void delay(unsigned long ms) {
    if (g_sim != nullptr) {
        g_sim->integrarMs(ms);
    }
}

void ejecutarTick(SimuladorCombate& sim, uint32_t tick) {
    const LecturasSensores lecturas = sim.leerSensores(tick);
    const DecisionMovimiento decision = sim.estrategia.decidir(lecturas);
    sim.ultimaAccion = decision.tipo;

    switch (decision.tipo) {
    case TipoAccion::EvadirBordeIzq:
    case TipoAccion::EvadirBordeDer:
    case TipoAccion::EvadirBordeAmbos:
        ++sim.conteo.evadir;
        break;
    case TipoAccion::AtaqueFrontal:
        ++sim.conteo.frontal;
        break;
    case TipoAccion::CorregirIzq:
    case TipoAccion::CorregirDer:
        ++sim.conteo.corregir;
        break;
    case TipoAccion::DefensaIzq:
    case TipoAccion::DefensaDer:
        ++sim.conteo.defensa;
        break;
    case TipoAccion::Busqueda:
    default:
        ++sim.conteo.busqueda;
        break;
    }

    sim.control.ejecutar(decision, sim.motor);

    // Periodo base de control para acciones sin delay.
    sim.integrarMs(static_cast<unsigned long>(DT_CONTROL_MS));
}

ResultadoSimulacion ejecutarSimulacion(bool diagonal) {
    SimuladorCombate sim;
    g_sim = &sim;
    test_set_simulador(&sim);

    ResultadoSimulacion resultado;

    for (uint32_t tick = 0; tick < 5000u; ++tick) {
        ejecutarTick(sim, tick);

        if (diagonal) {
            sim.estado.theta += 0.0015f;
            sim.estado.x += 0.0006f;
            sim.estado.y += 0.0004f;
        }

        if (sim.salio) {
            resultado.salio = true;
            resultado.tickSalida = tick;
            resultado.ultimaAccion = sim.ultimaAccion;
            break;
        }
    }

    resultado.estadoFinal = sim.estado;
    resultado.conteo = sim.conteo;
    resultado.ultimaAccion = sim.ultimaAccion;
    return resultado;
}

} // namespace

void test_simulacion_70x70_no_sale_con_todos_los_movimientos(void) {
    const ResultadoSimulacion base = ejecutarSimulacion(false);
    const ResultadoSimulacion diagonal = ejecutarSimulacion(true);

    if (base.salio) {
        std::printf(
            "Base sale en tick=%lu x=%.2f y=%.2f theta=%.2f accion=%d\n",
            static_cast<unsigned long>(base.tickSalida),
            static_cast<double>(base.estadoFinal.x),
            static_cast<double>(base.estadoFinal.y),
            static_cast<double>(base.estadoFinal.theta),
            static_cast<int>(base.ultimaAccion)
        );
    }

    if (diagonal.salio) {
        std::printf(
            "Diagonal sale en tick=%lu x=%.2f y=%.2f theta=%.2f accion=%d\n",
            static_cast<unsigned long>(diagonal.tickSalida),
            static_cast<double>(diagonal.estadoFinal.x),
            static_cast<double>(diagonal.estadoFinal.y),
            static_cast<double>(diagonal.estadoFinal.theta),
            static_cast<int>(diagonal.ultimaAccion)
        );
    }

    TEST_ASSERT_FALSE(base.salio);
    TEST_ASSERT_FALSE(diagonal.salio);
    TEST_ASSERT_TRUE(base.conteo.frontal > 15u);
    TEST_ASSERT_TRUE(base.conteo.corregir > 15u);
    TEST_ASSERT_TRUE(base.conteo.evadir > 5u);
    TEST_ASSERT_TRUE(diagonal.conteo.frontal > 20u);
    TEST_ASSERT_TRUE(diagonal.conteo.corregir > 20u);
    TEST_ASSERT_TRUE(diagonal.conteo.defensa > 20u);
    TEST_ASSERT_TRUE(diagonal.conteo.busqueda > 20u);
    TEST_ASSERT_TRUE(diagonal.conteo.evadir > 5u);
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_simulacion_70x70_no_sale_con_todos_los_movimientos);
    return UNITY_END();
}
