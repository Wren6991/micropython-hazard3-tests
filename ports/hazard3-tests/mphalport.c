#include "py/mphal.h"
#include "tbio_regs.h"
#include <limits.h>

// MTIME/MTIMEH is assumed to be a microsecond timebase

mp_uint_t mp_hal_ticks_us(void) {
    return mm_timer->mtime;
}

void mp_hal_delay_us(mp_uint_t us) {
    mp_uint_t starttime = mm_timer->mtime;
    while ((mm_timer->mtime - starttime) < us)
        ;
}

void mp_hal_delay_ms(mp_uint_t ms) {
    const mp_uint_t max_per_wait = UINT_MAX / 1000u;
    while (ms > 0) {
        mp_uint_t chunk = ms > max_per_wait ? max_per_wait : ms;
        mp_hal_delay_us(1000u * chunk);
        ms -= chunk;
    }
}

mp_uint_t mp_hal_ticks_cpu(void) {
    mp_uint_t x;
    __asm__ volatile ("csrr %0, mcycle\n" : "=r" (x));
    return x;
}

uint64_t mp_hal_time_ns(void) {
    return 1000ull * mm_timer->mtime;
}
