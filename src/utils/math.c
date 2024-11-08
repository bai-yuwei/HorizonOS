#include "math.h"

uint32_t max(uint32_t x, uint32_t y) {
    return x > y ? x : y;
}

uint32_t min(uint32_t x, uint32_t y) {
    return x < y ? x : y;
}
// 这里标准和C99不太一致，猜测是因为 CPU 的原因
int32_t div(int32_t x, int32_t N) {
    if (x >= 0) return x / N;
    else return (x - N + 1) / N;
}
// 这里标准和C99不太一致，猜测是因为 CPU 的原因
int32_t mod(int32_t x, int32_t N) {
    return (x % N + N) % N;
}