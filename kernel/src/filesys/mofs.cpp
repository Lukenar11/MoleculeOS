#include "filesys/mofs.hpp"

#include <array.hpp>
#include <kernel.hpp>
#include <string.hpp>
#include <memory.hpp>


namespace
{
    using namespace kernel::filesys;
    using namespace kernel;


    constexpr uint32_t MAX_FILE_HEADERS = 256;
    constexpr uint32_t MAX_OPEN_HANDLES = 64;


    /**
     * @brief Describes file properties and access permissions.
     */
    enum class File_Properties : uint8_t {
        NONE       = 0x00,
        READABLE   = 0x01,
        WRITABLE   = 0x02,
        EXECUTABLE = 0x04,
        EDITABLE   = 0x08,
        DELETABLE  = 0x10
    };


    /**
     * @brief Performs a bitwise AND on two file property flags.
     * 
     * @param flags           First file property flag.
     * @param additional_flag Second file property flag.
     * 
     * @return Bitwise AND result.
     */
    [[nodiscard]]
    inline
    constexpr
    File_Properties
    operator&(File_Properties flags, 
              File_Properties additional_flag) noexcept {
        return static_cast<File_Properties>(static_cast<uint8_t>(flags) &
                                            static_cast<uint8_t>(additional_flag));
    }


    /**
     * @brief Performs a bitwise OR on two file property flags.
     * 
     * @param flags           First file property flag.
     * @param additional_flag Second file property flag.
     * 
     * @return Bitwise OR result.
     */
    [[nodiscard]]
    inline
    constexpr
    File_Properties
    operator|(File_Properties flags, 
              File_Properties additional_flag) noexcept {
        return static_cast<File_Properties>(static_cast<uint8_t>(flags) |
                                            static_cast<uint8_t>(additional_flag));
    }


    /**
     * @brief Performs a bitwise XOR on two file property flags.
     * 
     * @param flags           First file property flag.
     * @param additional_flag Second file property flag.
     * 
     * @return Bitwise XOR result.
     */
    [[nodiscard]]
    inline
    constexpr
    File_Properties
    operator^(File_Properties flags, 
              File_Properties additional_flag) noexcept {
        return static_cast<File_Properties>(static_cast<uint8_t>(flags) ^
                                            static_cast<uint8_t>(additional_flag));
    }


    /**
     * @brief Stores file metadata and the associated file data information.
     * 
     * @note `name` and `format`
     *        are set to `'\0'` by default.
     * 
     * @note `name_hash`, `format_hash`, `byte_size` and `used_byte_size` 
     *        is set to `0` by default.
     * 
     * @note `data` and `resume_cursor` 
     *        is set to `nullptr` by default.
     * 
     * @note `file_open` 
     *        is set to `false` by default.
     * 
     * @note `properties` 
     *        is set to `File_Properties::NONE` by default.
     */
    struct File_Header final {
        stdlib::Array<char, max_name_length() + 1> name;
        stdlib::Array<char, max_format_length() + 1> format;
        uint32_t name_hash         = 0;
        uint32_t format_hash       = 0;
        uint32_t byte_size         = 0;
        uint32_t used_byte_size    = 0;
        uint8_t* data              = nullptr;
        uint8_t* resume_cursor     = nullptr;
        bool file_open             = false;
        File_Properties properties = File_Properties::NONE;
    };


    /**
     * @brief Links an open handle to its file header.
     * 
     * @note `header` 
     *        is set to `nullptr` by default.
     * 
     * @note `handle` 
     *        is set to `nullptr` by default.
     * 
     * @note `in_use` 
     *        is set to `false` by default.
     */
    struct Handle_Metadata final {
        File_Header* header = nullptr;
        Handle* handle      = nullptr;
        bool in_use         = false;
    };


    stdlib::Array<File_Header, MAX_FILE_HEADERS> file_header_table;
    stdlib::Array<Handle_Metadata, MAX_OPEN_HANDLES> open_handle_table;


    /**
     * @brief Calculates the FNV-1a hash of a string.
     * 
     * @param text String to hash.
     *
     * @return FNV-1a hash output.
     */
    [[nodiscard]]
    uint32_t
    to_fnv1a_hash(_IN_ const char* text) noexcept {
        uint32_t hash               = 0;
        const uint32_t prime_offset = 0x01000193;

        if (!text || text[0] == '\0') [[unlikely]] {
            goto cleanup;
        }

        hash = 0x811C9DC5;

        for (uint32_t cursor = 0; text[cursor] != '\0'; cursor++) [[likely]] {
            hash ^= static_cast<uint8_t>(text[cursor]);
            hash *= prime_offset;
        }

    cleanup:
        return hash;
    }


