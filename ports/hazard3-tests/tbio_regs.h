#ifndef _TBIO_REGS_H
#define _TBIO_REGS_H

// Hardware layout definitions for Hazard3 testbench IO

#define IO_BASE 0xc0000000

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

#endif
