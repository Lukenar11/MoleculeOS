/*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT Licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

    DESCRIPTION:
        This header defines a minimal, fixed-size array template.

        Unlike "std::array", this implementation avoids dependencies on the
        C++ standard library, which is unavailable in kernel mode.

    NOTES:
*/

#include <array.hpp>
