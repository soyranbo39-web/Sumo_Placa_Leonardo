#include <Arduino.h>
#include "ConfiguracionHardware.H"
#include "Pines.H"

void ConfiguracionHardware::inicializarPines() const {
    // Sensores de enemigo
    pinMode(S_FRONT_IZQ, INPUT);
    pinMode(S_FRONT_CEN, INPUT);
    pinMode(S_FRONT_DER, INPUT);
    pinMode(S_LAT_IZQ, INPUT);
    pinMode(S_LAT_DER, INPUT);

    // Sensores de piso (Seguridad de inicialización)
    pinMode(S_PISO_IZQ, INPUT);
    pinMode(S_PISO_DER, INPUT);

    // Pines de control de Motores
    pinMode(PWMA, OUTPUT);
    pinMode(MA1A, OUTPUT);
    pinMode(MA2A, OUTPUT);
    pinMode(PWMB, OUTPUT);
    pinMode(MA1B, OUTPUT);
    pinMode(MA2B, OUTPUT);

    // ESTADO INICIAL SEGURO: Fuerza 0V en los motores al encender
    digitalWrite(PWMA, LOW);
    digitalWrite(MA1A, LOW);
    digitalWrite(MA2A, LOW);
    digitalWrite(PWMB, LOW);
    digitalWrite(MA1B, LOW);
    digitalWrite(MA2B, LOW);
}