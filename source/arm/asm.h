#pragma once

#include <stdbool.h>
#include <stdint.h>

void flush_all_caches(void);
void enable_irqs(void);
void disable_irqs(void);
void use_low_exception_vectors(void);
void use_high_exception_vectors(void);
uint32_t get_bootenv(void);
void wfi(void);

void chainload(void *payload, size_t len);

uint32_t ctr_rand(void);
