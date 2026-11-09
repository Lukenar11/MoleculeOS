#pragma once

#include <types.hpp>
#include <sal.hpp>
#include <status.hpp>


namespace kernel::filesys
{
    enum class Handle_Flags : uint8_t;

    
    /**
     * @brief Performs a bitwise AND on two handle flags.
     * 
     * @param flags           First handle flag.
     * @param additional_flag Second handle flag.
     * 
     * @return Bitwise AND result.
     */
    [[nodiscard]]
    inline
    constexpr
    Handle_Flags
    operator&(_IN_ Handle_Flags flags, 
              _IN_ Handle_Flags additional_flag) noexcept {
        return static_cast<Handle_Flags>(static_cast<uint8_t>(flags) &
                                         static_cast<uint8_t>(additional_flag));
    }


    /**
     * @brief Performs a bitwise OR on two handle flags.
     * 
     * @param flags           First handle flag.
     * @param additional_flag Second handle flag.
     * 
     * @return Bitwise OR result.
     */
    [[nodiscard]]
    inline
    constexpr
    Handle_Flags
    operator|(_IN_ Handle_Flags flags, 
              _IN_ Handle_Flags additional_flag) noexcept {
        return static_cast<Handle_Flags>(static_cast<uint8_t>(flags) |
                                         static_cast<uint8_t>(additional_flag));
    }


    /**
     * @brief Performs a bitwise XOR on two handle flags.
     * 
     * @param flags           First handle flag.
     * @param additional_flag Second handle flag.
     * 
     * @return Bitwise XOR result.
     */
    [[nodiscard]]
    inline
    constexpr
    Handle_Flags
    operator^(_IN_ Handle_Flags flags, 
              _IN_ Handle_Flags additional_flag) noexcept {
        return static_cast<Handle_Flags>(static_cast<uint8_t>(flags) ^
                                         static_cast<uint8_t>(additional_flag));
    }


    /**
     * @brief Describes file handle flags for access modes.
     */
    enum class Handle_Flags : uint8_t {
        READ     = 0x01,
        WRITE    = 0x02,
        BINARY   = 0x04,
        CREATE   = 0x08,
        APPEND   = 0x10,
        TRUNCATE = 0x20,
        EDIT     = 0x40
    };


    /**
     * @brief Stores the handle state of an opened file.
     * 
     * @note `flags` 
     *        is set to `Handle_Flags::WRITE | Handle_Flags::CREATE` by default.
     * 
     * @note `buffer_size` 
     *        is set to `0` by default.
     * 
     * @note `buffer` 
     *        is set to `nullptr` by default.
     * 
     * @note `resume_cursor` 
     *        is set to `nullptr` by default.
     */
    struct Handle final {
        Handle_Flags flags     = Handle_Flags::WRITE | Handle_Flags::CREATE;
        uint32_t buffer_size   = 0;
        uint8_t* buffer        = nullptr;
        uint8_t* resume_cursor = nullptr;
    };


    /** 
     * @brief Initializes the file system. 
     */
    _API_ 
    void 
    init() noexcept;


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
              _IN_  const Handle_Flags flags) noexcept;


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
    close_file(_INOUT_ Handle& handle) noexcept;


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
                _IN_ const Handle& handle) noexcept;


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
    remove_file(_INOUT_ Handle& handle) noexcept;


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
              _OUT_   uint8_t* buffer,
              _IN_    const uint32_t buffer_byte_size,
              _IN_    const uint32_t offset,
              _IN_    const uint32_t byte_length) noexcept;


    /**
     * @brief Writes data to a file.
     *
     * @param handle         Handle for the file to write to.
     * @param data           Input data to write.
     * @param data_byte_size Byte size of `data`.
     * @param offset         Resume cursor offset.
     * @param byte_length    Byte number to write.
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
               _IN_    const uint32_t byte_length) noexcept;


    /**
     * @brief Flushes a file stream (clears the internal stream buffer and
     *        resets the internal stream buffer handling).
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
    flush_stream(_INOUT_ Handle& handle) noexcept;


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
    flush_file(_IN_ const Handle& handle) noexcept;


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
     *          If `handle` is invalid.
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
                     _IN_    const uint32_t buffer_byte_size) noexcept;


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
     *          If `handle` is invalid.
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
                     _IN_    const uint8_t* buffer,
                     _IN_    const uint32_t buffer_byte_size) noexcept;


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
                  _IN_  const Handle& handle) noexcept;


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
                  _IN_ const uint32_t new_byte_size) noexcept;


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
    clear_file_content(_INOUT_ Handle& handle) noexcept;


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
                      _IN_    const uint32_t byte_length) noexcept;


    /**
     * @brief Gets the max allowed filename length.
     * 
     * @return Max allowed filename length.
     */
    _API_
    [[nodiscard]]
    inline
    constexpr
    uint32_t
    max_name_length() noexcept {
        return 10;
    }
    

    /**
     * @brief Gets the max allowed fileformat length.
     * 
     * @return Max allowed fileformat length.
     */
    _API_
    [[nodiscard]]
    inline
    constexpr
    uint32_t
    max_format_length() noexcept {
        return 4;
    }
} // namespace kernel::filesys
