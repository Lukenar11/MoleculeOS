#include "Heap/Heap.hpp"

namespace kernel::heap {

    Heap::Heap() noexcept {

        const auto start = reinterpret_cast<uintptr_t>(&heap_start);
        const auto end = reinterpret_cast<uintptr_t>(&heap_end);

        first_block = reinterpret_cast<HeapBlock*>(start);
        first_block->size = (end - start) - sizeof(HeapBlock);
        first_block->is_free = true;
        first_block->next = nullptr;
    }

    void* Heap::allocate(const uint32_t size) noexcept {

        HeapBlock* current_block = first_block;
        while (current_block) {

            if (current_block->is_free && current_block->size >= size) {

                current_block->is_free = false;
                return reinterpret_cast<void*>(
                    reinterpret_cast<uint8_t*>(current_block) + sizeof(HeapBlock)
                );
            }
            
            current_block = current_block->next;
        }

        // no place (heap full)
        return nullptr;
    }

    void Heap::deallocate(void* block_ptr) noexcept { 

        if (!block_ptr) 
            return;

        HeapBlock* current_block = reinterpret_cast<HeapBlock*>(
            reinterpret_cast<uint8_t*>(block_ptr) - sizeof(HeapBlock)
        );
        
        current_block->is_free = true;
    }

    Heap heap;
} // namespace kernel::heap
