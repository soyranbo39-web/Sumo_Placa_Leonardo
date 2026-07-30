#ifndef DECISION_MOVIMIENTO_H
#define DECISION_MOVIMIENTO_H

#include <stdint.h>

enum class TipoAccion : uint8_t {
    EvadirBordeIzq,
    EvadirBordeDer,
    EvadirBordeAmbos,
    AtaqueFrontal,
    CorregirIzq,
    CorregirDer,
    DefensaIzq,
    DefensaDer,
    AtaqueLateralIzq,
    AtaqueLateralDer,
    Busqueda
};

struct DecisionMovimiento {
    TipoAccion tipo;
    int8_t error;

    DecisionMovimiento() : tipo(TipoAccion::Busqueda), error(0) {}
    DecisionMovimiento(TipoAccion tipoMovimiento, int8_t errorDireccion = 0)
        : tipo(tipoMovimiento), error(errorDireccion) {}
};

static_assert(sizeof(TipoAccion) == 1, "TipoAccion debe ocupar 1 byte");
static_assert(sizeof(DecisionMovimiento) == 2, "DecisionMovimiento debe ocupar 2 bytes");

#endif
