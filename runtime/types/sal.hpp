/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file defines a tiny 'Source Annotation Language' ('SAL'), 
    to mark in function/method paramater which is used as in or output
    because methods and functions returns 'void' or 'status_t'.

    SAL Explanation:
        - _IN_    => Input
        - _OUT_   => Output
        - _INOUT_ => Input & Output

NOTES:
*/

#pragma once

#define _IN_
#define _OUT_
#define _INOUT_
