#ifndef HALT_SYSTEM_H
#define HALT_SYSTEM_H

/*
    Notes: 
        internal implementation in "HaltSystem.asm")

        I deliberately wrote this code in C instead of C++ 
        to be maximally compatible with the Assembly componentes.
*/

#ifdef __cplusplus
extern "C" {
#endif

void HaltSystem(void);

#ifdef __cplusplus
}
#endif

#endif // HALT_SYSTEM_H
