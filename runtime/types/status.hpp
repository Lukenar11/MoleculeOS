/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file implements status-codes for handling general events and
    as an alternative for exceptions, 
    because in bare metal environments exceptions don't exists.

    The status-codes are grouped in a specific hex layout
    (0x[Grub]'[Flags][Flags]'[Number])(Each [...] represents one byte.).

    Grubs:
        - 0x00: generic status-codes
        - 0x01: comparison status-codes
        - 0x02: pointer status-codes
        - 0x03: heap status-codes
        - 0x04: file system status-codes
        - 0x05: ata status-codes

NOTES:
*/


#pragma once

#include <types.hpp>


using status_t = uint32_t;


namespace status
{
    constexpr status_t SUCCESS               = 0x00'0000'00;
    constexpr status_t FAIL                  = 0x00'0000'01;
    constexpr status_t INVALID_PARAMETER     = 0x00'0000'02;
    constexpr status_t NOT_FOUND             = 0x00'0000'03;
    constexpr status_t OUT_OF_MEMORY         = 0x00'0000'04;
    constexpr status_t IO_ERROR              = 0x00'0000'05;
    constexpr status_t END_OF_FILE           = 0x00'0000'06;
    constexpr status_t MISSING_PARAMETER     = 0x00'0000'07;
    constexpr status_t INVALID_STATE         = 0x00'0000'08;
    constexpr status_t BUFFER_TOO_SMALL      = 0x00'0000'09;
    constexpr status_t BUFFER_OVERFLOW       = 0x00'0000'0A;
    constexpr status_t BUFFER_UNDERFLOW      = 0x00'0000'0B;
    constexpr status_t UNSUPPORTED_OPERATION = 0x00'0000'0C;
    constexpr status_t NOT_IMPLEMENTED       = 0x00'0000'0D;

    constexpr status_t GREATER_THAN = 0x01'0000'00;
    constexpr status_t LESS_THAN    = 0x01'0000'01;
    constexpr status_t EQUAL_TO     = 0x01'0000'02;
    constexpr status_t FULL         = 0x01'0000'03;
    constexpr status_t EMPTY        = 0x01'0000'04;
    
    constexpr status_t NULL_POINTER         = 0x02'0000'00;
    constexpr status_t INVALID_POINTER      = 0x02'0000'01;
    constexpr status_t UNALIGNED_POINTER    = 0x02'0000'02;
    constexpr status_t POINTER_OUT_OF_RANGE = 0x02'0000'03;
    
    constexpr status_t HEAP_CORRUPTED  = 0x03'0000'00;
    constexpr status_t HEAP_EXHAUSTED  = 0x03'0000'01;
    constexpr status_t ALIGNMENT_ERROR = 0x03'0000'02;
    
    constexpr status_t FS_CORRUPTED           = 0x04'0000'00;
    constexpr status_t FS_INVALID_FILE_ENTRY  = 0x04'0000'01;
    constexpr status_t FS_INVALID_BLOCK       = 0x04'0000'02;
    constexpr status_t FS_OUT_OF_SPACE        = 0x04'0000'03;
    constexpr status_t FS_INVALID_INODE       = 0x04'0000'04;
    constexpr status_t ALREADY_EXISTS         = 0x04'0000'05;
    constexpr status_t FS_VERSION_MISMATCH    = 0x04'0000'06;
    
    constexpr status_t ATA_TIMEOUT              = 0x05'0000'00;
    constexpr status_t ATA_DEVICE_FAULT         = 0x05'0000'01;
    constexpr status_t ATA_BAD_SECTOR           = 0x05'0000'02;
    constexpr status_t ATA_NO_DEVICE            = 0x05'0000'03;
    constexpr status_t ATA_INVALID_SECTOR_COUNT = 0x05'0000'04;
    constexpr status_t ATA_INVALID_LBA          = 0x05'0000'05;
    constexpr status_t ATA_ERROR                = 0x05'0000'06;

    constexpr status_t PS2_SCANCODE_TO_BIG  = 0x06'0000'00;
    constexpr status_t PS2_KEYBOARD_RELEASE = 0x06'0000'01;
    constexpr status_t PS2_CAPS_LOCK        = 0x06'0000'02;
    constexpr status_t PS2_SHIFT            = 0x06'0000'03;
} // namespace status


namespace status::flags 
{
    constexpr status_t PARAM_A = 0x00'0001'00;
    constexpr status_t PARAM_B = 0x00'0002'00;
    constexpr status_t PARAM_C = 0x00'0004'00;
    constexpr status_t PARAM_D = 0x00'0004'00;
    
    constexpr status_t READ_ONLY    = 0x00'0010'00;
    constexpr status_t WRITE_ONLY   = 0x00'0020'00;
    constexpr status_t EXECUTE_ONLY = 0x00'0040'00;
    constexpr status_t UNALIGNED    = 0x00'0080'00;

    constexpr status_t OUT_OF_RANGE = 0x00'0100'00;
    constexpr status_t SIZE_ZERO    = 0x00'0200'00;
    constexpr status_t TOO_LARGE    = 0x00'0400'00;
    constexpr status_t OVERWRITTEN  = 0x00'0800'00;

    constexpr status_t IS_EMPTY    = 0x00'1000'00;
    constexpr status_t KEY_PRESS   = 0x00'2000'00;
    constexpr status_t KEY_RELEASE = 0x00'4000'00;
} // namespace status::flags
