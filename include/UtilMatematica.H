#ifndef UTIL_MATEMATICA_H
#define UTIL_MATEMATICA_H

#include <stdint.h>

template <typename T>
constexpr T limitar(T valor, T minimo, T maximo) {
    return (valor < minimo) ? minimo : ((valor > maximo) ? maximo : valor);
}

constexpr int16_t valorAbsoluto(int16_t valor) {
    return (valor < 0) ? static_cast<int16_t>(-valor) : valor;
}

constexpr int16_t promedioEntero(int16_t acumulado, int16_t cantidad) {
    return (cantidad == 0) ? 0 : static_cast<int16_t>(acumulado / cantidad);
}

#endif
