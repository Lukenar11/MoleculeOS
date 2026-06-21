/* 
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

#include "commands/utils/helpers.hpp"

namespace shell::commands
{
    Parsed_File_Name& parse_filename(const runtime::Array<char, 64>& arguments)
                                     noexcept {
        const char null_terminator = '\0';
        const uint32_t null = 0;

        static Parsed_File_Name parsed_filename;

        parsed_filename.name.fill(null_terminator);
        parsed_filename.format.fill(null_terminator);
        parsed_filename.error.fill(null_terminator);

        uint32_t file_name_index = null;
        uint32_t file_format_index = null;
        bool is_file_name = true;

        if (arguments[null] == null_terminator) [[unlikely]] {
            static const char* error_message = "missing argument";
            runtime::copy_string(parsed_filename.error.data(), error_message);

            return parsed_filename;
        }
    
        for (uint32_t i = null; arguments[i] != null_terminator; i++) [[likely]] {
            if (arguments[i] == '.') [[unlikely]] {
                is_file_name = false;
                continue;
            }

            if (arguments[i] == null_terminator) [[unlikely]]
                break;

            if ((is_file_name && 
                 !append_char(parsed_filename.name, file_name_index, arguments[i])) ||
                (file_name_index >= kernel::filesystem::MAX_FILENAME_LENGTH)) [[unlikely]] {
                static const char* error_message = "filename too long";
                runtime::copy_string(parsed_filename.error.data(), error_message);

                return parsed_filename;
            }
            
            if (!(is_file_name || 
                  append_char(parsed_filename.format, file_format_index, arguments[i])) || 
                (file_format_index >= kernel::filesystem::MAX_FILE_FORMAT_NAME_LENGTH)
                ) [[unlikely]] {
                static const char* error_message = "format too long";
                runtime::copy_string(parsed_filename.error.data(), error_message);

                return parsed_filename;
            }
        }

        const uint32_t one = 1;
        parsed_filename.name[file_name_index + one] = null_terminator;
        parsed_filename.format[file_format_index + one] = null_terminator;
    
        for (uint32_t i = null; parsed_filename.name[i] != '\0'; i++) [[likely]]
            if (!kernel::filesystem::mofs.is_valid_file_name_or_formant_char(
                parsed_filename.name[i])) [[unlikely]] {
                static const char* error_message = "not a valid File Name";
                runtime::copy_string(parsed_filename.error.data(), error_message);

                return parsed_filename;
            }

        for (uint32_t i = null; parsed_filename.format[i] != '\0'; i++) [[likely]]
            if (!kernel::filesystem::mofs.is_valid_file_name_or_formant_char(
                    parsed_filename.format[i]
                )) [[unlikely]] {
                static const char* error_message = "not a valid File Format";
                strcpy(parsed_filename.error.data(), error_message);

                return parsed_filename;
            }

        return parsed_filename;
    }
} // namespace shell::commands
