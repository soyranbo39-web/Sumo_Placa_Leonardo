#ifndef MOTOR_H
#define MOTOR_H

#include "Pines.H"
#include "IMotor.H"

class Motor: public IMotor {
public:
    void avanzar(int velocidad) override;
    void retroceder(int velocidad) override;
    void detener() override;
    void girar(int velocidad) override;
    void curva(int velocidad) override;
    void mover(int velIzq, int velDer) override;
};

#endif