    /**
     * @brief Gets the metadata of an opened handle.
     * 
     * @param metadata Pointer to the Handle metadata of the opened handle.
     * @param handle   Handle to search for.
     *
     * @retval `status::NOT_FOUND`
     *          If `handle` is not registered as open.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]]
    status_t
    get_open_handle(_OUT_ Handle_Metadata*& metadata,
                    _IN_  const Handle& handle) noexcept {
        status_t status = status::NOT_FOUND;

        metadata = nullptr;

        for (auto& open_handle : open_handle_table) [[likely]] {
            if (!open_handle.in_use || !open_handle.handle) {
                open_handle = {};
                continue;
            }

            if (open_handle.handle == &handle) {
                metadata = &open_handle;
                status   = status::SUCCESS;

                break;
            }
        }

        return status;
    }


    /**
     * @brief Registers a handle and gets its metadata.
     *
     * @param metadata Pointer to the assigned handle metadata.
     * @param handle   Handle to register.
     *
     * @retval `status::ALREADY_EXISTS`
     *          If `handle` is already registered as open.
     *
     * @retval `status::FS_OUT_OF_SPACE`
     *          If no free handle metadata entry is available.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]]
    status_t
    set_open_handle(_OUT_ Handle_Metadata*& metadata, 
                    _IN_  const Handle& handle) noexcept {
        status_t status = get_open_handle(metadata, handle);

        metadata = nullptr;

        if (status == status::SUCCESS) [[unlikely]] {
            status = status::ALREADY_EXISTS;
            goto cleanup;
        }

        status = status::FS_OUT_OF_SPACE;

        for (auto& open_handle : open_handle_table) [[likely]] {
            if (!open_handle.in_use) {
                open_handle.in_use = true;
                open_handle.handle = const_cast<Handle*>(&handle);
                open_handle.header = nullptr;

                status   = status::SUCCESS;
                metadata = &open_handle;
                break;
            }
        }
        
    cleanup:
        return status;
    }


    /**
     * @brief Clears the metadata of a registered handle.
     *
     * @param handle Handle to be released.
     *
     * @retval `status::NOT_FOUND`
     *          If `handle` is not registered as open.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]]
    status_t
    release_open_handle(_IN_ const Handle& handle) noexcept {
        status_t status              = status::SUCCESS;
        Handle_Metadata* open_handle = nullptr;

        status = get_open_handle(open_handle, handle);
        if (status == status::SUCCESS) [[likely]] {
            *open_handle = {};
        }

        return status;
    }


    /**
     * @brief Validates a file name and a file format.
     *
     * @param name   File name to validate.
     * @param format File format to validate.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `name` is empty or to long.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `format` is empty or to long.
     * 
     * @retval `status::FAIL`
     *          Internal unexpected error.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]]
    status_t
    validate_name_and_format(_IN_ const char* const name, 
                             _IN_ const char* const format) noexcept {
        status_t status        = status::SUCCESS;
        uint32_t name_length   = 0;
        uint32_t format_length = 0;

        if (!name) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        status = stdlib::string::get_length(name_length, name);
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        if (name[0] == '\0' || 
            name_length > max_name_length()) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!format) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        status = stdlib::string::get_length(format_length, format);
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        if (format[0] == '\0' || 
            format_length > max_format_length()) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
        }

    cleanup:
        return status;
    }


    /**
     * @brief Repairs an invalid file header and 
     *        resets it to the default state.
     *
     * @param current_header File header to repair.
     *
     * @retval `status::POINTER_OUT_OF_RANGE`
     *          If `current_header.data` points outside of allocated memory.
     *
     * @retval `status::HEAP_CORRUPTED`
     *          If the heap is corrupted.
     *
     * @retval `status::FAIL`
     *          If an unknown deallocation error occurred.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]]
    status_t
    repair_file_header(_INOUT_ File_Header& current_header) noexcept {
        status_t status   = status::SUCCESS;
        void* header_data = reinterpret_cast<void*&>(current_header.data);

        if (current_header.name.data()[0] == '\0' ||
            current_header.format.data()[0] == '\0' ||
            current_header.name_hash == 0 ||
            current_header.format_hash == 0 ||
            !current_header.data || 
            current_header.byte_size == 0 ||
            current_header.used_byte_size > current_header.byte_size) {
            if (current_header.data) {
                status = heap::deallocate(header_data);
            }

            current_header = {};
        }

        return status;
    }


    /**
     * @brief Checks whether a file header matches a file name and format.
     *
     * @param file_header  File header to compare.
     * @param name         Name to compare.
     * @param format       Format to compare.
     * @param name_hash    Hash of the file name.
     * @param format_hash  Hash of the file format.
     *
     * @return `true`/`false`
     *          If `name` and `format` matches or don't match the file header.
     */
    [[nodiscard]]
    bool
    filename_and_format_matches(_IN_ const File_Header& file_header,
                                _IN_ const char* const name,
                                _IN_ const char* const format,
                                _IN_ const uint32_t name_hash,
                                _IN_ const uint32_t format_hash) noexcept {
        bool is_match = false;

        if (file_header.name_hash == name_hash && 
            file_header.format_hash == format_hash) {
            // hash collisions guard, by comparing the full strings.
            if (stdlib::string::compare(file_header.name.data(), 
                                        name) == status::EQUAL_TO && 
                stdlib::string::compare(file_header.format.data(), 
                                        format) == status::EQUAL_TO) {
                is_match = true;
            }
        }

        return is_match;
    }


