#ifndef MOTOR_H
#define MOTOR_H

#include "Pines.H"
#include "IMotor.H"

class Motor: public IMotor {
public:
 
    void Adelante(int velocidad) override;
    void Atras(int velocidad) override;
    void GirarIzquierda(int velocidad) override;
    void GirarDerecha(int velocidad) override;
    void Detener() override;
    void seguirEnemigo(int velocidad) override;
};

#endif
