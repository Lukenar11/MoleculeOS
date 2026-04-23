#ifndef HEAP_POS_MARKER_H
#define HEAP_POS_MARKER_H

#include <Runtime/C/stdint.h>

/*
    Note:
    I deliberately wrote this code in C instead of C++ to be maximally compatible 
    with the Linker Script. 
*/

#ifdef __cplusplus
extern "C" {
#endif
    
extern uint8_t heap_start;
extern uint8_t heap_end;

#ifdef __cplusplus
}
#endif

#endif // HEAP_POS_MARKER_H
