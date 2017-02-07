#pragma once

#include <stdbool.h>
#include <stdint.h>

void flush_all_caches(void);
void use_low_exception_vectors(void);
void use_high_exception_vectors(void);
u32 get_bootenv(void);
void wfi(void);

void chainload(void *payload, size_t len);
u32 ctr_rand(void);
