#include "string.h"

void* memcpy(void* __restrict dest_ptr, const void* __restrict src_ptr, uint32_t size) {

    uint8_t* dest_base = (uint8_t*)dest_ptr;
    const uint8_t* src = (const uint8_t*)src_ptr;

    uint8_t* dest = dest_base;

    // 1) 4-byte bulk copy
    while (size >= 4) {

        *(uint32_t*)dest = *(const uint32_t*)src;
        dest += 4;
        src  += 4;
        size -= 4;
    }

    // 2) remaining bytes
    while (size--)
        *dest++ = *src++;

    return dest_base;
}

void* memmove(void* dest_ptr, const void* src_ptr, uint32_t size) {

    uint8_t* dest_base = (uint8_t*)dest_ptr;
    const uint8_t* src_base = (const uint8_t*)src_ptr;

    uint8_t* dest = dest_base;
    const uint8_t* src = src_base;

    if (dest_ptr < src_ptr) {

        // forward copy
        while (size >= 4) {

            *(uint32_t*)dest = *(const uint32_t*)src;
            dest += 4;
            src  += 4;
            size -= 4;
        }

        while (size--)
            *dest++ = *src++;

    } else {

        // backward copy
        dest += size;
        src  += size;

        while (size >= 4) {

            dest -= 4;
            src  -= 4;
            *(uint32_t*)dest = *(const uint32_t*)src;
            size -= 4;
        }

        while (size--)
            *--dest = *--src;
    }

    return dest_base;
}

void* memset(void* dest_ptr, uint8_t value, uint32_t size) {

    uint8_t* dest_base = (uint8_t*)dest_ptr;
    uint8_t* dest = dest_base;

    // build 32-bit pattern
    uint32_t pattern = (uint32_t)value;
    pattern |= pattern << 8;
    pattern |= pattern << 16;

    while (size >= 4) {

        *(uint32_t*)dest = pattern;
        dest += 4;
        size -= 4;
    }

    while (size--)
        *dest++ = value;

    return dest_base;
}

int32_t memcmp(const void* a_ptr, const void* b_ptr, const uint32_t size) {

    const uint8_t* a = (const uint8_t*)a_ptr;
    const uint8_t* b = (const uint8_t*)b_ptr;

    for (uint32_t i = 0; i < size; i++)
        if (a[i] != b[i])
            return (int32_t)a[i] - (int32_t)b[i];

    return 0;
}
