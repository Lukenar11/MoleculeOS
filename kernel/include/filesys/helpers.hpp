/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains all meta-informations and 
    the file structure about the file sys.

NOTES:
    Each file-structure represents a file with meta infos, the file contains the
    file name, format, the data, the size and a using info.

    An array of file instances is created from the file structure,
    which then represents all the files.
*/

#pragma once

#include <stdint.h>

namespace kernel::filesys
{
    constexpr uint32_t MAX_FILENAME_LENGTH         = 128;
    constexpr uint32_t MAX_FILE_FORMAT_NAME_LENGTH = 4;
    constexpr uint32_t MAX_FILE_SIZE               = 2048;
    constexpr uint32_t MAX_FILES_PER_DIRECTORY     = 512;

    struct File final {
        char name[MAX_FILENAME_LENGTH]           = {'\0'};
        char format[MAX_FILE_FORMAT_NAME_LENGTH] = {'\0'};
        bool in_use                              = false;
        uint32_t size                            = 0;
        uint8_t data[MAX_FILE_SIZE]              = {};
    };
} // namespace kernel::filesys
