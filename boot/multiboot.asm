;
; LICENSE:
;     Copyright (c) 2026 Lukenar11 (Luke Matthes)
;     MIT Licensed
;     https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
;
; DESCRIPTION:
;     This file defines the Multiboot header required by Multiboot‑compliant
;     bootloaders such as GRUB. 
;        
;     The header contains the magic value, flags,
;     and checksum that allow the bootloader to recognize and load the
;     kernel binary correctly.
;
;     The Multiboot header must be placed within the first 8 KiB of the
;     kernel image and aligned to a 4‑byte boundary to satisfy the
;     Multiboot specification.
;
; NOTES:
;     The checksum is computed such that:
;     magic + flags + checksum == 0 (mod 2^32)
;

section .multiboot
align 4

MB_MAGIC equ 0x1BADB002
MB_FLAGS equ 0x00000003
MB_CHECKSUM equ -(MB_MAGIC + MB_FLAGS)

dd MB_MAGIC
dd MB_FLAGS
dd MB_CHECKSUM
