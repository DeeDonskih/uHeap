/*
 *  c_heap_wrapper.h
 *
 *  Created on: Jul 15, 2021
 *      Author: Donskikh Dmitry
 *	All rights reserved
 */

#ifndef ALLOCATION_HEAP_C_HEAP_WRAPPER_H_
#define ALLOCATION_HEAP_C_HEAP_WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif

// Standart C Library headers
#include "stddef.h"
#include "stdint.h"
#include "stdlib.h"
//
/**
 * @fn void uheap_alloc*(size_t)
 * @brief C-wrapper for uHeap::alloc(size)
 * @param size
 */
void* ufw_heap_alloc(size_t size);
/**
 * @fn void ufw_heap_free(void*)
 * @brief C-wrapper for uHeap::free(ptr)
 * @param ptr
 */
void ufw_heap_free(void* ptr);
/**
 * @fn size_t uheap_getfreebytes()
 * @brief C-wrapper for uHeap::getFreeBytes()
 *
 * @return number of free heap bytes
 */
size_t ufw_heap_getfreebytes();

#ifdef __cplusplus
}
#endif

#endif /* ALLOCATION_HEAP_C_HEAP_WRAPPER_H_ */
