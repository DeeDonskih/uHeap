/**
 * @file uheap_allocator.h
 * @author Dmitry Donskikh (deedonskihdev@gmail.com)
 * @brief stl-compatible allocator for containers
 * @version 0.1
 * @date 2019-11-03
 * 
 * Copyright © 2018-2021 Dmitriy Donskikh
 * All rights reserved.
 * 
 */

#ifndef UFWALLOCATOR_H
#define UFWALLOCATOR_H

#include <./heap/uheap.h>

#include <cstddef>

#define PLATFORM_MEM_VALID (ufw::uHeap::instance().getFreeBytesRemaining() > n)
#define PLATFORM_MEM_ALLOC(size, type) ufw::uHeap::instance().allocate(size * sizeof(type))
#define PLATFORM_MEM_DEALLOC(size, obj_ptr) ufw::uHeap::instance().deallocate(obj_ptr)
static void _do_nothing(){}; /* placeholder */
#define PLATFORM_MEM_EXCEPTION _do_nothing()

/**
 * @brief stl-compatible allocator
 * @tparam T
 */
template <class T>
class uHeapAllocator
{
   public:
    typedef T value_type;
    uHeapAllocator() noexcept = default;

    template <class U>
    constexpr uHeapAllocator(const uHeapAllocator<U>&) noexcept
    {
    }

    T* allocate(size_t n) noexcept
    {
        if (n == 0) return nullptr;
        if (!PLATFORM_MEM_VALID)
        {
            PLATFORM_MEM_EXCEPTION;
            return nullptr;
        }
        if (auto p = static_cast<T*>(PLATFORM_MEM_ALLOC(n, T))) { return p; }
        PLATFORM_MEM_EXCEPTION;
        return nullptr;
    }

    void deallocate(T* p, size_t n) noexcept
    {
        if (p) PLATFORM_MEM_DEALLOC(n, p);
    }
};

template <class T, class U>
bool operator==(const uHeapAllocator<T>&, const uHeapAllocator<U>&)
{
    return true;
}
template <class T, class U>
bool operator!=(const uHeapAllocator<T>&, const uHeapAllocator<U>&)
{
    return false;
}

#endif  // UFWALLOCATOR_H
