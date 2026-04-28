#ifndef LOAD_IDT_H
#define LOAD_IDT_H

/*
    Notes: 
        internal implementation in "LoadIDT.asm")

        I deliberately wrote this code in C instead of C++ 
        to be maximally compatible with the Assembly componentes.
*/

#ifdef __cplusplus
extern "C" {
#endif

void LoadIDT(uint32_t);

#ifdef __cplusplus
}
#endif

#endif // LOAD_IDT_H
