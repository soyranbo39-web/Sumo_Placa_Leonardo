
#ifndef IMOTOR_H
#define IMOTOR_H

class IMotor {
public:
    virtual void avanzar(int velocidad) = 0;
    virtual void retroceder(int velocidad) = 0;
    virtual void detener() = 0;
    virtual void girar(int velocidad) = 0;
    virtual void curva(int velocidad) = 0;
    virtual void mover(int velIzq, int velDer) = 0;
    
};

#endif