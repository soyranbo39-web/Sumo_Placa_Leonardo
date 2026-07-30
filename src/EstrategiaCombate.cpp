#include "EstrategiaCombate.H"
#include "UtilMatematica.H"

namespace {
constexpr int8_t PESO_LAT_IZQ = -4;
constexpr int8_t PESO_C45_IZQ = -2;
constexpr int8_t PESO_FRONTAL = 0;
constexpr int8_t PESO_C45_DER = 2;
constexpr int8_t PESO_LAT_DER = 4;
constexpr uint8_t RETENCION_OBJETIVO_CICLOS = 6;
}

int8_t EstrategiaCombate::calcularErrorDireccion(const LecturasSensores& lecturas) {
    int16_t acumulado = 0;
    int16_t cantidad = 0;

    if (lecturas.latIzq) {
        acumulado += PESO_LAT_IZQ;
        ++cantidad;
    }
    if (lecturas.c45Izq) {
        acumulado += PESO_C45_IZQ;
        ++cantidad;
    }
    if (lecturas.frontal) {
        acumulado += PESO_FRONTAL;
        ++cantidad;
    }
    if (lecturas.c45Der) {
        acumulado += PESO_C45_DER;
        ++cantidad;
    }
    if (lecturas.latDer) {
        acumulado += PESO_LAT_DER;
        ++cantidad;
    }

    return static_cast<int8_t>(promedioEntero(acumulado, cantidad));
}

DecisionMovimiento EstrategiaCombate::decidir(const LecturasSensores& lecturas) const {
    if (lecturas.lineaIzq || lecturas.lineaDer) {
        retencionObjetivo = 0;
        if (lecturas.lineaIzq && !lecturas.lineaDer) {
            return DecisionMovimiento{TipoAccion::EvadirBordeIzq, 0};
        }
        if (lecturas.lineaDer && !lecturas.lineaIzq) {
            return DecisionMovimiento{TipoAccion::EvadirBordeDer, 0};
        }
        return DecisionMovimiento{TipoAccion::EvadirBordeAmbos, 0};
    }

    const bool hayEnemigo = lecturas.latIzq || lecturas.c45Izq || lecturas.frontal || lecturas.c45Der || lecturas.latDer;
    if (!hayEnemigo) {
        if (retencionObjetivo > 0) {
            --retencionObjetivo;
            if (ultimoErrorObjetivo <= -2) {
                return DecisionMovimiento{TipoAccion::CorregirIzq, ultimoErrorObjetivo};
            }
            if (ultimoErrorObjetivo >= 2) {
                return DecisionMovimiento{TipoAccion::CorregirDer, ultimoErrorObjetivo};
            }
            return DecisionMovimiento{TipoAccion::AtaqueFrontal, 0};
        }
        return DecisionMovimiento{TipoAccion::Busqueda, 0};
    }

    if (lecturas.frontal || lecturas.c45Izq || lecturas.c45Der) {
        const int8_t error = calcularErrorDireccion(lecturas);
        ultimoErrorObjetivo = error;
        retencionObjetivo = RETENCION_OBJETIVO_CICLOS;
        if (lecturas.frontal) {
            return DecisionMovimiento{TipoAccion::AtaqueFrontal, error};
        }

        if (lecturas.c45Izq) {
            return DecisionMovimiento{TipoAccion::CorregirIzq, error};
        }

        return DecisionMovimiento{TipoAccion::CorregirDer, error};
    }

    if (lecturas.latIzq) {
        ultimoErrorObjetivo = PESO_LAT_IZQ;
        retencionObjetivo = RETENCION_OBJETIVO_CICLOS;
        return DecisionMovimiento{TipoAccion::DefensaIzq, 0};
    }

    if (lecturas.latDer) {
        ultimoErrorObjetivo = PESO_LAT_DER;
        retencionObjetivo = RETENCION_OBJETIVO_CICLOS;
        return DecisionMovimiento{TipoAccion::DefensaDer, 0};
    }

    return DecisionMovimiento{TipoAccion::Busqueda, 0};
}
