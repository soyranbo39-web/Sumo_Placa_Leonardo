#ifndef PINES_H
#define PINES_H

#define S_PISO_IZQ   A1
#define S_PISO_DER   A2
#define BLANCO 50


#define S_FRONT_IZQ  2
#define S_FRONT_CEN  4
#define S_FRONT_DER  A5
// El pin 1 es TX, usarlo para un sensor puede causar problemas si se usa el puerto serial. Se cambia a 0 (RX).
#define S_LAT_IZQ    0
#define S_LAT_DER    A4


#define PWMA 10
#define MA1A 9
#define MA2A 13
#define PWMB 11
#define MA1B 8
#define MA2B 12



#define VelocidadMaxima 255
#define VelocidadMinima 50
#define VelocidadCurva 150
#define VelocidadGiro 200
#define VelocidadRetroceso 255
#define VelocidadAvance 160
#define VelocidadAtaqueFrontal 255
#define VelocidadBusquedaDer 90
#define VelocidadPivoteLateral 220
#define VelocidadCurvaAtaqueLateral 100




#endif