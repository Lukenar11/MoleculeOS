#pragma once

#include <stdint.h>
#include <kernel_api.hpp>

namespace tests
{
    void reallocate() noexcept {
        runtime::Text_Output::reset();
        runtime::Text_Output::put_string("=== reallocate basic test ===\n");

        // 1) allocate 16 bytes
        void* p = kernel::heap::Block_Allocator::allocate(16);
        runtime::Text_Output::put_string("Allocated 16 bytes at: ");
        runtime::Text_Output::put_ptr(reinterpret_cast<uint32_t>(p));
        runtime::Text_Output::put_char('\n');

        // 2) write test pattern
        uint8_t* b = reinterpret_cast<uint8_t*>(p);
        for (uint32_t i = 0; i < 16; ++i)
            b[i] = i;

        // 3) reallocate to 64 bytes
        void* p2 = kernel::heap::Block_Allocator::reallocate(p, 64);
        runtime::Text_Output::put_string("Reallocated to 64 bytes at: ");
        runtime::Text_Output::put_ptr(reinterpret_cast<uint32_t>(p2));
        runtime::Text_Output::put_char('\n');

        // 4) verify copy
        uint8_t* b2 = reinterpret_cast<uint8_t*>(p2);
        bool ok = true;
        for (uint32_t i = 0; i < 16; ++i)
            if (b2[i] != i)
                ok = false;

        runtime::Text_Output::put_string(ok ? "Copy OK\n" : "Copy FAILED\n");

        // 5) shrink to 8 bytes
        void* p3 = kernel::heap::Block_Allocator::reallocate(p2, 8);
        runtime::Text_Output::put_string("Shrink to 8 bytes at: ");
        runtime::Text_Output::put_ptr(reinterpret_cast<uint32_t>(p3));
        runtime::Text_Output::put_char('\n');

        // 6) free
        kernel::heap::Block_Allocator::deallocate(p3);
        runtime::Text_Output::put_string("Freed final block\n");

        kernel::sys::hang();
    }

    void reallocate_special() noexcept {
        runtime::Text_Output::reset();
        runtime::Text_Output::put_string("=== reallocate special cases ===\n");

        // realloc(nullptr, size) → allocate(size)
        void* p = kernel::heap::Block_Allocator::reallocate(nullptr, 32);
        runtime::Text_Output::put_string("realloc(nullptr, 32) → ");
        runtime::Text_Output::put_ptr(reinterpret_cast<uint32_t>(p));
        runtime::Text_Output::put_char('\n');

        // realloc(ptr, 0) → free(ptr)
        void* p2 = kernel::heap::Block_Allocator::reallocate(p, 0);
        runtime::Text_Output::put_string("realloc(ptr, 0) → ");
        runtime::Text_Output::put_ptr(reinterpret_cast<uint32_t>(p2));
        runtime::Text_Output::put_char('\n');

        kernel::sys::hang();
    }

    void deallocate() noexcept {
        runtime::Text_Output::reset();
        runtime::Text_Output::put_string("=== deallocate checks ===\n");

        // 1) normal free
        void* p = kernel::heap::Block_Allocator::allocate(32);
        kernel::heap::Block_Allocator::deallocate(p);
        runtime::Text_Output::put_string("Normal free OK\n");

        // 2) double free
        // kernel::heap::Block_Allocator::deallocate(p);

        // 3) misaligned pointer
        // uint8_t* bad = reinterpret_cast<uint8_t*>(p) + 1;
        // kernel::heap::Block_Allocator::deallocate(bad);

        // 4) pointer outside the heap
        // kernel::heap::Block_Allocator::deallocate(reinterpret_cast<void*>(0x12345678));

        kernel::sys::hang();
    }

    void get_allocation_info() noexcept {
        runtime::Text_Output::reset();
        runtime::Text_Output::put_string("=== get_allocation_info test ===\n");

        uint32_t index = 0;
        uint32_t blocks = 0;

        // 1) allocate 32 bytes → should be 3 blocks (with a block size of 16 bytes)
        void* p = kernel::heap::Block_Allocator::allocate(40);
        runtime::Text_Output::put_string("Allocated pointer: ");
        runtime::Text_Output::put_ptr(reinterpret_cast<uint32_t>(p));
        runtime::Text_Output::put_char('\n');

        bool ok = kernel::heap::Block_Allocator::get_allocation_info(p, index, blocks);

        runtime::Text_Output::put_string("Valid pointer: ");
        runtime::Text_Output::put_string(ok ? "yes\n" : "no\n");

        runtime::Text_Output::put_string("Block index: ");
        runtime::Text_Output::put_uint(index);
        runtime::Text_Output::put_char('\n');

        runtime::Text_Output::put_string("Block count: ");
        runtime::Text_Output::put_uint(blocks);
        runtime::Text_Output::put_char('\n');

        // 2) misaligned pointer
        uint8_t* misaligned = reinterpret_cast<uint8_t*>(p) + 1;
        ok = kernel::heap::Block_Allocator::get_allocation_info(misaligned, index, blocks);

        runtime::Text_Output::put_string("Misaligned pointer valid: ");
        runtime::Text_Output::put_string(ok ? "yes\n" : "no\n");

        // 3) pointer outside the heap
        void* outside = reinterpret_cast<void*>(0x12345678);
        ok = kernel::heap::Block_Allocator::get_allocation_info(outside, index, blocks);

        runtime::Text_Output::put_string("Outside pointer valid: ");
        runtime::Text_Output::put_string(ok ? "yes\n" : "no\n");

        // 4) free block → should be invalid
        kernel::heap::Block_Allocator::deallocate(p);
        ok = kernel::heap::Block_Allocator::get_allocation_info(p, index, blocks);

        runtime::Text_Output::put_string("Freed pointer valid: ");
        runtime::Text_Output::put_string(ok ? "yes\n" : "no\n");

        kernel::sys::hang();
    }
} // namespace tests
