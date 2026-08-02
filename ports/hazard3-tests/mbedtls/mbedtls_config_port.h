#ifndef MICROPY_INCLUDED_MBEDTLS_CONFIG_H
#define MICROPY_INCLUDED_MBEDTLS_CONFIG_H

// Bare-metal configuration: no stdlib allocation/entropy, memory hooks into
// MicroPython's tracked allocator, hardware entropy poll.
#define MICROPY_MBEDTLS_CONFIG_BARE_METAL (1)

// The hazard3 testbench has no RTC, so provide stubbed time functions
// (defined in mbedtls_port.c).
#include <time.h>
time_t hazard3_mbedtls_time(time_t *timer);
#define MBEDTLS_PLATFORM_TIME_MACRO hazard3_mbedtls_time
#define MBEDTLS_PLATFORM_MS_TIME_ALT mbedtls_ms_time

// Include common mbedtls configuration.
#include "extmod/mbedtls/mbedtls_config_common.h"

#endif /* MICROPY_INCLUDED_MBEDTLS_CONFIG_H */
