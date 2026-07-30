#include <unity.h>
#include <time.h>
#include <stdio.h>

#include "Percepcion.H"

extern "C" void test_set_silencio_digital(int activo);

namespace {

struct MetricasLectura {
    unsigned long lineaIzq = 0;
    unsigned long lineaDer = 0;
    unsigned long enemigo = 0;
};

MetricasLectura ejecutarMuestras(Percepcion& percepcion, unsigned long muestras) {
    MetricasLectura metricas;
    for (unsigned long i = 0; i < muestras; ++i) {
        const LecturasSensores lecturas = percepcion.leer();
        metricas.lineaIzq += lecturas.lineaIzq ? 1u : 0u;
        metricas.lineaDer += lecturas.lineaDer ? 1u : 0u;
        metricas.enemigo += (lecturas.latIzq || lecturas.c45Izq || lecturas.frontal || lecturas.c45Der || lecturas.latDer) ? 1u : 0u;
    }
    return metricas;
}

} // namespace

void test_persistencia_500k_muestras_consecutivas(void) {
    Percepcion percepcion;
    const unsigned long muestras = 500000ul;

    test_set_silencio_digital(0);
    const MetricasLectura metricas = ejecutarMuestras(percepcion, muestras);

    TEST_ASSERT_TRUE(metricas.lineaIzq > (muestras / 2ul));
    TEST_ASSERT_TRUE(metricas.lineaDer < (muestras / 2ul));
    TEST_ASSERT_TRUE(metricas.enemigo > 0ul);

    // Después de una fase larga de carga, fuerza sensores digitales en LOW
    // para comprobar que la retención se limpia y no queda saturada.
    test_set_silencio_digital(1);
    const MetricasLectura recuperacion = ejecutarMuestras(percepcion, 20ul);
    TEST_ASSERT_TRUE(recuperacion.enemigo <= 2ul);
}

void test_rendimiento_y_saturacion_aproximada(void) {
    Percepcion percepcion;
    unsigned long muestras = 1000ul;
    unsigned long ultimoEstable = muestras;
    double ultimoMs = 0.0;

    while (muestras <= 2000000ul) {
        const clock_t inicio = clock();
        (void)ejecutarMuestras(percepcion, muestras);
        const clock_t fin = clock();

        const double ms = (1000.0 * static_cast<double>(fin - inicio)) / static_cast<double>(CLOCKS_PER_SEC);
        ultimoEstable = muestras;
        ultimoMs = ms;

        if (ms > 200.0) {
            break;
        }

        muestras *= 2ul;
    }

    char mensaje[160];
    snprintf(
        mensaje,
        sizeof(mensaje),
        "Rendimiento: estable hasta %lu muestras consecutivas (%.2f ms)",
        ultimoEstable,
        ultimoMs
    );
    printf("%s\n", mensaje);
    TEST_MESSAGE(mensaje);

    TEST_ASSERT_TRUE(ultimoEstable >= 500000ul);
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_persistencia_500k_muestras_consecutivas);
    RUN_TEST(test_rendimiento_y_saturacion_aproximada);
    return UNITY_END();
}
