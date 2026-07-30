
#ifndef Estados_H
#define Estados_H

class Motor;

 class Estados {
    public:
        enum Estado {
        BUSQUEDA,
        ATAQUE,
        PERSECUSION_ENEMIGO_45_GRADOS,
        RETROCESO,
        PERSECUSION_LATERAL
        };
        virtual void ejecutarEstrategia(Estado estadoActual, Motor& motor) = 0;
};



#endif
