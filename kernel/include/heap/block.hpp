/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This class defines the entire system heap using a 'block allocator'.
    The allocator uses linker-defined symbols (heap_start & heap_end)
    to determine the valid heap area.

NOTES:
    Some methods are placed in the header 
    because they are so small that the compiler can inline them.
*/

#pragma once

#include "heap/helpers.hpp"
#include <types.hpp>
#include <sal.hpp>
#include <kernel.hpp>
#include <memory_manip.hpp>
#include <status.hpp>

namespace kernel::heap
{
    class Block_Allocator final {
    private:
        static inline constexpr uint32_t ALL_BLOCKS_FREE   = 0xFFFF'FFFF;
        static inline constexpr uint32_t MEMORY_CLEAR      = 0;
        static inline constexpr uint32_t MEMORY_BLOCK_SIZE = 16;

        static inline uint32_t all_memory_blocks = 0;
        static inline uint32_t needed_pool_space = 0;

        static inline uint8_t* memory_pool_ptr   = nullptr;
        static inline uint16_t* allocation_sizes = nullptr;
        static inline uint32_t* memory_bitmap    = nullptr;

        static inline
        void set_block_used(_IN_ const uint32_t i) noexcept {
            memory_bitmap[i >> 5] &= ~(1u << (i & 31));
        }

        static inline
        void set_block_free(_IN_ const uint32_t i) noexcept {
            memory_bitmap[i >> 5] |= (1u << (i & 31));
        }

        static inline
        bool is_block_free(_IN_ const uint32_t i) noexcept {
            return memory_bitmap[i >> 5] & (1u << (i & 31));
        }

        static
        void compute_block_count(_IN_ const uint32_t start_addr,
                                 _IN_ const uint32_t end_addr) noexcept;

        static
        void setup_metadata_pointers(_IN_ const uint32_t start_addr) noexcept;

        static
        void setup_memory_pool(_IN_ const uint32_t start_addr,
                               _IN_ const uint32_t end_addr,
                               _IN_ const uint32_t word_count) noexcept;

        static
        void clear_metadata(_IN_ const uint32_t word_count) noexcept;

        static 
        void* set_allocation_sizes_entry(_IN_ const uint32_t blocks_needed, 
                                         _IN_ const uint32_t i) noexcept;

        static 
        bool find_enough_free_memory_blocks(_INOUT_ uint32_t& j,
                                            _IN_    const uint32_t i, 
                                            _IN_    const uint32_t blocks_needed)
                                            noexcept;

        static
        status_t validate_allocate_size(_IN_ const uint32_t size,
                                        _OUT_ uint32_t& blocks_needed) 
                                        noexcept;

        static
        status_t find_free_memory_region(_OUT_ uint32_t& index,
                                         _IN_  const uint32_t blocks_needed) 
                                         noexcept;

        static
        status_t perform_reallocate(_INOUT_ void*& ptr,
                                    _IN_    const uint32_t new_size) 
                                    noexcept;

    public:
        static 
        void init(_IN_ const uint8_t* begin, 
                  _IN_ const uint8_t* end) noexcept;

        static 
        status_t get_allocation_info(_OUT_ uint32_t& index, 
                                     _OUT_ uint32_t& blocks,
                                     _IN_  void* ptr) noexcept;

        static 
        status_t allocate(_OUT_ void*& ptr, 
                          _IN_  const uint32_t size) noexcept;

        static
        status_t clear_allocate(_OUT_ void*& ptr,
                                _IN_  const uint32_t size) noexcept;

        static 
        status_t reallocate(_INOUT_ void*& ptr, 
                            _IN_    const uint32_t new_size) noexcept;

        static 
        status_t deallocate(_IN_ void* ptr) noexcept;

        Block_Allocator() noexcept  = default;
        ~Block_Allocator() noexcept = default;
    };
} // namespace kernel::heap
