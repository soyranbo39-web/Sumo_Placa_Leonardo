#include <unity.h>
#include "EstrategiaCombate.H"

static LecturasSensores L(
    bool lineaIzq,
    bool lineaDer,
    bool latIzq,
    bool c45Izq,
    bool frontal,
    bool c45Der,
    bool latDer
) {
    LecturasSensores s{};
    s.lineaIzq = lineaIzq;
    s.lineaDer = lineaDer;
    s.latIzq = latIzq;
    s.c45Izq = c45Izq;
    s.frontal = frontal;
    s.c45Der = c45Der;
    s.latDer = latDer;
    return s;
}

void test_prioriza_borde_sobre_todo(void) {
    EstrategiaCombate e;
    DecisionMovimiento d = e.decidir(L(true, false, true, true, true, true, true));
    TEST_ASSERT_EQUAL_INT((int)TipoAccion::EvadirBordeIzq, (int)d.tipo);
}

void test_frontal_sobre_45_y_laterales(void) {
    EstrategiaCombate e;
    DecisionMovimiento d = e.decidir(L(false, false, true, true, true, true, true));
    TEST_ASSERT_EQUAL_INT((int)TipoAccion::AtaqueFrontal, (int)d.tipo);
}

void test_45_izq_sobre_45_der_y_laterales(void) {
    EstrategiaCombate e;
    DecisionMovimiento d = e.decidir(L(false, false, true, true, false, true, true));
    TEST_ASSERT_EQUAL_INT((int)TipoAccion::CorregirIzq, (int)d.tipo);
}

void test_busqueda_si_no_hay_deteccion(void) {
    EstrategiaCombate e;
    DecisionMovimiento d = e.decidir(L(false, false, false, false, false, false, false));
    TEST_ASSERT_EQUAL_INT((int)TipoAccion::Busqueda, (int)d.tipo);
    TEST_ASSERT_EQUAL_INT(0, d.error);
}

void test_error_de_ajuste_se_calcula_por_promedio_de_sensores(void) {
    EstrategiaCombate e;
    DecisionMovimiento d = e.decidir(L(false, false, true, true, false, false, false));
    TEST_ASSERT_EQUAL_INT((int)TipoAccion::CorregirIzq, (int)d.tipo);
    TEST_ASSERT_TRUE(d.error < 0);
}

void test_sensor_lateral_izquierdo_activa_defensa_izquierda(void) {
    EstrategiaCombate e;
    DecisionMovimiento d = e.decidir(L(false, false, true, false, false, false, false));
    TEST_ASSERT_EQUAL_INT((int)TipoAccion::DefensaIzq, (int)d.tipo);
}

void test_sensor_lateral_derecho_activa_defensa_derecha(void) {
    EstrategiaCombate e;
    DecisionMovimiento d = e.decidir(L(false, false, false, false, false, false, true));
    TEST_ASSERT_EQUAL_INT((int)TipoAccion::DefensaDer, (int)d.tipo);
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_prioriza_borde_sobre_todo);
    RUN_TEST(test_frontal_sobre_45_y_laterales);
    RUN_TEST(test_45_izq_sobre_45_der_y_laterales);
    RUN_TEST(test_busqueda_si_no_hay_deteccion);
    RUN_TEST(test_error_de_ajuste_se_calcula_por_promedio_de_sensores);
    RUN_TEST(test_sensor_lateral_izquierdo_activa_defensa_izquierda);
    RUN_TEST(test_sensor_lateral_derecho_activa_defensa_derecha);
    return UNITY_END();
}
