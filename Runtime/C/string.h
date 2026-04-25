#ifndef STRING_H
#define STRING_H

#include <Runtime/C/stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void* memcpy(void* __restrict dest_ptr, const void* __restrict src_ptr, uint32_t size);
void* memmove(void* dest_ptr, const void* src_ptr, uint32_t size);
void* memset(void* dest_ptr, uint8_t value, uint32_t size);
int32_t memcmp(const void* a_ptr, const void* b_ptr, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif
