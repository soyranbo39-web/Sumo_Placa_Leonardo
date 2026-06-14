#include "EstrategiaCombate.H"

DecisionMovimiento EstrategiaCombate::decidir(const LecturasSensores& lecturas) const {
    if (lecturas.lineaIzq || lecturas.lineaDer) {
        if (lecturas.lineaIzq && !lecturas.lineaDer) {
            return {TipoAccion::EvadirBordeIzq};
        }
        if (lecturas.lineaDer && !lecturas.lineaIzq) {
            return {TipoAccion::EvadirBordeDer};
        }
        return {TipoAccion::EvadirBordeAmbos};
    }

    if (lecturas.frontal) {
        return {TipoAccion::AtaqueFrontal};
    }

    if (lecturas.c45Izq) {
        return {TipoAccion::CorregirIzq};
    }

    if (lecturas.c45Der) {
        return {TipoAccion::CorregirDer};
    }

    if (lecturas.latIzq) {
        return {TipoAccion::DefensaIzq};
    }

    if (lecturas.latDer) {
        return {TipoAccion::DefensaDer};
    }

    return {TipoAccion::Busqueda};
}
