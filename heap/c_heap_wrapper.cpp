/*
 * c_heap_wrapper.cpp
 *
 *  Created on: Jul 15, 2021
 *      Author: Donskikh Dmitry
 *	All rights reserved
 */

#include "c_heap_wrapper.h"
#include <heap/uheap.h>

void* ufw_heap_alloc (size_t size)
{
  return ufw::uHeap::instance().allocate(size);
}

void ufw_heap_free (void *ptr)
{
  ufw::uHeap::instance().deallocate(ptr);
}

size_t ufw_heap_getfreebytes ()
{
  return ufw::uHeap::instance().getFreeBytesRemaining();
}


