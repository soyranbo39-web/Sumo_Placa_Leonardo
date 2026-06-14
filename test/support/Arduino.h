#ifndef ARDUINO_H
#define ARDUINO_H

#include <stdint.h>

#ifndef HIGH
#define HIGH 0x1
#endif

#ifndef LOW
#define LOW 0x0
#endif

#ifndef INPUT
#define INPUT 0x0
#endif

#ifndef OUTPUT
#define OUTPUT 0x1
#endif

#ifndef A1
#define A1 15
#endif

#ifndef A2
#define A2 16
#endif

#ifndef A4
#define A4 18
#endif

#ifndef A5
#define A5 19
#endif

#ifdef __cplusplus
extern "C" {
#endif

void delay(unsigned long ms);
int analogRead(uint8_t pin);
int digitalRead(uint8_t pin);
void digitalWrite(uint8_t pin, uint8_t value);
void analogWrite(uint8_t pin, int value);
void pinMode(uint8_t pin, uint8_t mode);

extern int g_delayCallCount;
extern unsigned long g_delayValues[16];

#ifdef __cplusplus
}
#endif

#endif
