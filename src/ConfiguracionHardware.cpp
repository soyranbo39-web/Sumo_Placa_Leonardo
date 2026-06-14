#include <Arduino.h>
#include "ConfiguracionHardware.H"
#include "Pines.H"

void ConfiguracionHardware::inicializarPines() const {
    pinMode(S_FRONT_IZQ, INPUT);
    pinMode(S_FRONT_CEN, INPUT);
    pinMode(S_FRONT_DER, INPUT);
    pinMode(S_LAT_IZQ, INPUT);
    pinMode(S_LAT_DER, INPUT);

    pinMode(PWMA, OUTPUT);
    pinMode(MA1A, OUTPUT);
    pinMode(MA2A, OUTPUT);
    pinMode(PWMB, OUTPUT);
    pinMode(MA1B, OUTPUT);
    pinMode(MA2B, OUTPUT);
}
