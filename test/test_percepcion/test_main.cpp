#include <unity.h>
#include "Arduino.h"
#include "Percepcion.H"
#include "DecisionMovimiento.H"
#include "UtilMatematica.H"

extern "C" void test_reset_percepcion_stubs(void);
extern "C" void test_set_digital_sequence(const int* values, int count);
extern "C" void test_set_analog_value(uint8_t pin, int value);

static void configurarSecuencia(int v0, int v1, int v2,
                                int v3, int v4, int v5,
                                int v6, int v7, int v8,
                                int v9, int v10, int v11,
                                int v12, int v13, int v14) {
    const int valores[15] = {
        v0, v1, v2, v3, v4,
        v5, v6, v7, v8, v9,
        v10, v11, v12, v13, v14
    };
    test_set_digital_sequence(valores, 15);
}

void test_tipos_de_movimiento_compactos_en_memoria(void) {
    TEST_ASSERT_EQUAL_UINT8(1, sizeof(LecturasSensores));
    TEST_ASSERT_EQUAL_UINT8(1, sizeof(TipoAccion));
    TEST_ASSERT_EQUAL_UINT8(2, sizeof(DecisionMovimiento));
}

void test_funciones_matematicas_optimizadas(void) {
    TEST_ASSERT_EQUAL_INT(10, limitar<int16_t>(12, 0, 10));
    TEST_ASSERT_EQUAL_INT(-10, limitar<int16_t>(-12, -10, 10));
    TEST_ASSERT_EQUAL_INT(4, promedioEntero(12, 3));
}

void test_percepcion_aplica_mayoria_y_retencion(void) {
    Percepcion percepcion;

    test_reset_percepcion_stubs();
    test_set_analog_value(A1, 100);
    test_set_analog_value(A2, 100);
    configurarSecuencia(
        1, 0, 1,  // latIzq -> true
        0, 0, 1,  // c45Izq -> false
        0, 0, 0,  // frontal -> false
        1, 1, 0,  // c45Der -> true
        0, 0, 0   // latDer -> false
    );

    LecturasSensores primera = percepcion.leer();
    TEST_ASSERT_TRUE(primera.latIzq);
    TEST_ASSERT_FALSE(primera.c45Izq);
    TEST_ASSERT_TRUE(primera.c45Der);
    TEST_ASSERT_FALSE(primera.frontal);

    LecturasSensores segunda = percepcion.leer();
    TEST_ASSERT_TRUE(segunda.latIzq);
    TEST_ASSERT_TRUE(segunda.c45Der);

    LecturasSensores tercera = percepcion.leer();
    TEST_ASSERT_TRUE(tercera.latIzq);
    TEST_ASSERT_TRUE(tercera.c45Der);

    LecturasSensores cuarta = percepcion.leer();
    TEST_ASSERT_FALSE(cuarta.latIzq);
    TEST_ASSERT_FALSE(cuarta.c45Der);
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_tipos_de_movimiento_compactos_en_memoria);
    RUN_TEST(test_funciones_matematicas_optimizadas);
    RUN_TEST(test_percepcion_aplica_mayoria_y_retencion);
    return UNITY_END();
}
