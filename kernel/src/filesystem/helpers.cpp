/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains all meta-informations and 
    the file structure about the file system.

NOTES:
    Each file-structure represents a file with meta infos, the file contains the
    file name, format, the data, the size and a using info.

    An array of file instances is created from the file structure,
    which then represents all the files.
*/

#include "filesystem/helpers.hpp"
