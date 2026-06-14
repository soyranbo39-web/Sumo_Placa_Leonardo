#ifndef CONFIGURACION_HARDWARE_H
#define CONFIGURACION_HARDWARE_H

#include "IConfiguracionHardware.H"

class ConfiguracionHardware: public IConfiguracionHardware {
public:
    void inicializarPines() const override;
};

#endif
