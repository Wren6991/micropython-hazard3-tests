#include <unistd.h>
#include "py/mpconfig.h"

// Hardware layout definitions for Hazard3 testbench IO

#define IO_BASE 0x80000000

typedef struct {
    volatile uint32_t print_char;
    volatile uint32_t print_u32;
    volatile uint32_t exit;
    uint32_t _pad0;
    volatile uint32_t set_softirq;
    volatile uint32_t clr_softirq;
    volatile uint32_t globmon_en;
    volatile uint32_t poison_addr;
    volatile uint32_t set_irq;
    uint32_t _pad2[3];
    volatile uint32_t clr_irq;
    uint32_t _pad3[3];
} io_hw_t;

#define mm_io ((io_hw_t *const)IO_BASE)

typedef struct {
    volatile uint32_t mtime;
    volatile uint32_t mtimeh;
    volatile uint32_t mtimecmp;
    volatile uint32_t mtimecmph;
} timer_hw_t;

#define mm_timer ((timer_hw_t *const)(IO_BASE + 0x100))

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    // Currently not implemented, so just kill the simulator when this happens.
    mm_io->exit = 1234;
    return 0;
}

// Send string of given length
mp_uint_t mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    mp_uint_t ret = len;
    while (len--) {
        mm_io->print_char = *str++;
    }
    return ret;
}
