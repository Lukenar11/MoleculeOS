/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file implements helper structures for the storage manager.

    'Filesys_Header' begins at the first hard-drive sector and 
    includes meta informations for the file system.

    A 'Serialized_I_Node'-table contains the inode infos which is fetched from the 
    inodes in the RAM.
    The Serialized_I_Node table begins in the second sector of the hard drive,
    immediately after the file-system header.

NOTES:

*/

#include "storagemgr/helpers.hpp"
