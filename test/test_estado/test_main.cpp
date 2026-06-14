#include <unity.h>
#include "Estados.H"

class MotorDummy : public IMotor {
public:
    void avanzar(int) override {}
    void retroceder(int) override {}
    void detener() override {}
    void girar(int) override {}
    void curva(int) override {}
    void mover(int, int) override {}
};

class PercepcionMock : public IPercepcion {
public:
    mutable int calls = 0;
    LecturasSensores next{};

    LecturasSensores leer() const override {
        calls++;
        return next;
    }
};

class EstrategiaMock : public IEstrategiaCombate {
public:
    mutable int calls = 0;
    mutable LecturasSensores recibido{};
    DecisionMovimiento salida{TipoAccion::Busqueda};

    DecisionMovimiento decidir(const LecturasSensores& lecturas) const override {
        calls++;
        recibido = lecturas;
        return salida;
    }
};

class ControlMock : public IControlMovimiento {
public:
    mutable int calls = 0;
    mutable TipoAccion ultima = TipoAccion::Busqueda;

    void ejecutar(const DecisionMovimiento& decision, IMotor&) const override {
        calls++;
        ultima = decision.tipo;
    }
};

void test_no_hace_nada_si_no_hay_motor(void) {
    PercepcionMock p;
    EstrategiaMock e;
    ControlMock c;
    Estado estado(p, e, c);

    estado.actualizarEstado();

    TEST_ASSERT_EQUAL_INT(0, p.calls);
    TEST_ASSERT_EQUAL_INT(0, e.calls);
    TEST_ASSERT_EQUAL_INT(0, c.calls);
}

void test_orquesta_leer_decidir_ejecutar(void) {
    PercepcionMock p;
    EstrategiaMock e;
    ControlMock c;
    MotorDummy m;

    p.next = {false, true, false, false, false, false, false};
    e.salida = {TipoAccion::EvadirBordeDer};

    Estado estado(p, e, c);
    estado.setMotor(&m);
    estado.actualizarEstado();

    TEST_ASSERT_EQUAL_INT(1, p.calls);
    TEST_ASSERT_EQUAL_INT(1, e.calls);
    TEST_ASSERT_EQUAL_INT(1, c.calls);
    TEST_ASSERT_EQUAL_INT((int)TipoAccion::EvadirBordeDer, (int)c.ultima);

    TEST_ASSERT_EQUAL_INT((int)p.next.lineaDer, (int)e.recibido.lineaDer);
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_no_hace_nada_si_no_hay_motor);
    RUN_TEST(test_orquesta_leer_decidir_ejecutar);
    return UNITY_END();
}
