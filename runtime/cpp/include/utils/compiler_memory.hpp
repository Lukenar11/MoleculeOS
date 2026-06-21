#include <memory_manipulation.hpp>

extern "C"
{
    void* memcpy(void* dest_ptr,
                 const void* src_ptr,
                 uint32_t size);
                 
    void* memmove(void* dest_ptr,
                  const void* src_ptr,
                  uint32_t size);

    void* memset(void* dest_ptr,
                 int32_t value,
                 uint32_t size);

    int32_t memcmp(const void* a_ptr,
                   const void* b_ptr,
                   uint32_t size);
}