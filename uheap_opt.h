/**
 * @file uheap_opt.h
 * @author Dmitry Donskikh (deedonskihdev@gmail.com)
 * @brief uHeap options header
 * @version 0.1
 * @date 2021-05-30
 * 
 * Copyright © 2018-2021 Dmitriy Donskikh
 * All rights reserved.
 * 
 */

// include guard
#ifndef __UHEAP_OPTIONS_H
    #define __UHEAP_OPTIONS_H

    /**
     * @def UHEAP_WARNINGS_ENABLE
     * @brief define this option to enable compile-time warnings
     */
    #ifndef UHEAP_WARNINGS_ENABLE
//        #define UHEAP_WARNINGS_ENABLE
    #endif
    /**
     * @def UHEAP_HEAP_SIZE
     * @brief Heap size in bytes
     */
    #ifndef UHEAP_HEAP_SIZE
        #define UHEAP_HEAP_SIZE (1024 * 1024)
    #endif

    /**
     * @def UHEAP_OVERRIDES_NEW
     * @brief If set to "1" overrides new/delete operators
     */
    #ifndef UHEAP_OVERRIDES_NEW
        #define UHEAP_OVERRIDES_NEW 0
    #endif

    /**
     * @def UHEAP_LOCK_TYPE
     * @brief Define your own "BasicLockable" object type
     */
//#ifdef __cplusplus
//    #include <mutex>
//    #define UHEAP_LOCK_TYPE std::mutex
//#endif
/**
 * @def UHEAP_WRAPS_MALLOC
 * @brief if defined wraps standart library malloc, calloc, realloc and free.
 * @note You must add "-Xlinker --wrap=malloc" linker option to use it as malloc
 */
//    #define UHEAP_WRAPS_MALLOC
    #ifndef UHEAP_WRAPS_MALLOC
        #ifdef UHEAP_WARNINGS_ENABLE
            #warning STDLIBC MALLOC
        #endif
    #else
        #ifdef UHEAP_WARNINGS_ENABLE
            #warning UHEAP MALLOC
        #endif
    #endif

    /**
     * @def UHEAP_USE_ERRNO
     * @brief Premission for using POSIX Error numbers and "errno.h"
     */
    #define UHEAP_USE_ERRNO

/**
 * @def UHEAP_SECTION
 * @brief Define memory section for your heap(GCC linker extention)
 *
 */
//    #define UHEAP_SECTION ".uheap"

/**
 * @def UHEAP_DEBUG
 * @brief Define your own debug output function (or UHEAP_USING_STDIO to use system printf)
 */
//    #define UHEAP_DEBUG(x) your_print_function(const char* x)

/**
 * @def UHEAP_ABORT
 * @brief Define your own abort() function (or UHEAP_USING_STDLIB to use system abort())
 */
//    #define UHEAP_ABORT(x)

    #ifndef UHEAP_DEBUG
        #define UHEAP_USING_STDIO
    #endif

    #ifndef UHEAP_ABORT
        #define UHEAP_USING_STDLIB
    #endif

    #ifdef UHEAP_USING_STDLIB
        #include "stdlib.h"
        #define UHEAP_ABORT(x) abort()
    #endif

    #ifdef UHEAP_USING_STDIO
        #ifdef UHEAP_WARNINGS_ENABLE
            #warning uHeap uses std printf as debug output
        #endif
        #include "stdio.h"
        #define UHEAP_DEBUG(x) printf(x)
    #endif

#endif
// include guard end
