
#include "ILecturaSensoresEnemigos.h"
#ifndef LECTURA_SENSORES_ENEMIGOS_H
#define LECTURA_SENSORES_ENEMIGOS_H

#include <stdint.h>

class LecturaSensoresEnemigos: public ILecturaSensoresEnemigos {
public:
    void inicializarSensoresEnemigos() override;
    void Lecturas_de_sensores_enemigos() override;
    void Lecturas_de_sensores_enemigos_con_filtro() override;
    void obtenerLecturasSensoresEnemigos(uint8_t& lecturas) override;
    void FiltrosDeKlaman() override;
};


#endif