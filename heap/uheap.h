/**
 * @file uheap.h
 * @author Dmitry Donskikh (deedonskihdev@gmail.com)
 * @brief Dynamic memory management, implemented as singleton static linked object
 * @version 0.1
 * @date 2018-11-29
 * 
 * Copyright (c) 2018-2021 Dmitriy Donskikh
 * All rights reserved.
 * 
 */

/*
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "../uheap_opt.h"

#ifndef UHEAP_HEAP_SIZE
    #define UHEAP_HEAP_SIZE (120 * 1024)
#endif

#define UHEAP_FORCEINLINE inline __attribute__((always_inline))
#define UHEAP_INLINE_VISIBILITY __attribute__ ((__visibility__("hidden"), __always_inline__))

#include <cstddef>
#include <cstdint>

namespace ufw
{

    /**
     * @class uHeap
     * @brief "FreeRTOS heap4"-like dynamic memory management rewritten on C++ without
     * dependencies to OS.
     *
     */
    class uHeap
    {
#ifndef UHEAP_LOCK_TYPE
    #ifdef UHEAP_WARNINGS_ENABLE
    #warning You must provide your own lock
    #endif
        /**
         * @class uDummyLock - spinlock-alike lock
         */
        class uDummyLock
        {
           private:
            bool m_locked = false;

           public:
            void lock()
            {
                while (m_locked)
                    ;
                m_locked = true;
            }
            void unlock() { m_locked = false; }
        };
    #define UHEAP_LOCK_TYPE uDummyLock
#endif
        /**
         * @class uGuard<> - just a lock guard as in your stl
         * @tparam Lockable - the type of the lock. The type must meet the BasicLockable
         * requirements
         */
        template <typename Lockable = UHEAP_LOCK_TYPE>
        class uGuard
        {
           private:
            Lockable& m_lock_;
            uGuard(uGuard const&) = delete;
            uGuard& operator=(uGuard const&) = delete;

           public:
            UHEAP_INLINE_VISIBILITY
            explicit uGuard(Lockable& lock) : m_lock_(lock) { m_lock_.lock(); }
            UHEAP_INLINE_VISIBILITY
            ~uGuard() { m_lock_.unlock(); }
        };

        /**
         * @class BlockLink_t - forward linked list node of free memory blocks
         */
        struct uBlockLink
        {
            uBlockLink* nextFreeBlock = nullptr;
            size_t blockSize = 0;

            UHEAP_FORCEINLINE
            uint8_t* block() { return (uint8_t*)this + HeapStructSize; }
        };

        static constexpr size_t BITS_PER_BYTE = 8;
        /* Alignment settings */
        static constexpr size_t BYTE_ALIGNMENT = 16;
        static constexpr size_t BYTE_ALIGNMENT_MASK = BYTE_ALIGNMENT - 1;

        /* The size of the structure placed at the beginning of each allocated memory
         block must by correctly byte aligned. */
        static constexpr size_t HeapStructSize =
            (sizeof(uBlockLink) + ((size_t)(BYTE_ALIGNMENT - 1))) & ~((size_t)BYTE_ALIGNMENT_MASK);

        /* Gets set to the top bit of an size_t type.  When this bit in the xBlockSize
         member of an uBlockLink structure is set then the block belongs to the
         application.  When the bit is free the block is still part of the free heap
         space. */
        static constexpr size_t blockAllocatedBit = ((size_t)1) << ((sizeof(size_t) * 8) - 1);

        static constexpr size_t MINIMUM_BLOCK_SIZE = (HeapStructSize << 1);

       private:
        /* Raw heap array */
        uint8_t heapBase[UHEAP_HEAP_SIZE] = {};

        /* Links to mark the start and end of the list. */
        uBlockLink m_start{};
        uBlockLink* m_endptr = nullptr;

        /* Keeps track of the number of free bytes remaining, but says nothing about
         * fragmentation. */
        size_t m_freeBytesRemaining = 0UL;
        size_t m_memoryLowWatermark = 0UL;

        /* Interrnal lock */
        UHEAP_LOCK_TYPE m_lock{};

        /**
         * @brief uHeap - Constructor. Setup the required heap structures.
         */
        uHeap();

        /**
         * @brief heapError - called when error causes
         */
        void heapError();  // TODO: reimplement with error code

        /**
         * @brief heapFull - called when heap is full
         */
        void heapFull();

        /**
         * @brief m_insertFreeBlock
         * @param BlockToInsert
         * Inserts a block of memory that is being freed into the correct position in
         * the list of free memory blocks.  The block being freed will be merged with
         * the block in front it and/or the block behind it if the memory blocks are
         * adjacent to each other.
         */
        UHEAP_FORCEINLINE void m_insertFreeBlock(uBlockLink* BlockToInsert);
        // Internal malloc and free functions wrapped by public ones for debug
        // purposes
        UHEAP_FORCEINLINE void* m_malloc(size_t new_size);
        UHEAP_FORCEINLINE void m_free(void* pv);

       public:
        /**
         * @fn uHeapManager instance&()
         * @brief Return reference to a memory object. Instantiates it if invoked the
         * first time.
         */
        static uHeap& instance();
        /**
         * @fn void allocate*(size_t)
         * @brief Allocate number of bytes
         * @param new_size
         */
        void* allocate(size_t new_size);
        /**
         * @fn void deallocate(void*)
         * @brief Deallocate previousely allocated block
         * @param pv
         */
        void deallocate(void* pv);
        /**
         * @fn size_t getFreeBytesRemaining()
         * @brief Returns number of free bytes remaining
         */
        const size_t& getFreeBytesRemaining() const;
        /**
         * @fn size_t getMemoryLowWatermark()
         * @brief Returns the minimum ever number of free bytes.
         */
        const size_t& getMemoryLowWatermark() const;

        /**
         * @brief max_capacity
         * @return Capacity of heap
         */
        static constexpr size_t max_capacity();

       private:
        /* Rule of five */
        ~uHeap() = default;
        uHeap(const uHeap& other) = delete;
        uHeap(uHeap&& other) = delete;
        uHeap& operator=(const uHeap& other) = delete;
        uHeap& operator=(uHeap&& other) = delete;
        /* End rule of five */

        /* Inlines */
        /**
         * @brief begin - pointer to heap (raw)
         * @return
         */
        UHEAP_FORCEINLINE size_t begin() { return reinterpret_cast<size_t>(heapBase); }
        /**
         * @brief end - pointer to heap end (raw)
         * @return
         */
        UHEAP_FORCEINLINE size_t end()
        {
            return reinterpret_cast<size_t>(&heapBase[UHEAP_HEAP_SIZE]);
        }

        /**
         * @fn bool isOwned(uint8_t*) - Checks, is the given pointer lays in heap
         * address space?
         * @param ptr
         * @return
         */
        UHEAP_FORCEINLINE bool isOwned(void* ptr)
        {
            return ((ptr >= heapBase) && (ptr < heapBase + UHEAP_HEAP_SIZE));
        }

        UHEAP_FORCEINLINE size_t allignBlock(size_t block)
        {
            return ((block + BYTE_ALIGNMENT_MASK) & (~BYTE_ALIGNMENT_MASK));
        }
        /* End inlines */
    };

} /* namespace heap */
