/** 
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains a few helper functions 
    that are used by the shell command implementations.

NOTES:
    Since some functions are quite small, they are intentionally placed in the 
    header so that the compiler can inline them more easily.
*/

#include "utils/cmds_helpers.hpp"

namespace shell::commands
{
    Parsed_File_Name& parse_filename(const runtime::Array<char, 64>& args) noexcept {
        static Parsed_File_Name parsed_filename;

        parsed_filename.name.fill('\0');
        parsed_filename.format.fill('\0');
        parsed_filename.error.fill('\0');

        uint32_t file_name_index   = 0;
        uint32_t file_format_index = 0;
        bool is_file_name          = true;

        if (args[0] == '\0') [[unlikely]] {
            static const char* error_message = "missing argument";
            runtime::String_Manipulation::copy_string(parsed_filename.error.data(), 
                                                      error_message);
            return parsed_filename;
        }
    
        for (uint32_t i = 0; args[i] != '\0'; i++) [[likely]] {
            if (args[i] == '.') {
                is_file_name = false;
                continue;
            }

            if (args[i] == '\0') [[unlikely]]
                break;

            if ((is_file_name && !append_char(parsed_filename.name, 
                                              file_name_index, 
                                              args[i])) ||
                (file_name_index >= kernel::filesys::MAX_FILE_NAME_LENGTH)) {
                static const char* error_message = "filename too long";
                runtime::String_Manipulation::copy_string(parsed_filename.error.data(),
                                                          error_message);

                return parsed_filename;
            }
            
            if (!(is_file_name || append_char(parsed_filename.format, 
                                              file_format_index, 
                                              args[i])) || 
                (file_format_index >= kernel::filesys::MAX_FILE_FORMAT_LENGTH)) {
                static const char* error_message = "format too long";
                runtime::String_Manipulation::copy_string(parsed_filename.error.data(), 
                                                          error_message);
                return parsed_filename;
            }
        }

        parsed_filename.name[file_name_index]     = '\0';
        parsed_filename.format[file_format_index] = '\0';

        if (parsed_filename.name[0] == '\0') {
            static const char* error_message = "no File Name entered";
            runtime::String_Manipulation::copy_string(parsed_filename.error.data(),
                                                      error_message);
            return parsed_filename;
        }

        if (parsed_filename.format[0] == '\0') {
            static const char* error_message = "no File Format entered";
            runtime::String_Manipulation::copy_string(parsed_filename.error.data(),
                                                      error_message);
            return parsed_filename;
        }

        for (uint32_t i = 0; parsed_filename.name[i] != '\0'; i++)
            if (!kernel::filesys::MoleculeOS_File_System_2::
                 is_valid_name_or_format_char(parsed_filename.name[i])) {
                static const char* error_message = "not a valid File Name";
                runtime::String_Manipulation::copy_string(parsed_filename.error.data(),
                                                          error_message);
                return parsed_filename;
            }

        for (uint32_t i = 0; parsed_filename.format[i] != '\0'; i++)
            if (!kernel::filesys::MoleculeOS_File_System_2::
                 is_valid_name_or_format_char(parsed_filename.format[i])) {
                static const char* error_message = "not a valid File Format";
                runtime::String_Manipulation::copy_string(parsed_filename.error.data(), 
                                                          error_message);
                return parsed_filename;
            }

        return parsed_filename;
    }
} // namespace shell::commands
