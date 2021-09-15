/*
 * _uheap_hooks.c - uHeap Default hook implementations
 *
 *  Created on: Jul 19, 2020
 *  Author: Donskikh Dmitriy
 *  All rights reserved
 */

#include "uheap_opt.h"

#ifndef UHEAP_DEBUG
    #ifdef CMSIS_DEVICE_HEADER_FILE
        #include CMSIS_DEVICE_HEADER_FILE
        #ifdef ITM
static void __itm_debug_output(const char* str)
{
    while (*str) ITM_SendChar(*str++);
}
            #define UHEAP_DEBUG(x)                   \
                __itm_debug_output("DEBUG uHeap: "); \
                __itm_debug_output(x)
        #else /* #ifdef ITM */
            #warning "uHeap debug output aren't defined"
            #define UHEAP_DEBUG(x)
        #endif                       /* #ifdef ITM */
    #elif defined(UHEAP_USING_STDIO) /* #ifdef CMSIS_DEVICE_HEADER_FILE */
        #include "stdio.h"
        #define UHEAP_DEBUG(x) printf("DEBUG uHeap: %s", x)
    #else
        #warning "uHeap debug output aren't defined"
        #define UHEAP_DEBUG(x)
    #endif
#endif /* #ifndef UHEAP_DEBUG */

/**
 * @fn void uHeapErrorHook()
 * @brief weak default implementation of heap error hook
 */
__attribute__((weak)) void uHeapErrorHook() { UHEAP_DEBUG("Heap error default hook!\n"); }
/**
 * @fn void uHeapFullHook()
 * @brief weak default implementation of full heap hook
 */
__attribute__((weak)) void uHeapFullHook() { UHEAP_DEBUG("Heap full default hook!\n"); }
