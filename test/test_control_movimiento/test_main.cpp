#include <unity.h>
#include "ControlMovimiento.H"
#include "Pines.H"
#include "Arduino.h"

class MotorMock : public IMotor {
public:
    int calls = 0;
    int izq[32] = {0};
    int der[32] = {0};

    void avanzar(int) override {}
    void retroceder(int) override {}
    void detener() override {}
    void girar(int) override {}
    void curva(int) override {}

    void mover(int velIzq, int velDer) override {
        if (calls < 32) {
            izq[calls] = velIzq;
            der[calls] = velDer;
        }
        calls++;
    }
};

static void resetDelays() {
    g_delayCallCount = 0;
    for (int i = 0; i < 16; i++) {
        g_delayValues[i] = 0;
    }
}

void test_ataque_frontal_velocidades_correctas(void) {
    ControlMovimiento c;
    MotorMock m;
    resetDelays();

    c.ejecutar({TipoAccion::AtaqueFrontal}, m);

    TEST_ASSERT_EQUAL_INT(1, m.calls);
    TEST_ASSERT_EQUAL_INT(VelocidadAtaqueFrontal, m.izq[0]);
    TEST_ASSERT_EQUAL_INT(VelocidadAtaqueFrontal, m.der[0]);
    TEST_ASSERT_EQUAL_INT(0, g_delayCallCount);
}

void test_evadir_borde_izq_hace_retroceso_y_giro_con_delays(void) {
    ControlMovimiento c;
    MotorMock m;
    resetDelays();

    c.ejecutar({TipoAccion::EvadirBordeIzq}, m);

    TEST_ASSERT_EQUAL_INT(2, m.calls);
    TEST_ASSERT_EQUAL_INT(-VelocidadRetroceso, m.izq[0]);
    TEST_ASSERT_EQUAL_INT(-VelocidadRetroceso, m.der[0]);
    TEST_ASSERT_EQUAL_INT(0, m.izq[1]);
    TEST_ASSERT_EQUAL_INT(VelocidadMaxima, m.der[1]);

    TEST_ASSERT_EQUAL_INT(2, g_delayCallCount);
    TEST_ASSERT_EQUAL_UINT32(280, g_delayValues[0]);
    TEST_ASSERT_EQUAL_UINT32(260, g_delayValues[1]);
}

void test_busqueda_por_defecto(void) {
    ControlMovimiento c;
    MotorMock m;
    resetDelays();

    c.ejecutar({TipoAccion::Busqueda}, m);

    TEST_ASSERT_EQUAL_INT(1, m.calls);
    TEST_ASSERT_TRUE(m.izq[0] > m.der[0]);
    TEST_ASSERT_TRUE(m.izq[0] < VelocidadAvance);
    TEST_ASSERT_TRUE(m.der[0] < VelocidadAvance);
}

void test_busqueda_barre_lentamente_el_ring(void) {
    ControlMovimiento c;
    MotorMock m;
    resetDelays();

    for (int i = 0; i < 8; ++i) {
        c.ejecutar({TipoAccion::Busqueda}, m);
    }

    TEST_ASSERT_EQUAL_INT(8, m.calls);
    TEST_ASSERT_TRUE(m.izq[0] > m.der[0]);
    TEST_ASSERT_EQUAL_INT(m.izq[3], m.der[3]);
    TEST_ASSERT_EQUAL_INT(m.izq[4], m.der[4]);
    TEST_ASSERT_TRUE(m.izq[7] < m.der[7]);
    TEST_ASSERT_TRUE(m.izq[0] < VelocidadMaxima);
    TEST_ASSERT_TRUE(m.der[0] < VelocidadMaxima);
}

void test_busqueda_recuerda_el_ultimo_lado_detectado(void) {
    ControlMovimiento c;
    MotorMock m;
    resetDelays();

    c.ejecutar({TipoAccion::AtaqueFrontal, 4}, m);
    c.ejecutar({TipoAccion::Busqueda}, m);
    c.ejecutar({TipoAccion::Busqueda}, m);

    TEST_ASSERT_EQUAL_INT(3, m.calls);
    TEST_ASSERT_EQUAL_INT(-VelocidadMaxima, m.izq[1]);
    TEST_ASSERT_EQUAL_INT(-VelocidadMaxima, m.der[1]);
    TEST_ASSERT_TRUE(m.izq[2] < m.der[2]);
}

void test_defensa_izq_hace_pivote_con_una_sola_llanta(void) {
    ControlMovimiento c;
    MotorMock m;
    resetDelays();

    c.ejecutar({TipoAccion::DefensaIzq}, m);

    TEST_ASSERT_EQUAL_INT(1, m.calls);
    TEST_ASSERT_EQUAL_INT(0, m.izq[0]);
    TEST_ASSERT_EQUAL_INT(VelocidadPivoteLateral, m.der[0]);
    TEST_ASSERT_EQUAL_INT(0, g_delayCallCount);
}

void test_defensa_der_hace_pivote_con_una_sola_llanta(void) {
    ControlMovimiento c;
    MotorMock m;
    resetDelays();

    c.ejecutar({TipoAccion::DefensaDer}, m);

    TEST_ASSERT_EQUAL_INT(1, m.calls);
    TEST_ASSERT_EQUAL_INT(VelocidadPivoteLateral, m.izq[0]);
    TEST_ASSERT_EQUAL_INT(0, m.der[0]);
    TEST_ASSERT_EQUAL_INT(0, g_delayCallCount);
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_ataque_frontal_velocidades_correctas);
    RUN_TEST(test_evadir_borde_izq_hace_retroceso_y_giro_con_delays);
    RUN_TEST(test_busqueda_por_defecto);
    RUN_TEST(test_busqueda_barre_lentamente_el_ring);
    RUN_TEST(test_busqueda_recuerda_el_ultimo_lado_detectado);
    RUN_TEST(test_defensa_izq_hace_pivote_con_una_sola_llanta);
    RUN_TEST(test_defensa_der_hace_pivote_con_una_sola_llanta);
    return UNITY_END();
}
