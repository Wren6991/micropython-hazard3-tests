#include "py/mphal.h"
#include "py/stream.h"
#include "tbio_regs.h"
#include <limits.h>

// MTIME/MTIMEH is assumed to be a microsecond timebase

mp_uint_t mp_hal_ticks_ms(void) {
    uint32_t h0, l, h1;
    do {
        h0 = mm_timer->mtimeh;
        l = mm_timer->mtime;
        h1 = mm_timer->mtimeh;
    } while (h0 != h1);
    uint64_t us = ((uint64_t)h1 << 32) | (uint64_t)l;
    return us / 1000ull;
}

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

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    (void)poll_flags;
    return 0;
}

// Random source for the mbed-tls entropy pool. No libc in this port so use a
// small LCG; running in simulation so deterministic behaviour is expected.
static uint32_t rng_state = 0x12345678u;

void mp_hal_get_random(size_t n, uint8_t *buf) {
    for (size_t i = 0; i < n; i++) {
        rng_state = rng_state * 1103515245u + 12345u;
        buf[i] = (uint8_t)(rng_state >> 16);
    }
}
