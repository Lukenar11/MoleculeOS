#pragma once

#include "kernel/include/filesystem/file_system.hpp"
#include <stdint.h>
#include <text_output.hpp>

void test_file_system()
{
    runtime::text_output.reset();
    runtime::text_output.put_string("Testing the file system...\n");
}