    /**
     * @brief Finds a file header by name and format.
     *
     * @param header Pointer to the found file header.
     * @param name   File name to search for.
     * @param format File format to search for.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `name` is empty or to long.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_C`
     *          If `format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *          If `format` is empty or to long.
     *
     * @retval `status::NOT_FOUND`
     *          If no matching file was found.
     * 
     * @retval `status::FAIL`
     *          Internal unexpected error.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]]
    status_t
    find_file(_OUT_ File_Header*& header,
              _IN_  const char* const name,
              _IN_  const char* const format) noexcept {
        status_t status        = validate_name_and_format(name, format);
        uint32_t name_hash     = 0;
        uint32_t format_hash   = 0;
        status_t repair_status = status::FAIL;

        header = nullptr;

        switch (status) {
        case status::NULL_POINTER | status::flags::PARAM_A:
            // The status flag `PARAM_A` is changed to `PARAM_B`, 
            // so that the returned status code matches the method signature.

            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;

        case status::INVALID_PARAMETER | status::flags::PARAM_A:
            // The status flag `PARAM_A` is changed to `PARAM_B`, 
            // so that the returned status code matches the method signature.

            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;

        case status::NULL_POINTER | status::flags::PARAM_B:
            // The status flag `PARAM_B` is changed to `PARAM_C`, 
            // so that the returned status code matches the method signature.

            status = status::NULL_POINTER | status::flags::PARAM_C;
            goto cleanup;

        case status::INVALID_PARAMETER | status::flags::PARAM_B:
            // The status flag `PARAM_B` is changed to `PARAM_C`, 
            // so that the returned status code matches the method signature.

            status = status::INVALID_PARAMETER | status::flags::PARAM_C;
            goto cleanup;
        }

        status = status::NOT_FOUND;

        name_hash   = to_fnv1a_hash(name);
        format_hash = to_fnv1a_hash(format);

        for (auto& file_header : file_header_table) [[likely]] {
            repair_status = repair_file_header(file_header);
            if (repair_status != status::SUCCESS) [[unlikely]] {
                sys::panic("File-System repairing failed!");
            }

            if (filename_and_format_matches(file_header, 
                                            name, 
                                            format, 
                                            name_hash, 
                                            format_hash)) {
                header = &file_header;
                status = status::SUCCESS;

                break;
            }
        }

    cleanup:
        return status;
    }


    /**
     * @brief Finds the first free file header slot.
     *
     * @param header Pointer to the free file header.
     *
     * @retval `status::FS_OUT_OF_SPACE`
     *          If no free file header slot is available.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]]
    status_t
    find_free_file_header(_OUT_ File_Header*& header) noexcept {
        status_t status = status::FS_OUT_OF_SPACE;
        
        header = nullptr;

        for (auto& file_header : file_header_table) [[likely]] {
            status = repair_file_header(file_header);
            if (status != status::SUCCESS) [[unlikely]] {
                sys::panic("File-System repairing failed!");
            }

            if (file_header.properties == File_Properties::NONE) {
                header = &file_header;
                status = status::SUCCESS;

                break;
            }
        }

        return status;
    }


    /**
     * @brief Creates a new file.
     *
     * @param header    Pointer to the created file header.
     * @param name      Name of the file to create.
     * @param format    Format of the file to create.
     * @param byte_size File data size.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `name` is empty or to long.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_C`
     *          If `format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *          If `format` is empty or to long.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D`
     *          If `byte_size` is zero.
     * 
     * @retval `status::FS_OUT_OF_SPACE`
     *          If no free file header slot is available.
     * 
     * @retval `status::FAIL`
     *          Internal unexpected error.
     * 
     * @retval `status::HEAP_EXHAUSTED`
     *          If the heap doesn't have enough free space or 
     *          `byte_size` is to large.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]]
    status_t 
    make_file(_OUT_ File_Header*& header,
              _IN_  const char* const name,
              _IN_  const char* const format,
              _IN_  const uint32_t byte_size) noexcept {
        status_t status        = status::SUCCESS;
        status_t repair_status = status::SUCCESS;

        status = find_free_file_header(header);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = validate_name_and_format(name, format);
        switch (status) {
        case status::NULL_POINTER | status::flags::PARAM_A:
            // The status flag `PARAM_A` is changed to `PARAM_B`, 
            // so that the returned status code matches the method signature.

            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;

        case status::INVALID_PARAMETER | status::flags::PARAM_A:
            // The status flag `PARAM_A` is changed to `PARAM_B`, 
            // so that the returned status code matches the method signature.

            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;

        case status::NULL_POINTER | status::flags::PARAM_B:
            // The status flag `PARAM_B` is changed to `PARAM_C`, 
            // so that the returned status code matches the method signature.

            status = status::NULL_POINTER | status::flags::PARAM_C;
            goto cleanup;

        case status::INVALID_PARAMETER | status::flags::PARAM_B:
            // The status flag `PARAM_B` is changed to `PARAM_C`, 
            // so that the returned status code matches the method signature.

            status = status::INVALID_PARAMETER | status::flags::PARAM_C;
            goto cleanup;
        }

        if (byte_size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_D;
            goto cleanup;
        }

        status = stdlib::string::copy(header->name.data(), name);
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }
        
        status = stdlib::string::copy(header->format.data(), format);
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        header->name_hash      = to_fnv1a_hash(name);
        header->format_hash    = to_fnv1a_hash(format);
        header->byte_size      = byte_size;
        header->used_byte_size = 0;

        // The file data is located on the heap and remains allocated until the
        // file is deleted, recovered from an error, 
        // or released during shutdown after saving.
        status = heap::allocate(reinterpret_cast<void*&>(header->data), 
                                byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        header->resume_cursor = header->data;
        header->file_open     = false;
        header->properties    = File_Properties::NONE;

    cleanup:
        if (status != status::SUCCESS && header) [[unlikely]] {
            repair_status = repair_file_header(*header);
            if (repair_status != status::SUCCESS) [[unlikely]] {
                sys::panic("File-System repairing failed!");
            }

            header = nullptr;
        }

        return status;
    }


    /**
     * @brief Validates file stream access for reading and writing.
     *
     * @param file_handle    File handle to validate.
     * @param data           Input data pointer to validate.
     * @param data_byte_size Byte size of the buffer to validate.
     * @param offset         File offset to validate.
     * @param byte_length    Bytes to read/write to validate.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If internal `handle` pointers are invalid.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If the handle is invalid.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `data` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *          If `data_byte_size` is to smal for `byte_length`.
     *
     * @retval `status::BUFFER_OVERFLOW`
     *          If the range is outside the file buffer.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]] 
    status_t 
    validate_file_stream_access(_IN_ const Handle& file_handle,
                                _IN_ const uint8_t* const data,
                                _IN_ const uint32_t data_byte_size,
                                _IN_ const uint32_t offset,
                                _IN_ const uint32_t byte_length) noexcept {
        status_t status        = status::SUCCESS;
        uint32_t cursor_offset = 0;

        if (!file_handle.buffer || !file_handle.resume_cursor) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }
        
        if (file_handle.buffer_size == 0 ||
            file_handle.resume_cursor < file_handle.buffer ||
            file_handle.resume_cursor > 
            file_handle.buffer + file_handle.buffer_size) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!data) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (data_byte_size < byte_length) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_C;
            goto cleanup;
        }

        cursor_offset = static_cast<uint32_t>(file_handle.resume_cursor - 
                                              file_handle.buffer);

        if (cursor_offset >= file_handle.buffer_size ||
            offset > file_handle.buffer_size - cursor_offset ||
            byte_length > 
            file_handle.buffer_size - cursor_offset - offset) [[unlikely]] {
            status = status::BUFFER_OVERFLOW;
        }

    cleanup:
        return status;
    }

    
    /**
     * @brief Validates a handle for stream & file flushing and 
     *        gets a pointer to validated handle metadata.
     * 
     * @param handle_metadata Pointer to the validated handle metadata.
     * @param handle          Handle to validate.
     * 
     * @retval `status::NOT_FOUND`
     *          If `handle` is not registered as open.
     * 
     * @retval `status::NULL_POINTER`
     *          If internal handle pointer are invalid.
     *
     * @retval `status::INVALID_PARAMETER`
     *          If the handle is invalid.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]]
    status_t
    validate_flush_handling(_OUT_ Handle_Metadata*& handle_metadata,
                            _IN_  const Handle& handle) noexcept {
        handle_metadata = nullptr;

        status_t status = get_open_handle(handle_metadata, handle);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (!handle_metadata->header) [[unlikely]] {
            sys::panic("File-System flush, invalid handle metadata!");
        }

        if (!handle.buffer || !handle.resume_cursor) [[unlikely]] {
            status = status::NULL_POINTER;
            goto cleanup;
        }

        if (handle.buffer_size == 0 ||
            handle.resume_cursor < handle.buffer ||
            handle.resume_cursor > 
            handle.buffer + handle.buffer_size) [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

    cleanup:
        return status;
    }


    /**
     * @brief Gets the file content.
     *
     * @param handle_metadata Pointer to the validated handle metadata.
     * @param handle          Handle to validate.
     * @param io_buffer       Pointer to a I/O buffer for file content.
     *
     * @retval `status::NOT_FOUND`
     *          If `handle` is not registered as open.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If internal `handle` pointers are invalid.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `handle` is invalid.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_C`
     *          If `buffer` is a `nullptr`.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]]
    status_t 
    validate_file_content_access(_OUT_ Handle_Metadata*& handle_metadata,
                                 _IN_  const Handle& handle,
                                 _IN_  const uint8_t* io_buffer) noexcept {
        handle_metadata = nullptr;

        status_t status = get_open_handle(handle_metadata, handle);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (!handle_metadata->header) [[unlikely]] {
            sys::panic("File-System content, invalid handle metadata!");
        }

        if (!handle.buffer || !handle.resume_cursor) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (handle.buffer_size == 0 ||
            handle.resume_cursor < handle.buffer ||
            handle.resume_cursor > 
            handle.buffer + handle.buffer_size) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (!io_buffer) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_C;
        }

    cleanup:
        return status;
    }


    [[nodiscard]]
    status_t
    validate_open_file_state(_OUT_ Handle_Metadata*& handle_metadata,
                             _IN_  const Handle& handle) noexcept {
        status_t status = get_open_handle(handle_metadata, handle);
        if (status != status::SUCCESS) [[unlikely]] {
            return status;
        }

        if (!handle_metadata->header) [[unlikely]] {
            sys::panic("File-System 'copy_file_content' invalid handle metadata!");
        }

        if (!handle.buffer || !handle.resume_cursor) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (handle.buffer_size == 0 ||
            handle.resume_cursor < handle.buffer ||
            handle.resume_cursor > handle.buffer + 
            handle.buffer_size) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

    cleanup:
        return status;
    }
} // namespace


namespace kernel::filesys
{
    /** 
     * @brief Initializes the file system. 
     */
    _API_ 
    void 
    init() noexcept {
        file_header_table.fill({});
        open_handle_table.fill({});
    }


