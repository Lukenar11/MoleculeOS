/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file implements helper structures for the storage manager.

    'MOFS_Header' begins at the first hard-drive sector and 
    includes meta infos for the file system.

    A 'Serialized_File_Header'-table contains the file_header infos which is fetched from the 
    file_headers in RAM.
    The Serialized_File_Header table begins in the second sector of the hard drive,
    immediately after the file-system header.

NOTES:

*/

#include "storemgr/helpers.hpp"
