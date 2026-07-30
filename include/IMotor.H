
#ifndef IMOTOR_H
#define IMOTOR_H

class IMotor {
public:

virtual void Adelante(int velocidad) = 0;
virtual void Atras(int velocidad) = 0;
virtual void GirarIzquierda(int velocidad) = 0;
virtual void GirarDerecha(int velocidad) = 0;
virtual void Detener() = 0;
virtual void seguirEnemigo(int velocidad) = 0;

    
};

#endif