/**
 * @brief Implements kernel entry point and system main loop, 
 *        after the architecture dependent initialisation.
 *
 * @copyright Copyright (c) 2026 Lukenar11 (Luke Matthes)
 * @license:  https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
 * @author    Lukenar11 (Luke Matthes)
 *
 * @note The main function must not return, 
 *       leaving the main loop triggers a panic.
 */


#include <include/kernel/sys.hpp>
#include <include/kernel/heap.hpp>
#include <include/terminal/user.hpp>


_C_API_
extern "C" 
{ 
    extern uint8_t heap_start;
    extern uint8_t heap_end;
}


terminal::Terminal terminal;


namespace kernel
{                
    _API_
    [[noreturn]]
    void 
    main() noexcept {
        heap::init(&heap_start, &heap_end);

        while (true) {
            sys::sleep();
            terminal.step();
        }

        sys::panic("Unexpected return from the kernel-main loop!");
    }
}
