/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file defines a small 'Source Annotation Language' ('SAL')
    to mark function/method parameters as input or output, 
    since methods and functions return 'void' or 'status_t'.

    SAL Explanation:
        - _IN_    => Input
        - _OUT_   => Output
        - _INOUT_ => input => 2. output
        - _API_   => public method

NOTES:
*/


#pragma once

#define _IN_
#define _OUT_
#define _INOUT_
#define _API_
