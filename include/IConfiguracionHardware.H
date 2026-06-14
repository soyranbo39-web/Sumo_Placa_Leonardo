#ifndef ICONFIGURACION_HARDWARE_H
#define ICONFIGURACION_HARDWARE_H

class IConfiguracionHardware {
public:
    virtual ~IConfiguracionHardware() {}
    virtual void inicializarPines() const = 0;
};

#endif