    /**
     * @brief Opens an existing file or creates a new.
     *
     * @param handle Opened handle to initialize.
     * @param name   Name of the file to open.
     * @param format Format of the file to open.
     * @param flags  Flags for the file to open.
     *
     * @retval `status::ALREADY_EXISTS`
     *          If the file is already open.
     *
     * @retval `status::FS_OUT_OF_SPACE`
     *          If the file opening is not available.
     *
     * @retval `status::NOT_FOUND`
     *          If the file was not found and `CREATE` is not set.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `name` is empty or to long.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_C`
     *          If `format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *          If `format` is empty or to long.
     *
     * @retval `status::HEAP_EXHAUSTED`
     *          If the system does not have enough free memory space.
     * 
     * @retval `status::FAIL`
     *          Internal unexpected error.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    [[nodiscard]]
    status_t
    open_file(_OUT_ Handle& handle,
              _IN_  const char* const name,
              _IN_  const char* const format,
              _IN_  const Handle_Flags flags) noexcept {
        status_t status                  = status::SUCCESS;
        Handle_Metadata* handle_metadata = nullptr;
        File_Header* file_header         = nullptr;
        bool handle_registered           = false;
        bool file_was_created            = false;
        bool buffer_allocated            = false;
        status_t cleanup_status          = status::SUCCESS;

        handle = {};

        status = set_open_handle(handle_metadata, handle);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        handle_registered = true;

        status = find_file(file_header, name, format);
        if (status == status::NOT_FOUND) {
            if ((flags & Handle_Flags::CREATE) != Handle_Flags::CREATE) {
                goto cleanup;
            }

            status = make_file(file_header, name, format, 1024);
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }

            file_was_created = true;
        }

        handle_metadata->header = file_header;

        handle.flags       = flags;
        handle.buffer_size = file_header->byte_size;

        status = heap::clear_allocate(reinterpret_cast<void*&>(handle.buffer),
                                      handle.buffer_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        buffer_allocated = true;

        handle.resume_cursor = handle.buffer;

    cleanup:
        if (status != status::SUCCESS) [[unlikely]] {
            if (file_was_created && file_header) {
                cleanup_status = remove_file(handle);

                // All resources where automatically released by `remove_file`.
                buffer_allocated  = false;
                handle_registered = false;
            }

            if (buffer_allocated && handle.buffer) {
                cleanup_status = heap::deallocate(reinterpret_cast<void*&>(handle.buffer));
            }

            if (handle_registered) {
                cleanup_status = release_open_handle(handle);
            }

            handle = {};
        }

        if (cleanup_status != status::SUCCESS) [[unlikely]] {
            sys::panic("File-System 'open_file' cleanup failed!");
        }

        return status;
    }


    /**
     * @brief Closes an opened file.
     *
     * @param handle Handle for the file to close.
     *
     * @retval `status::NOT_FOUND`
     *          If `handle` is not registered as open.
     * 
     * @retval `status::HEAP_CORRUPTED` 
     *          If a deallocation error has occurred. 
     * 
     * @retval `status::FAIL` 
     *          Unknown error.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    [[nodiscard]]
    status_t
    close_file(_INOUT_ Handle& handle) noexcept {
        Handle_Metadata* _ = nullptr;
        status_t status    = get_open_handle(_, handle);

        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (handle.buffer) {
            status = heap::deallocate(reinterpret_cast<void*&>(handle.buffer));
            if (status == status::POINTER_OUT_OF_RANGE) [[unlikely]] {
                status = status::FAIL;
            }
            else if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }
        }

        status = release_open_handle(handle);

    cleanup:
        handle = {};

        return status;
    }


    /**
     * @brief Renames a file.
     *
     * @param new_name   New file name.
     * @param new_format New file format.
     * @param handle     Handle for the file to rename.  
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `new_name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `new_name` is empty or to long.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `new_format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `new_format` is empty or to long.
     *
     * @retval `status::ALREADY_EXISTS`
     *          If a file with the new name already exists.
     * 
     * @retval `status::FAIL`
     *          Internal unexpected error.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    [[nodiscard]]
    status_t
    rename_file(_IN_ const char* const new_name,
                _IN_ const char* const new_format,
                _IN_ const Handle& handle) noexcept {
        status_t status                  = status::NOT_FOUND;
        File_Header* file_header         = nullptr;
        Handle_Metadata* handle_metadata = nullptr;

        status = get_open_handle(handle_metadata, handle);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (!handle_metadata->header) [[unlikely]] {
            sys::panic("File-System 'rename_file' invalid handle metadata!");
        }

        status = find_file(file_header, new_name, new_format);
        switch (status) {
        case status::NULL_POINTER | status::flags::PARAM_B:
            // The status flag `PARAM_B` is changed to `PARAM_A`, 
            // so that the returned status code matches the method signature.

            status = status::NULL_POINTER | status::flags::PARAM_A;
            break;

        case status::INVALID_PARAMETER | status::flags::PARAM_B:
            // The status flag `PARAM_B` is changed to `PARAM_A`, 
            // so that the returned status code matches the method signature.

            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            break;

        case status::NULL_POINTER | status::flags::PARAM_C:
            // The status flag `PARAM_C` is changed to `PARAM_B`, 
            // so that the returned status code matches the method signature.

            status = status::NULL_POINTER | status::flags::PARAM_B;
            break;

        case status::INVALID_PARAMETER | status::flags::PARAM_C:
            // The status flag `PARAM_C` is changed to `PARAM_B`, 
            // so that the returned status code matches the method signature.

            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            break;

        case status::SUCCESS:
            status = status::ALREADY_EXISTS;
            break;

        case status::NOT_FOUND:
            file_header = handle_metadata->header;
            
            status = stdlib::string::copy(file_header->name.data(), 
                                          new_name);
            if (status != status::SUCCESS) [[unlikely]] {
                status = status::FAIL;
                goto cleanup;
            }
            
            status = stdlib::string::copy(file_header->format.data(), 
                                          new_format);
            if (status != status::SUCCESS) [[unlikely]] {
                status = status::FAIL;
                goto cleanup;
            }

            file_header->name_hash   = to_fnv1a_hash(new_name);
            file_header->format_hash = to_fnv1a_hash(new_format);

            status = status::SUCCESS;
            break;
        }

    cleanup:
        return status;
    }


    /**
     * @brief Removes an opened file.
     *
     * @param handle Handle for the opened file to remove.
     *
     * @retval `status::NOT_FOUND`
     *          If `handle` is not registered as open.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    [[nodiscard]]
    status_t
    remove_file(_INOUT_ Handle& handle) noexcept {
        status_t status                  = status::SUCCESS;
        Handle_Metadata* handle_metadata = nullptr;
        status_t repair_status           = status::FAIL;
        bool handle_found                = false;

        status = get_open_handle(handle_metadata, handle);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        handle_found = true;

        if (!handle_metadata->header) [[unlikely]] {
            sys::panic("File-System 'remove_file' invalid handle metadata!");
        }

        if (handle.buffer) {
            status = heap::deallocate(reinterpret_cast<void*&>(handle.buffer));
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }
        }

        if (handle_metadata->header->data) {
            status = heap::deallocate(reinterpret_cast<void*&>(handle_metadata->header->data));
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }
        }

        status = repair_file_header(*handle_metadata->header);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = release_open_handle(handle);

    cleanup:
        if (status != status::SUCCESS) [[unlikely]] {
            if (handle_found && handle_metadata && handle_metadata->header) {
                repair_status = repair_file_header(*handle_metadata->header);
                if (repair_status != status::SUCCESS) [[unlikely]] {
                    goto panic;
                }
            }
        
            if (handle_found) {
                repair_status = release_open_handle(handle);
                if (repair_status != status::SUCCESS) [[unlikely]] {
                    goto panic;
                }
            }
        
            goto cleanup_done;

    panic:
            sys::panic("File-System 'remove_file' cleanup failed!");
        }

    cleanup_done:
        handle = {};
        
        return status;
    }


    /**
     * @brief Reads data from a file.
     *
     * @param handle         Handle of the file to read for.
     * @param data           Pointer to a buffer for outputted file data.
     * @param data_byte_size Byte size of the buffer for outputted file data.
     * @param offset         File offset to read.
     * @param byte_length    Bytes to read.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If internal `handle` pointers are invalid.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If the handle is invalid.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `data` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *          If `data_byte_size` is to smal for `byte_length`.
     *
     * @retval `status::BUFFER_OVERFLOW`
     *          If the range is outside the file buffer.
     *
     * @retval `status::FAIL`
     *          Internal unexpected error.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    [[nodiscard]]
    status_t
    read_file(_INOUT_ Handle& handle,
              _OUT_   uint8_t* data,
              _IN_    const uint32_t data_byte_size,
              _IN_    const uint32_t offset,
              _IN_    const uint32_t byte_length) noexcept {
        status_t status = status::SUCCESS;
        uint8_t* cursor = nullptr;

        status = validate_file_stream_access(handle, 
                                             data, 
                                             data_byte_size, 
                                             offset, 
                                             byte_length);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        cursor = handle.resume_cursor;
    
        status = stdlib::memory::copy_block(data, 
                                            cursor + offset, 
                                            byte_length);
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }
    
        handle.resume_cursor = const_cast<uint8_t*>(cursor + 
                                                    offset + byte_length);
    
    cleanup:
        return status;
    }


    /**
     * @brief Writes data to a file.
     *
     * @param handle         Handle of the file to write for.
     * @param data           Pointer to a input buffer for new file data.
     * @param data_byte_size Byte size of the buffer for new file data.
     * @param offset         File offset to write.
     * @param byte_length    Bytes to write.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If internal `handle` pointers are invalid.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If the handle is invalid.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `data` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *          If `data_byte_size` is to smal for `byte_length`.
     *
     * @retval `status::BUFFER_OVERFLOW`
     *          If the range is outside the file buffer.
     *
     * @retval `status::FAIL`
     *          Internal unexpected error.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    [[nodiscard]]
    status_t
    write_file(_INOUT_ Handle& handle,
               _IN_    const uint8_t* data,
               _IN_    const uint32_t data_byte_size,
               _IN_    const uint32_t offset,
               _IN_    const uint32_t byte_length) noexcept {
        status_t status = status::SUCCESS;
        uint8_t* cursor = nullptr;

        status = validate_file_stream_access(handle, 
                                             data, 
                                             data_byte_size, 
                                             offset, 
                                             byte_length);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        cursor = handle.resume_cursor;

        status = stdlib::memory::copy_block(handle.resume_cursor + offset, 
                                            data, 
                                            byte_length);
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        handle.resume_cursor = const_cast<uint8_t*>(cursor + 
                                                    offset + byte_length);

    cleanup:
        return status;
    }


    /**
     * @brief Flushes a file stream (clears the internal stream buffer and
     *        resets the internal stream buffer handling).
     * 
     * @retval `status::NOT_FOUND`
     *          If `handle` is not registered as open.
     * 
     * @retval `status::NULL_POINTER`
     *          If internal `handle` pointers are invalid.
     *
     * @retval `status::INVALID_PARAMETER`
     *          If the handle is invalid.
     * 
     * @retval `status::FAIL`
     *          Internal unexpected error.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    [[nodiscard]]
    status_t
    flush_stream(_INOUT_ Handle& handle) noexcept {
        Handle_Metadata* handle_metadata = nullptr;

        status_t status = validate_flush_handling(handle_metadata, handle);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = stdlib::memory::set_block(handle.buffer,
                                           0x00,
                                           handle.buffer_size);
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        handle.resume_cursor = handle.buffer;

    cleanup:
        return status;
    }


    /**
     * @brief Flushes the file content.
     * 
     * @retval `status::NOT_FOUND`
     *          If `handle` is not registered as open.
     * 
     * @retval `status::NULL_POINTER`
     *          If internal handle pointer are invalid.
     *
     * @retval `status::INVALID_PARAMETER`
     *          If the handle is invalid.
     * 
     * @retval `status::FAIL`
     *          Internal unexpected error.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    [[nodiscard]]
    status_t
    flush_file(_IN_ const Handle& handle) noexcept {
        Handle_Metadata* handle_metadata = nullptr;

        status_t status = validate_flush_handling(handle_metadata, handle);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = stdlib::memory::copy_block(handle_metadata->header->data,
                                            handle.buffer,
                                            handle.buffer_size);
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
        }

    cleanup:
        return status;
    }


    /**
     * @brief Gets the file size.
     *
     * @retval `status::NOT_FOUND`
     *          If `handle` is not registered as open.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    [[nodiscard]]
    status_t
    get_file_size(_OUT_ uint32_t& byte_size,
                  _IN_  const Handle& handle) noexcept {
        status_t status                  = status::SUCCESS;
        Handle_Metadata* handle_metadata = nullptr;

        byte_size = 0;

        status = get_open_handle(handle_metadata, handle);
        if (status == status::SUCCESS) [[unlikely]] {
            if (!handle_metadata->header) [[unlikely]] {
                sys::panic("File-System 'get_file_size' invalid handle metadata!");
            }

            byte_size = handle_metadata->header->byte_size;
        }

        return status;
    }


    /**
     * @brief Sets a new file size.
     *
     * @retval `status::NOT_FOUND`
     *          If `handle` is not registered as open.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    [[nodiscard]]
    status_t
    set_file_size(_IN_ Handle& handle,
                  _IN_ const uint32_t new_byte_size) noexcept {
        status_t status                  = status::SUCCESS;
        Handle_Metadata* handle_metadata = nullptr;

        status = get_open_handle(handle_metadata, handle);
        if (status == status::SUCCESS) [[unlikely]] {
            if (!handle_metadata->header) [[unlikely]] {
                sys::panic("File-System 'set_file_size' invalid handle metadata!");
            }

            handle_metadata->header->byte_size = new_byte_size;
        }

        return status;
    }


    /**
     * @brief Gets the file content.
     *
     * @param handle           Handle for the file to get the content from.
     * @param buffer           Pointer to a buffer for the file content.
     * @param buffer_byte_size Byte size of the buffer for the file content.
     *
     * @retval `status::NOT_FOUND`
     *          If `handle` is not registered as open.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If internal `handle` pointers are invalid.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `handle` is invalid.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `data` is a `nullptr`.
     *
     * @retval `status::BUFFER_TOO_SMALL | status::flags::PARAM_C`
     *          If the buffer is too small.
     *
     * @retval `status::FAIL`
     *          If an unexpected internal error occurred.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    [[nodiscard]]
    status_t
    get_file_content(_INOUT_ Handle& handle,
                     _INOUT_ uint8_t*& buffer,
                     _IN_    const uint32_t buffer_byte_size) noexcept {
        status_t status                  = status::SUCCESS;
        Handle_Metadata* handle_metadata = nullptr;
        uint32_t file_size               = 0;

        status = validate_file_content_access(handle_metadata, handle, buffer);
        switch (status) {
        case status::NULL_POINTER | status::flags::PARAM_B:
            // The status flag `PARAM_B` is changed to `PARAM_A`, 
            // so that the returned status code matches the method signature.
            status = status::NULL_POINTER | status::flags::PARAM_A;
            break;

        case status::INVALID_PARAMETER | status::flags::PARAM_B:
            // The status flag `PARAM_B` is changed to `PARAM_A`, 
            // so that the returned status code matches the method signature.
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            break;

        case status::NULL_POINTER | status::flags::PARAM_C:
            // The status flag `PARAM_C` is changed to `PARAM_B`, 
            // so that the returned status code matches the method signature.
            status = status::NULL_POINTER | status::flags::PARAM_B;
            break;

        case status::SUCCESS:
            file_size = handle_metadata->header->used_byte_size;
            
            if (buffer_byte_size < file_size) {
                status = status::BUFFER_TOO_SMALL | status::flags::PARAM_C;
                break;
            }

            status = stdlib::memory::copy_block(buffer, 
                                                handle.buffer, 
                                                file_size);
            if (status != status::SUCCESS) [[unlikely]] {
                status = status::FAIL;
            }
            break;

        default:
            break;
        }

        return status;
    }


    /**
     * @brief Sets the file content.
     *
     * @param handle         Handle for the file to set the content of.
     * @param data           Pointer to the data for the new file content.
     * @param data_byte_size Byte size of the data buffer for the 
     *                       new file content.
     *
     * @retval `status::NOT_FOUND`
     *          If `handle` is not registered as open.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If internal `handle` pointers are invalid.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `handle` is invalid.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `data` is a `nullptr`.
     *
     * @retval `status::BUFFER_TOO_SMALL | status::flags::PARAM_C`
     *          If the file buffer is too small.
     *
     * @retval `status::FAIL`
     *          If an unexpected internal error occurred.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    [[nodiscard]]
    status_t
    set_file_content(_INOUT_ Handle& handle,
                     _IN_    const uint8_t* data,
                     _IN_    const uint32_t data_byte_size) noexcept {
        status_t status                  = status::SUCCESS;
        Handle_Metadata* handle_metadata = nullptr;

        status = validate_file_content_access(handle_metadata, handle, data);
        switch (status) {
        case status::NULL_POINTER | status::flags::PARAM_B:
            // The status flag `PARAM_B` is changed to `PARAM_A`, 
            // so that the returned status code matches the method signature.
            status = status::NULL_POINTER | status::flags::PARAM_A;
            break;

        case status::INVALID_PARAMETER | status::flags::PARAM_B:
            // The status flag `PARAM_B` is changed to `PARAM_A`, 
            // so that the returned status code matches the method signature.
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            break;

        case status::NULL_POINTER | status::flags::PARAM_C:
            // The status flag `PARAM_C` is changed to `PARAM_B`, 
            // so that the returned status code matches the method signature.
            status = status::NULL_POINTER | status::flags::PARAM_B;
            break;

        case status::SUCCESS:
            if (data_byte_size > handle.buffer_size) [[unlikely]] {
                status = status::BUFFER_TOO_SMALL | status::flags::PARAM_C;
                break;
            }

            status = stdlib::memory::set_block(handle.buffer, 
                                               0x00, 
                                               handle.buffer_size);
            if (status != status::SUCCESS) [[unlikely]] {
                status = status::FAIL;
                break;
            }

            if (data_byte_size > 0) {
                status = stdlib::memory::copy_block(handle.buffer, 
                                                    data, 
                                                    data_byte_size);
                if (status != status::SUCCESS) [[unlikely]] {
                    status = status::FAIL;
                    break;
                }
            }

            handle_metadata->header->used_byte_size = data_byte_size;
            handle.resume_cursor                    = handle.buffer;
            break;
        }

        return status;
    }


    /**
     * @brief Clears the file content.
     *
     * @param handle Handle for the file to clear the content of.
     *
     * @retval `status::NOT_FOUND`
     *          If `handle` is not registered as open.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If internal `handle` pointers are invalid.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If the handle is invalid.
     *
     * @retval `status::FAIL`
     *          If an unexpected internal error occurred.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    [[nodiscard]]
    status_t
    clear_file_content(_INOUT_ Handle& handle) noexcept {
        status_t status                  = status::SUCCESS;
        Handle_Metadata* handle_metadata = nullptr;

        status = get_open_handle(handle_metadata, handle);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (!handle_metadata->header) [[unlikely]] {
            sys::panic("File-System 'clear_file_content' invalid handle metadata!");
        }

        if (!handle.buffer || !handle.resume_cursor) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (handle.buffer_size == 0 ||
            handle.resume_cursor < handle.buffer ||
            handle.resume_cursor > 
            handle.buffer + handle.buffer_size) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        status = stdlib::memory::set_block(handle.buffer, 
                                           0x00, 
                                           handle.buffer_size);
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        status = stdlib::memory::set_block(handle_metadata->header->data, 
                                           0x00, 
                                           handle.buffer_size);
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        handle_metadata->header->used_byte_size = 0;
        handle.resume_cursor                    = handle.buffer;

    cleanup:
        return status;
    }


    /**
     * @brief Copies file content from one file to another.
     *
     * @param destination_handle Handle for the destination file.
     * @param source_handle      Handle for the source file.
     * @param offset             File content offset.
     * @param length             Number of bytes to copy.
     *
     * @retval `status::NOT_FOUND`
     *          If `destination_handle` or `source_handle` 
     *          is not registered as open.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If the destination handle pointers are invalid.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If internal `source_handle` pointers are invalid.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If internal `destination_handle` pointers are invalid.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If the `source_handle` is invalid.
     *
     * @retval `status::BUFFER_TOO_SMALL`
     *          If the internal destination buffer is too small.
     *
     * @retval `status::BUFFER_OVERFLOW`
     *          If the source range is outside the source file buffer.
     *
     * @retval `status::FAIL`
     *          If an unexpected internal error occurred.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    [[nodiscard]]
    status_t
    copy_file_content(_INOUT_ Handle& destination_handle,
                      _IN_    const Handle& source_handle,
                      _IN_    const uint32_t offset,
                      _IN_    const uint32_t byte_length) noexcept {
        status_t status                       = status::SUCCESS;
        Handle_Metadata* destination_metadata = nullptr;
        Handle_Metadata* source_metadata      = nullptr;
        uint32_t source_cursor_offset         = 0;
        uint32_t destination_cursor_offset    = 0;

        status = validate_open_file_state(destination_metadata, 
                                          destination_handle);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = validate_open_file_state(source_metadata, 
                                          source_handle);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        source_cursor_offset = static_cast<uint32_t>(source_handle.resume_cursor -
                                                     source_handle.buffer);

        if (source_cursor_offset >= source_handle.buffer_size ||
            offset > source_handle.buffer_size - source_cursor_offset ||
            byte_length > 
            source_handle.buffer_size - source_cursor_offset - offset) 
            [[unlikely]] {
            status = status::BUFFER_OVERFLOW;
            goto cleanup;
        }

        destination_cursor_offset = static_cast<uint32_t>(destination_handle.resume_cursor -
                                                          destination_handle.buffer);

        if (destination_cursor_offset >= destination_handle.buffer_size ||
            byte_length > 
            destination_handle.buffer_size - destination_cursor_offset) 
            [[unlikely]] {
            status = status::BUFFER_TOO_SMALL;
            goto cleanup;
        }

        status = stdlib::memory::copy_block(destination_handle.resume_cursor,
                                            source_handle.resume_cursor + offset,
                                            byte_length);
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        destination_metadata->header->used_byte_size = destination_cursor_offset + 
                                                       byte_length;

        destination_handle.resume_cursor = destination_handle.resume_cursor + 
                                           byte_length;

        source_metadata->header->resume_cursor = source_metadata->header->resume_cursor + 
                                                 offset + byte_length;

    cleanup:
        return status;
    }
} // namespace kernel::filesys
