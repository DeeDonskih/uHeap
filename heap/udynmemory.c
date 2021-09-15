/**
 * @file udynmemory.c
 * @author Dmitry Donskikh (deedonskihdev@gmail.com)
 * @brief This file provides malloc series function using "uHeap" - dynamic memory management
 * @version 0.1
 * @date 2020-05-15
 * 
 * Copyright © 2018-2021 Dmitriy Donskikh
 * All rights reserved.
 * 
 */

/*
 * You must add "-Xlinker --wrap=malloc -Xlinker --wrap=_malloc_r" to linker options to use it.
 * For using newlib allocation functions define _UFW_USE_NEWLIB_MALLOC_
 */
#include "udynmemory.h"

#include "errno.h"
#include "heap/c_heap_wrapper.h"
#include "malloc.h"
#include "string.h"

#ifdef UHEAP_WRAPS_MALLOC

/**
 * @fn void __wrap_malloc*(size_t)
 * @brief overrides C library malloc with using uHeap
 * @note You must add "-Xlinker --wrap=malloc" linker option
 * @param size
 */
void *__wrap_malloc(size_t size) {
  return ufw_heap_alloc(size);
}

void free(void *ptr)
{
  ufw_heap_free(ptr);
}

void *calloc(size_t num, size_t size)
{
    void *temp = ufw_heap_alloc(num * size);
    memset(temp, 0x00, num * size);
    return temp;
}

void *realloc(void *ptr, size_t new_size)
{
    if (new_size == 0) { return NULL; }
    if (new_size > ufw_heap_getfreebytes())
    {
        errno = ENOMEM;
        return NULL;
    }
    void *temp = ufw_heap_alloc(new_size);
    if (!temp) { return NULL; }
    memcpy(temp, ptr, new_size);
    ufw_heap_free(ptr);
    return temp;
}

    #ifdef UHEAP_WRAPS_NEWLIB_MALLOC

/**
 * @fn void __wrap__malloc_r*(struct _reent*, size_t)
 * @brief overrides newlib's _malloc_r with using uHeap
 * @note You must add "-Xlinker --wrap=_malloc_r" linker option
 * @param reent
 * @param size
 */
extern void *__wrap__malloc_r(struct _reent *reent, size_t size) { return ufw_heap_alloc(size); }

void __wrap__free_r(struct _reent *reent, void *ptr) { free(ptr); }

void *__wrap__realloc_r(struct _reent *reent, void *ptr, size_t size) { return realloc(ptr, size); }

    /*
     *
     */
    #endif /* UHEAP_WRAPS_NEWLIB_MALLOC */

size_t getFreeHeap() { return ufw_heap_getfreebytes(); }

#endif /*UHEAP_WRAPS_MALLOC*/
