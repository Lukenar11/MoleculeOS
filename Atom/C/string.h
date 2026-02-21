#ifndef STRING_H
#define STRING_H

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

    void* memcpy(void* dest_ptr, const void* sourche_ptr, const uint32_t size);
    void* memmove(void* dest_ptr, const void* sourche_ptr, const uint32_t size);
    void* memset(void* dest_ptr, int value, const uint32_t size);
    int memcmp(const void* a_ptr, const void* b_ptr, const uint32_t size);
#ifdef __cplusplus
    }
#endif

#endif  // STRING_H
