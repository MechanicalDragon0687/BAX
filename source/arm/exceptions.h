#pragma once

#include <common.h>

#define XRQ_RESET          (0)
#define XRQ_UNDEFINED      (1)
#define XRQ_SWI            (2)
#define XRQ_PREFETCH_ABORT (3)
#define XRQ_DATA_ABORT     (4)
#define XRQ_RESERVED       (5)
#define XRQ_IRQ            (6)
#define XRQ_FIQ            (7)


void install_exception_vectors(void);

extern void xrq_reset(void);
extern void xrq_undefined(void);
extern void xrq_swi(void);
extern void xrq_prefetch_abort(void);
extern void xrq_data_abort(void);
extern void xrq_reserved(void);
extern void xrq_irq(void);
extern void xrq_fiq(void);
