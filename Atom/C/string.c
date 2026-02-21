#include "string.h"

void* memcpy(void* dest_ptr, const void* sourche_ptr, const uint32_t size) {

    uint8_t* dest = (uint8_t*)dest_ptr;
    const uint8_t* sourche = (const uint8_t*)sourche_ptr;

    for (uint32_t i = 0; i < size; i++) 
        dest[i] = sourche[i];

    return dest_ptr;
}

void* memmove(void* dest_ptr, const void* src_ptr, const uint32_t size) {

    uint8_t* dest = (uint8_t*)dest_ptr;
    const uint8_t* src = (const uint8_t*)src_ptr;

    if (dest < src) {

        // Copy forward
        for (uint32_t i = 0; i < size; i++)
            dest[i] = src[i];
    } else {

        // Copy backward (in case of overlap)
        for (uint32_t i = size; i > 0; i--)
            dest[i - 1] = src[i - 1];
    }

    return dest_ptr;
}

void* memset(void* dest_ptr, int value, const uint32_t size) {

    uint8_t* dest = (uint8_t*)dest_ptr;
    for (uint32_t i = 0; i < size; i++)
        dest[i] = (uint8_t)value;

    return dest_ptr;
}

int memcmp(const void* a_ptr, const void* b_ptr, const uint32_t size) {

    const uint8_t* a = (const uint8_t*)a_ptr;
    const uint8_t* b = (const uint8_t*)b_ptr;

    for (uint32_t i = 0; i < size; i++) 
        if (a[i] != b[i]) 
            return (int)a[i] - (int)b[i];
    return 0;
}
