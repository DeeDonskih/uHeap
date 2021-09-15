/**
 * @file uheap_new.cpp
 * @author Dmitry Donskikh (deedonskihdev@gmail.com)
 * @brief Overrides to new/delete operators with  uHeap manager
 * @version 0.1
 * @date 2019-11-04
 * 
 * Copyright © 2018-2021 Dmitriy Donskikh
 * All rights reserved.
 * 
 */

#include <./heap/uheap.h>

#include <cstddef>

#if (UHEAP_OVERRIDES_NEW == 1)

void* operator new(std::size_t size)
{
    return ufw::uHeap::instance().allocate(size);
}

void operator delete(void* ptr) noexcept { ufw::uHeap::instance().deallocate(ptr); }

void* operator new[](std::size_t size)
{
    return ufw::uHeap::instance().allocate(size);
}

void operator delete[](void* ptr) noexcept { ufw::uHeap::instance().deallocate(ptr); }

#endif  // UHEAP_OVERRIDES_NEW
