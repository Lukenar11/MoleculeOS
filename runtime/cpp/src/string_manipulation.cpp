#include "string_manipulation.hpp"

namespace runtime
{
    void String_Manipulation::copy_string(char* dest_ptr, 
                                          const char* src_ptr) noexcept {
        while ((*dest_ptr++ = *src_ptr++));
    }

    void String_Manipulation::copy_string_part(char* dest_ptr,
                                               const char* src_ptr, 
                                               uint32_t size) noexcept {
        const char null_char = '\0';

        for (uint32_t i = 0; i < size; i++) {
            if (src_ptr[i] == null_char) {
                dest_ptr[i] = null_char;
                return;
            }

            dest_ptr[i] = src_ptr[i];
        }

        dest_ptr[size - 1] = null_char;
    }

    const char* String_Manipulation::find_char_in_string(const char* string, 
                                                         int32_t symbol) 
                                                         noexcept {
        while (*string != static_cast<char>(symbol))
            if (!(*string++))
                return nullptr;

        return const_cast<char*>(string);
    }

    uint32_t String_Manipulation::get_string_length(const char *string) 
                                                    noexcept {
        uint32_t length = 0;
        while (*string != '\0') {
            string++;
            length++;
        }

        return length;
    }

    int32_t String_Manipulation::compare_strings(const char* a_ptr, 
                                                 const char* b_ptr) noexcept {
        while (*a_ptr && (*a_ptr == *b_ptr)) {
            ++a_ptr;
            ++b_ptr;
        }

        return static_cast<uint8_t>(*a_ptr) - static_cast<uint8_t>(*b_ptr);
    }

    String_Manipulation string_manip;
} // namespace runtime
