/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains all meta-informations and 
    the inode structure about the file system.

NOTES:
    Eatch inode represents a file with meta infos,
    the inode contains the file name, frmat, the data, the size and a using info.

    An array of inode instances is created from the inode structure, 
    which then represents all the files.

    "Inode" is an abbreviation that stands for "Inodex Node", 
    but I use the abbreviation because it is the common term and the full term is rarely used.
*/

#include "helpers.hpp"
