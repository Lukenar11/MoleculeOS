#pragma once

#include <Runtime/C/stdint.h>

struct HeapBlock {

    uint32_t size;
    bool is_free;
    HeapBlock* next;
};
