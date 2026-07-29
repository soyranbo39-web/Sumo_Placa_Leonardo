#include <Arduino.h>
#include "Motor.H"

void Motor::mover(int velIzq, int velDer) {
	// Control motor izquierdo (A)
	if (velIzq >= 0) {
		digitalWrite(MA1A, HIGH);
		digitalWrite(MA2A, LOW);
		analogWrite(PWMA, velIzq);
	} else {
		digitalWrite(MA1A, LOW);
		digitalWrite(MA2A, HIGH);
		analogWrite(PWMA, abs(velIzq));
	}

	// Control motor derecho (B)
	if (velDer >= 0) {
		digitalWrite(MA1B, HIGH);
		digitalWrite(MA2B, LOW);
		analogWrite(PWMB, velDer);
	} else {
		digitalWrite(MA1B, LOW);
		digitalWrite(MA2B, HIGH);
		analogWrite(PWMB, abs(velDer));
	}
}

void Motor::avanzar(int velocidad) {
	mover(velocidad, velocidad);
}

void Motor::retroceder(int velocidad) {
	mover(-velocidad, -velocidad);
}

void Motor::detener() {
	mover(0, 0);
}

void Motor::girar(int velocidad) {
	mover(velocidad, -velocidad);
}

void Motor::curva(int velocidad) {
	mover(velocidad / 2, velocidad);
}
