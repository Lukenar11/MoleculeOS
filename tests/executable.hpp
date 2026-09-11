#pragma once

#include <types.hpp>
#include <text_output.hpp>
#include <kernel.hpp>
#include <memory_manip.hpp>

namespace tests
{
    /** 
     * Warum müssen Sprünge nud Branches in i386 nur so komplex sein!?
     * 
     * Später in MoleculeOS ein richtiges "exec()" für Programme zu bauen, 
     * wird der horror.🤢
     */

    void 
    program_main()
    {
        stdlib::Text_Output::reset();
    }

    void 
    exec() 
    {
        kernel::filesys::File_Entry* file_entry;
        kernel::filesys::MoleculeOS_File_System_2::create_file(file_entry, 
                                                               "hello", 
                                                               "run", 
                                                               48);

        const 
        uint8_t* 
        program_start = reinterpret_cast<const uint8_t*>(&program_main);

        stdlib::Memory_Manipulation::copy_memory_block(file_entry->file_data_ptr,
                                                       program_start,
                                                       48);

        auto* run_program = reinterpret_cast<void(*)()>(file_entry->file_data_ptr);
        run_program();

        kernel::sys::hang();
    }
} // namespace tests
