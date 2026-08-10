/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file implements status-codes for handling general events and
    as alternative for exceptions, 
    because in bare mattel environments exceptions doesn't exists.

    The status-codes are grouped in a specific hex layout
    (0x[Grub]'[Flags][Flags]'[Number])(Each [...] represents one byte.).

    Grubs:
        - 0x00: generic status-codes
        - 0x01: pointer status-codes
        - 0x02: heap status-codes
        - 0x03: file system status-codes
        - 0x04: ata status-codes

NOTES:
*/

#pragma once

#include <types.hpp>

using status_t = uint32_t;

constexpr status_t STATUS_SUCCESS           = 0x00'0000'00;
constexpr status_t STATUS_FAIL              = 0x00'0000'01;
constexpr status_t STATUS_INVALID_PARAMETER = 0x00'0000'02;
constexpr status_t STATUS_NOT_FOUND         = 0x00'0000'03;
constexpr status_t STATUS_OUT_OF_MEMORY     = 0x00'0000'04;
constexpr status_t STATUS_IO_ERROR          = 0x00'0000'05;
constexpr status_t STATUS_END_OF_FILE       = 0x00'0000'06;

constexpr status_t STATUS_NULL_POINTER         = 0x01'0000'00;
constexpr status_t STATUS_INVALID_POINTER      = 0x01'0000'01;
constexpr status_t STATUS_UNALIGNED_POINTER    = 0x01'0000'02;
constexpr status_t STATUS_POINTER_OUT_OF_RANGE = 0x01'0000'03;

constexpr status_t STATUS_HEAP_CORRUPTED  = 0x02'0000'00;
constexpr status_t STATUS_HEAP_EXHAUSTED  = 0x02'0000'01;
constexpr status_t STATUS_ALIGNMENT_ERROR = 0x02'0000'02;

constexpr status_t STATUS_FS_CORRUPTED      = 0x03'0000'00;
constexpr status_t STATUS_FS_INVALID_INODE  = 0x03'0000'01;
constexpr status_t STATUS_FS_INVALID_BLOCK  = 0x03'0000'02;
constexpr status_t STATUS_FS_OUT_OF_SPACE   = 0x03'0000'03;

constexpr status_t STATUS_ATA_TIMEOUT      = 0x04'0000'00;
constexpr status_t STATUS_ATA_DEVICE_FAULT = 0x04'0000'01;
constexpr status_t STATUS_ATA_BAD_SECTOR   = 0x04'0000'02;
