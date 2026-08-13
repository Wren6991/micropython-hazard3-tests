#include <stdint.h>
#include <alloca.h>
#include <limits.h>

#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_EVERYTHING)

#define MICROPY_PY_OS_UNAME     (1)
#define MICROPY_PY_SYS_PLATFORM "hazard3"

// EVERYTHING turns on marshal, whose loads() needs the persistent-code loader
#define MICROPY_PERSISTENT_CODE_LOAD      (1)

// Enable soft-float via libgcc routines
#define MICROPY_FLOAT_IMPL (MICROPY_FLOAT_IMPL_FLOAT)

#define MICROPY_ENABLE_COMPILER           (1)

#define MICROPY_ENABLE_GC                 (1)

// Enable the mbed-tls backed ssl module
#define MICROPY_TRACKED_ALLOC            (1)
#define MICROPY_HELPER_REPL               (1)
#define MICROPY_ENABLE_EXTERNAL_IMPORT    (1)

#define MICROPY_HELPER_REPL               (1)
#define MICROPY_REPL_INFO                 (1)

#define MICROPY_ALLOC_PATH_MAX            (256)

// Use the minimum headroom in the chunk allocator for parse nodes.
#define MICROPY_ALLOC_PARSE_CHUNK_INIT    (16)

// Get the constants available in REPL -- note the cryptolib enable is in the
// Makefile, following the pattern from other ports:
#define MICROPY_PY_CRYPTOLIB_CONSTS (1)

#define MICROPY_PY_SYS_STDFILES (1)
#define MICROPY_PY_UCTYPES (0)

// todo remove -- think I just need to initialise the stack limit in my main.c
#define MICROPY_STACK_CHECK (0)

#define MICROPY_PY_MATH_GAMMA_FIX_NEGINF (1)

#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF (1)

#define MICROPY_LONGINT_IMPL (MICROPY_LONGINT_IMPL_MPZ)

// type definitions for the specific machine

typedef intptr_t mp_int_t; // must be pointer size
typedef uintptr_t mp_uint_t; // must be pointer size
typedef long mp_off_t;

#define MICROPY_HW_BOARD_NAME "hazard3-testbench"
#define MICROPY_HW_MCU_NAME "Hazard3"

#define MICROPY_HEAP_SIZE      (256 * 1024)

#define MP_STATE_PORT MP_STATE_VM

#if defined(SIZE_MAX) && !defined(SSIZE_MAX)
// Hack; this is not in the limits.h that ships with riscv-gnu-toolchain
#define SSIZE_MAX (SIZE_MAX >> 1)
#endif
