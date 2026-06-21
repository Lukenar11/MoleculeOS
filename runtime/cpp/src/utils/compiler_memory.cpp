#include "utils/compiler_memory.hpp"

extern "C"
{
    void* memcpy(void* dest_ptr,
                 const void* src_ptr,
                 uint32_t size) {
        return runtime::memory_manip.copy_memory_block(
            dest_ptr,
            src_ptr,
            size
        );
    }

    void* memmove(void* dest_ptr,
                  const void* src_ptr,
                  uint32_t size) {
        return runtime::memory_manip.move_memory_block(
            dest_ptr,
            src_ptr,
            size
        );
    }

    void* memset(void* dest_ptr,
                 int32_t value,
                 uint32_t size) {
        return runtime::memory_manip.set_memory_block(
            dest_ptr,
            value,
            size
        );
    }

    int32_t memcmp(const void* a_ptr,
                   const void* b_ptr,
                   uint32_t size) {
        return runtime::memory_manip.compare_memory_block(
            a_ptr,
            b_ptr,
            size
        );
    }
}
