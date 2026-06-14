#ifndef IPERCEPCION_H
#define IPERCEPCION_H

#include "LecturasSensores.H"

class IPercepcion {
public:
    virtual ~IPercepcion() {}
    virtual LecturasSensores leer() const = 0;
};

#endif
