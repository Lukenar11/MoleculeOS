#ifndef HALT_SYSTEM_H
#define HALT_SYSTEM_H

/*
    Notes: 
        internal implementation in "HaltSystem.asm")

        I deliberately wrote this code in C instead of C++ 
        to be maximally compatible with the Assembly componentes.
*/

extern "C"
void HaltSystem(void);

#endif // HALT_SYSTEM_H
