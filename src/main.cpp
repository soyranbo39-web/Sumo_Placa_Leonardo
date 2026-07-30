#include <Arduino.h>
#include "Motor.H"
#include "Estados.H"
#include "ConfiguracionHardware.H"
#include "Percepcion.H"
#include "EstrategiaCombate.H"
#include "ControlMovimiento.H"

Motor motor;
Percepcion percepcion;
EstrategiaCombate estrategia;
ControlMovimiento controlMovimiento;
Estado estado(percepcion, estrategia, controlMovimiento);
ConfiguracionHardware hardware;

void setup() {
  hardware.inicializarPines();
  estado.setMotor(&motor);


}

void loop() {
  estado.actualizarEstado();
}