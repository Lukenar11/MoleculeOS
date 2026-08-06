#pragma once 

#include "helpers.hpp"
#include <types.h>
#include <drivers_api.hpp>
#include <memory_manip.hpp>

namespace kernel::storagmgr
{
    class Storage_Manager final {
    private:
        static bool read_or_write_bytes(drivers::ata::Driver_Operations op,
                                        const uint32_t offset, 
                                        const uint32_t size, 
                                        void* buffer) noexcept;

    public:
        static void init() noexcept;

        static bool save_filesys() noexcept;
        static bool load_filesys() noexcept;

        Storage_Manager() noexcept  = default;
        ~Storage_Manager() noexcept = default;
    };
} // namespace kernel::storagmgr
