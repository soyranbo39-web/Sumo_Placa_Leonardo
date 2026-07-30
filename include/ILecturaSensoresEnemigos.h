
#ifndef LecturaSensoresEnemigos_H
#define LecturaSensoresEnemigos_H

#include <stdint.h>

class ILecturaSensoresEnemigos {
public:
    virtual void inicializarSensoresEnemigos() = 0;
    virtual void Lecturas_de_sensores_enemigos() = 0;
    virtual void Lecturas_de_sensores_enemigos_con_filtro() = 0;
    virtual void obtenerLecturasSensoresEnemigos(uint8_t& lecturas) = 0;
    virtual void FiltrosDeKlaman() = 0;
};


#endif
