/**
 * @brief Implements a smal C runtime for the compiler.
 *
 * @copyright Copyright (c) 2026 Lukenar11 (Luke Matthes)
 * @license:  https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
 * @author    Lukenar11 (Luke Matthes)
 *
 * @note Don't use this code, it's only for the compiler.
 */


#include "compiler_c_runtime_library.h"

#ifdef __cplusplus
#   define NULL 0
#else
#   define NULL ((void*)0)
#endif


/**
 * @brief Copys a memory block with a specific byte size.
 *
 * @param destination pointer to the destination memory block
 * @param source      pointer to the source memory block
 * @param byte_size   memory block byte byte size
 *
 * @return destination
 */
_API_
void* 
memcpy(_IN_ void* destination, 
       _IN_ const void* source, 
       _IN_ uint32_t byte_size) 
{
    uint8_t* destination_byte  = (uint8_t*)destination;
    const uint8_t* source_byte = (const uint8_t*)source;

    while (byte_size--) {
        *destination_byte++ = *source_byte++;
    }

    return destination;
}


/**
 * @brief Copys a memory block with a specific byte size and 
 *        alows backward copying.
 *
 * @param destination pointer to the destination memory block
 * @param source      pointer to the source memory block
 * @param byte_size   memory block byte size
 *
 * @return pointer to the destination memory block
 */
_API_
void* 
memmove(_IN_ void* destination, 
        _IN_ const void* source, 
        _IN_ uint32_t byte_size) 
{
    uint8_t* destination_byte  = (uint8_t*)destination;
    const uint8_t* source_byte = (const uint8_t*)source;

    if ((uint32_t)destination_byte < (uint32_t)source_byte) {
        while (byte_size--) {
            *destination_byte++ = *source_byte++;
        }
    } 
    else {
        destination_byte += byte_size;
        source_byte      += byte_size;

        while (byte_size--) {
            *--destination_byte = *--source_byte;
        }
    }

    return destination;
}


/**
 * @brief Fils a memory block with 
 *        a specific byte size with a specific value.
 *
 * @param destination pointer to the memory block
 * @param value       value with which the memory block 
 *                    has to be filed
 * @param byte_size   memory block byte byte size
 *
 * @return pointer to the memory block
 */
_API_
void* 
memset(_IN_ void* destination, 
       _IN_ const int32_t value, 
       _IN_ uint32_t byte_size) 
{

    uint8_t* destination_byte = (uint8_t*)destination;
    uint8_t byte              = (uint8_t)value;

    while (byte_size--) {
        *destination_byte++ = byte;
    }

    return destination;
}


/**
 * @brief Compares the values in two diffenent memory blocks 
 *        with a specific byte size.
 *
 * @param a         pointer to destination memory block a
 * @param b         pointer to source memory block b
 * @param byte_size memory block byte size
 *
 * @return pointer to destination memory block a
 */
_API_
int32_t 
memcmp(_IN_ const void* a, 
       _IN_ const void* b, 
       _IN_ uint32_t byte_size) 
{
    int32_t result        = 0;
    const uint8_t* a_byte = (const uint8_t*)a;
    const uint8_t* b_byte = (const uint8_t*)b;

    while (byte_size--) {
        if (*a_byte < *b_byte) {
            result = -1;
            goto cleanup;  
        }

        if (*a_byte > *b_byte) {
            result = 1;
            goto cleanup;  
        }

        a_byte++;
        b_byte++;
    }

cleanup:
    return result;
}


/**
 * @brief Compares 2 different strings.
 * 
 * @param string_a pointer to string a.
 * @param string_b pointer to string b.
 */
_API_
void 
strcpy(_IN_ char* destination, 
       _IN_ const char* source) 
{
    while ((*destination++ = *source++)) {
    }
}


/**
 * @brief Copys the content of a string part in another string.
 * 
 * @note The destination buffer must be at least `byte_size` bytes long.
 * 
 * @param destination pointer to destination string
 * @param source      pointer to source string
 * @param byte_size   string part char size
 */
_API_
void 
strncpy(_IN_ char* destination, 
        _IN_ const char* source, 
        _IN_ uint32_t byte_size) 
{
    const char null_char = '\0';

    for (uint32_t byte = 0; byte < byte_size; byte++) {
        if (source[byte] == null_char) {
            for (; byte < byte_size; byte++) {
                destination[byte] = '\0';
            }

            goto cleanup;
        }

        destination[byte] = source[byte];
    }

cleanup:
    return;
}


/**
 * @brief Gets the length of a string.
 * 
 * @param string the string for the length calculation.
 * 
 * @return the length of `string`.
 */
_API_
uint32_t 
strlen(_IN_ const char* string) 
{
    uint32_t length = 0;

    while (*string != '\0') {
        ++string;
        ++length;
    }

    return length;
}


/**
 * @brief Compares 2 different strings.
 * 
 * @param string_a pointer to string a.
 * @param string_b pointer to string b.
 * 
 * @retval `-1` 
 *          If `string_a` is less then `string_b`.
 * 
 * @retval `1` 
 *          If `string_a` is greater then `string_b`.
 * 
 * @retval `0`
 *          If `string_a` and `string_b` are identical.
 */
_API_
int32_t 
strcmp(_IN_ const char* string_a, 
       _IN_ const char* string_b) 
{
    int32_t status = 0;

    while (*string_a && (*string_a == *string_b)) {
        ++string_a;
        ++string_b;
    }

    if (*string_a < *string_b) {
        status = -1;
    } 
    else if (*string_a > *string_b) {
        status = 1;
    }

    return status;
}


/**
 * @brief Finds a specific symbol in a string and returns the position.
 * 
 * @param string     the string to search
 * @param symbol     the symbol to search for
 * 
 * @retval pointer to the founded char
 * @retval `NULL` if `symbol` not found
 */
_API_
const 
char* 
strchr(_IN_ const char* string, 
       _IN_ int32_t symbol) 
{
    const char null_char = '\0';
    const char* result   = NULL;

    while (*string != (char)symbol) {
        if (*string == null_char) {
            goto cleanup;
        }

        string++;
    }

    result = string;

cleanup:
    return result;
}
