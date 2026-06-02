#pragma once

#include "kernel/include/filesystem/file_system.hpp"
#include <stdint.h>
#include <text_output.hpp>

void test_create_file()
{
    runtime::text_output.reset();
    runtime::text_output.put_string("Starte create_file-Test...\n");

    if (!kernel::filesystem::mofs.create_file("heap")) {
        runtime::text_output.put_string("Error: create_file(\"heap\")\n");
        return;
    }

    kernel::filesystem::Inode* test_inode = kernel::filesystem::mofs.get_inode_by_path("heap");

    if (test_inode == nullptr) {
        runtime::text_output.put_string("Error: get_inode_by_path(\"heap\") gives nullptr!\n");
        return;
    }

    if (test_inode->type != kernel::filesystem::Inode_Type::INODE_FILE) {
        runtime::text_output.put_string("Error: Inode-Type is not INODE_FILE!\n");
        return;
    }
}
