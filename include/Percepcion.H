#ifndef PERCEPCION_H
#define PERCEPCION_H

#include "LecturasSensores.H"
#include "IPercepcion.H"

class Percepcion: public IPercepcion {
public:
    LecturasSensores leer() const override;
};

#endif
