#include <Arduino.h>
#include "Pines.H"
#include "ConfiguracionInicial.h"

class ConfiguracionInicial : public ConfiguracionInicial {
public:
    ConfiguracionInicial(
        Estrategias& estrategiasRef,
        LecturaSensoresEnemigos& lecturaSensoresEnemigosRef,
        SensoresDePiso& sensoresDePisoRef,
        Motor& motorRef
    );
    {
            estrategias = estrategiasRef;
            lecturaSensoresEnemigos = lecturaSensoresEnemigosRef;
            sensoresDePiso = sensoresDePisoRef;
            motor = motorRef;
    }
    void inicializarComponentes( ) {
        estrategias.ejecutarEstrategia();
        lecturaSensoresEnemigos.inicializarSensoresEnemigos();
        sensoresDePiso.inicializarSensoresDePiso();
        motor.Detener();
    }

};  


