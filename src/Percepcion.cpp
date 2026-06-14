#include <Arduino.h>
#include "Percepcion.H"
#include "Pines.H"

LecturasSensores Percepcion::leer() const {
    LecturasSensores lecturas;
    lecturas.lineaIzq = (analogRead(S_PISO_IZQ) < BLANCO);
    lecturas.lineaDer = (analogRead(S_PISO_DER) < BLANCO);
    lecturas.latIzq = digitalRead(S_LAT_IZQ);
    lecturas.c45Izq = digitalRead(S_FRONT_IZQ);
    lecturas.frontal = digitalRead(S_FRONT_CEN);
    lecturas.c45Der = digitalRead(S_FRONT_DER);
    lecturas.latDer = digitalRead(S_LAT_DER);
    return lecturas;
}
