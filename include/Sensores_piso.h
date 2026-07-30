
#ifndef SENSORES_PISO_H
#define SENSORES_PISO_H

#include "ISensores_de_piso.h"

#include <stdint.h>


class SensoresDePiso : public ISensores_de_piso {
public:
    void inicializarSensoresDePiso() override;
    void Lecturas_de_piso_con_filtro() override;
    void obtenerLecturasSensoresDePiso(uint8_t& lecturas) override;
    void FiltrosDeKlaman() override;
};

#endif