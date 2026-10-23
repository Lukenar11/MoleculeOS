#pragma once

#include <sal.hpp>
#include <status.hpp>


namespace kernel 
{
    struct Registers;
}


namespace drivers::ps2
{
    template<typename Func>
    class Interface {
    public:
        /**
         * @brief Uninitializes the driver.
         * 
         * @note Is declared as default.
         */
        _API_
        Interface() noexcept = default;


        /**
         * @brief Uninitializes the driver.
         * 
         * @note Is declared as default.
         */
        _API_
        ~Interface() noexcept = default;


        /**
         * @brief Removes the entered keyboard key from the buffer.
         *
         * @param item The removed element.
         */
        _API_
        [[nodiscard]]
        static
        status_t
        pop_scancode_key(_OUT_ char& item) noexcept {
            return Func::pop_scancode_key(item);
        };


        /** 
         * @brief Pushed the pressed keyboard key into the scancode_buffer,
         *        if a  the interrupt event was triggered.
         * 
         * @note Do not use this in the normal code because 
         *       this method is only for the interrupts handling system.
         * 
         * @param register_dump dump of all CPU registers.
         */
        _API_ 
        static
        void 
        handle_keypress(_IN_ kernel::Registers* register_dump) noexcept {
            return Func::handle_keypress(register_dump);
        };
    };
} // namespace drivers::ps2
