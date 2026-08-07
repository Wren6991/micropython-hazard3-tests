include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h
FROZEN_MANIFEST ?= manifest.py

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1

# Enable mbed-tls based ssl module
MICROPY_PY_SSL ?= 1
MICROPY_SSL_MBEDTLS ?= 1

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

CROSS_COMPILE ?= /opt/riscv/gcc16/bin/riscv32-unknown-elf-

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

# -fdisable-rtl-popretz is due to a GCC bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=126454
CFLAGS_MARCH = -march=rv32imab_zicsr_zifencei_zbkb_zca_zcb_zcmp_xh3bextm
CFLAGS += $(INC) -Wall -Werror -std=c99 -nostdlib $(CFLAGS_MARCH) $(COPT)
LDFLAGS += -nostdlib -T hazard3_tb.ld -Map=$@.map --cref --gc-sections

CSUPEROPT = -Os # save some code space

# Tune for Debugging or Optimization
CFLAGS += -g  # always include debug info in the ELF
ifeq ($(DEBUG), 1)
CFLAGS += -Og
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

# Flags for optional C++ source code
CXXFLAGS += $(filter-out -std=c99,$(CFLAGS))

LIBS = $(shell $(CC) -print-file-name=libm.a) $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

SRC_C = \
	main.c \
	uart_core.c \
	mphalport.c \
	reent_stub.c \
	mbedtls/mbedtls_port.c \
	shared/libc/printf.c \
	shared/readline/readline.c \
	shared/runtime/pyexec.c \
	shared/runtime/stdout_helpers.c \
	shared/runtime/sys_stdio_mphal.c \

SRC_C += shared/libc/string0.c

SRC_O += init.o

SRC_QSTR += shared/readline/readline.c shared/runtime/pyexec.c
SRC_QSTR += shared/runtime/sys_stdio_mphal.c

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_O))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))

all: $(BUILD)/firmware.bin $(BUILD)/firmware.dis

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary -j .isr_vector -j .text -j .data $^ $(BUILD)/firmware.bin

$(BUILD)/firmware.dis: $(BUILD)/firmware.elf
	$(Q)$(CROSS_COMPILE)objdump -h $^ | expand > $@
	$(Q)$(CROSS_COMPILE)objdump -d $^ | expand >> $@

include $(TOP)/py/mkrules.mk
