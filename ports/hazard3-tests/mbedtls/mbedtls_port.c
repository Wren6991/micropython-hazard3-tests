#include <time.h>

#include "mbedtls_config_port.h"
#include "mbedtls/platform_time.h"

// The hazard3 testbench has no real time clock, so stub the mbed-tls time
// functions used for certificate validity checking / DTLS.
time_t hazard3_mbedtls_time(time_t *timer) {
    if (timer != NULL) {
        *timer = 0;
    }
    return 0;
}

mbedtls_ms_time_t mbedtls_ms_time(void) {
    return 0;
}

// The testbench has no clock so return a fixed (zero) broken-down time.
struct tm *gmtime(const time_t *timer) {
    (void)timer;
    static struct tm tm = {0};
    return &tm;
}
