
#ifndef CONFIGURACIONINICIAL_H
#define CONFIGURACIONINICIAL_H
#include "Estados.h"
#include "Estrategias.h"
#include "LecturaSensoresEnemigos.h"
#include "Sensores_piso.h"
#include "Motor.H"

class ConfiguracionInicial {
public:
    ConfiguracionInicial();
    void inicializarComponentes();
private:
    Estrategias estrategias;
    LecturaSensoresEnemigos lecturaSensoresEnemigos;
    SensoresDePiso sensoresDePiso;
    Motor motor;
};

#endif


