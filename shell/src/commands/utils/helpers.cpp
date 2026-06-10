/* 
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains a few helper functions 
    that are used by the shell command implementations.

NOTES:
    Since some functions are quite small, 
    they are intentionally placed in the header so that the compiler can inline them more easily.
*/

#include "commands/utils/helpers.hpp"

namespace shell::commands
{
    Parsed_File_Name& parse_filename(const runtime::Array<char, 64>& arguments) noexcept {
        const char null_terminator = '\0';
        const uint32_t null = 0;

        static Parsed_File_Name parsed;

        parsed.name.fill(null_terminator);
        parsed.format.fill(null_terminator);
        parsed.error.fill(null_terminator);

        uint32_t file_name_index = null;
        uint32_t file_format_index = null;
        bool is_file_name = true;

        if (arguments[null] == null_terminator) [[unlikely]] {
            static const char* error_message = "missing argument";
            strcpy(parsed.error.data(), error_message);

            return parsed;
        }
    
        for (uint32_t i = null; arguments[i] != null_terminator; i++) [[likely]] {
            if (arguments[i] == '.') [[unlikely]] {
                is_file_name = false;
                continue;
            }

            if (arguments[i] == null_terminator) [[unlikely]]
                break;

            if ((is_file_name && !append_char(parsed.name, file_name_index, arguments[i])) || 
                (file_name_index >= kernel::filesystem::MAX_FILENAME_LENGTH)) [[unlikely]] {
                static const char* error_message = "filename too long";
                strcpy(parsed.error.data(), error_message);

                return parsed;
            }
            
            if (!(is_file_name || append_char(parsed.format, file_format_index, arguments[i])) || 
                (file_format_index >= kernel::filesystem::MAX_FILE_FORMAT_NAME_LENGTH)) [[unlikely]] {
                static const char* error_message = "format too long";
                strcpy(parsed.error.data(), error_message);

                return parsed;
            }
        }

        const uint32_t one = 1;
        parsed.name[file_name_index + one] = null_terminator;
        parsed.format[file_format_index + one] = null_terminator;
    
        for (uint32_t i = null; parsed.name[i] != '\0'; i++) [[likely]]
            if (!kernel::filesystem::mofs.is_valid_file_name_or_formant_char(parsed.name[i])
               ) [[unlikely]] {
                static const char* error_message = "not a valid File Name";
                strcpy(parsed.error.data(), error_message);

                return parsed;
            }

        for (uint32_t i = null; parsed.format[i] != '\0'; i++) [[likely]]
            if (!kernel::filesystem::mofs.is_valid_file_name_or_formant_char(parsed.format[i])
               ) [[unlikely]] {
                static const char* error_message = "not a valid File Format";
                strcpy(parsed.error.data(), error_message);

                return parsed;
            }

        return parsed;
    }
} // namespace shell::commands
