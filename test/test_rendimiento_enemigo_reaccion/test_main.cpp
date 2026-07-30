#include <unity.h>
#include <time.h>
#include <stdio.h>

#include "Percepcion.H"
#include "EstrategiaCombate.H"
#include "ControlMovimiento.H"

extern "C" void test_set_silencio_digital_enemigo(int activo);

namespace {

bool hayEnemigo(const LecturasSensores& lecturas) {
    return lecturas.latIzq || lecturas.c45Izq || lecturas.frontal || lecturas.c45Der || lecturas.latDer;
}

class MotorMock : public IMotor {
public:
    unsigned long calls = 0;
    unsigned long movimientosActivos = 0;

    void avanzar(int) override {}
    void retroceder(int) override {}
    void detener() override {}
    void girar(int) override {}
    void curva(int) override {}

    void mover(int velIzq, int velDer) override {
        ++calls;
        if (velIzq != 0 || velDer != 0) {
            ++movimientosActivos;
        }
    }
};

struct ConteoAcciones {
    unsigned long frontal = 0;
    unsigned long corregir = 0;
    unsigned long defensa = 0;
    unsigned long busqueda = 0;
};

ConteoAcciones ejecutarPipeline(
    Percepcion& percepcion,
    EstrategiaCombate& estrategia,
    ControlMovimiento& control,
    MotorMock& motor,
    unsigned long muestras,
    unsigned long* deteccionesEnemigo
) {
    ConteoAcciones conteo;
    unsigned long detecciones = 0;

    for (unsigned long i = 0; i < muestras; ++i) {
        const LecturasSensores lecturas = percepcion.leer();
        detecciones += hayEnemigo(lecturas) ? 1u : 0u;

        const DecisionMovimiento decision = estrategia.decidir(lecturas);
        control.ejecutar(decision, motor);

        if (decision.tipo == TipoAccion::AtaqueFrontal) {
            ++conteo.frontal;
        } else if (decision.tipo == TipoAccion::CorregirIzq || decision.tipo == TipoAccion::CorregirDer) {
            ++conteo.corregir;
        } else if (decision.tipo == TipoAccion::DefensaIzq || decision.tipo == TipoAccion::DefensaDer) {
            ++conteo.defensa;
        } else if (decision.tipo == TipoAccion::Busqueda) {
            ++conteo.busqueda;
        }
    }

    if (deteccionesEnemigo != nullptr) {
        *deteccionesEnemigo = detecciones;
    }

    return conteo;
}

} // namespace

void test_persistencia_deteccion_enemigo_y_desaturacion(void) {
    Percepcion percepcion;
    EstrategiaCombate estrategia;
    ControlMovimiento control;
    MotorMock motor;

    test_set_silencio_digital_enemigo(0);

    unsigned long deteccionesActivas = 0;
    const ConteoAcciones activas = ejecutarPipeline(
        percepcion,
        estrategia,
        control,
        motor,
        500000ul,
        &deteccionesActivas
    );

    TEST_ASSERT_TRUE(deteccionesActivas > 450000ul);
    TEST_ASSERT_EQUAL_UINT32(500000ul, static_cast<uint32_t>(motor.calls));
    TEST_ASSERT_TRUE(activas.busqueda < 1000ul);
    TEST_ASSERT_TRUE((activas.frontal + activas.corregir + activas.defensa) > 495000ul);

    test_set_silencio_digital_enemigo(1);
    unsigned long deteccionesSilencio = 0;
    const ConteoAcciones silencio = ejecutarPipeline(
        percepcion,
        estrategia,
        control,
        motor,
        40ul,
        &deteccionesSilencio
    );

    TEST_ASSERT_TRUE(deteccionesSilencio <= 2ul);
    TEST_ASSERT_TRUE(silencio.busqueda >= 38ul);
}

void test_rendimiento_reaccion_enemigo_muestras_seguidas(void) {
    Percepcion percepcion;
    EstrategiaCombate estrategia;
    ControlMovimiento control;
    MotorMock motor;

    test_set_silencio_digital_enemigo(0);

    unsigned long muestras = 1000ul;
    unsigned long ultimoEstable = muestras;
    double ultimoMs = 0.0;

    while (muestras <= 2000000ul) {
        const clock_t inicio = clock();
        (void)ejecutarPipeline(percepcion, estrategia, control, motor, muestras, nullptr);
        const clock_t fin = clock();

        const double ms = (1000.0 * static_cast<double>(fin - inicio)) / static_cast<double>(CLOCKS_PER_SEC);
        ultimoEstable = muestras;
        ultimoMs = ms;

        if (ms > 250.0) {
            break;
        }

        muestras *= 2ul;
    }

    char mensaje[180];
    snprintf(
        mensaje,
        sizeof(mensaje),
        "Reaccion enemigo: estable hasta %lu muestras consecutivas (%.2f ms)",
        ultimoEstable,
        ultimoMs
    );
    printf("%s\n", mensaje);
    TEST_MESSAGE(mensaje);

    TEST_ASSERT_TRUE(ultimoEstable >= 500000ul);
    TEST_ASSERT_TRUE(motor.movimientosActivos > 0ul);
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_persistencia_deteccion_enemigo_y_desaturacion);
    RUN_TEST(test_rendimiento_reaccion_enemigo_muestras_seguidas);
    return UNITY_END();
}
