#include "Estrategias.h"
#include "Motor.H"

void Estrategias::ejecutarEstrategia(Estados::Estado estadoActual, Motor& motor) {
    switch (estadoActual) {
        case Estados::BUSQUEDA:
            // Lógica para el estado de búsqueda
            break;
        case Estados::ATAQUE:
            // Lógica para el estado de ataque
            break;
        case Estados::PERSECUSION_ENEMIGO_45_GRADOS:
            // Lógica para la persecución del enemigo a 45 grados
            break;
        case Estados::RETROCESO:
            // Lógica para el estado de retroceso
            break;
        case Estados::PERSECUSION_LATERAL:
            // Lógica para la persecución lateral
            break;
        default:
            // Manejo de estados desconocidos
            break;
    }
}
