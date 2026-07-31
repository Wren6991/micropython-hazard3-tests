#include <unistd.h>
#include "py/mpconfig.h"

#include "tbio_regs.h"

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    return mm_io->print_char;
}

// Send string of given length
mp_uint_t mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    mp_uint_t ret = len;
    while (len--) {
        mm_io->print_char = *str++;
    }
    return ret;
}
