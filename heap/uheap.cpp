/**
 * @file uheap.cpp
 * @author Dmitry Donskikh (deedonskihdev@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2018-11-29
 * 
 * Copyright (c) 2018-2021 Dmitriy Donskikh
 * All rights reserved.
 * 
 */

#include <heap/uheap.h>

#include <cerrno>

#ifdef UHEAP_SECTION
    #define __UHEAP_SECTION_INT __attribute__((section(UHEAP_SECTION)))
#else
    #define UHEAP_SECTION_INT
#endif

//#include "sys/itm.h"
//
//    #define _U_DEBUG_ALLOCATE(NEW,REM,MIN) \
//itmPuts("Allocated: ");itmPutN(NEW);itmPuts("bytes Remaining: ");itmPutN(REM);itmPuts(" Min:
// ");itmPutN(MIN);itmPuts("\n");\
//
//    #define _U_DEBUG_DEALLOCATE(REM,MIN) \
//itmPuts("MEM FREE! Remaining: ");itmPutN(REM);itmPuts(" Min: ");itmPutN(MIN);itmPuts("\n");\

#define U_DEBUG_DEALLOCATE(REM, MIN)
#define U_DEBUG_ALLOCATE(NEW, REM, MIN)

#define userheapASSERT(x)                       \
    if ((x) != true)                            \
    {                                           \
        UHEAP_DEBUG("Assertation failed at\n"); \
        UHEAP_DEBUG(__PRETTY_FUNCTION__);       \
        UHEAP_DEBUG("\n");                      \
        UHEAP_ABORT(-1);                        \
    }

extern "C" void uHeapErrorHook();
extern "C" void uHeapFullHook();

namespace ufw
{
    uHeap &uHeap::instance()
    {
        static uHeap s_instance UHEAP_SECTION_INT;
        return s_instance;
    }

    uHeap::uHeap()
    {
        /* Ensure the heap starts on a correctly aligned boundary. */
        size_t aligned_heap = allignBlock(reinterpret_cast<size_t>(heapBase));
        /* m_start is used to hold a pointer to the first item in the list of free blocks.*/
        m_start.nextFreeBlock = reinterpret_cast<uBlockLink *>(aligned_heap);
        m_start.blockSize = 0UL;

        /* m_endptr is used to mark the end of the list of free blocks and is inserted at the end of
         * the heap space. */
        m_endptr =
            reinterpret_cast<uBlockLink *>((end() - HeapStructSize) & (~BYTE_ALIGNMENT_MASK));
        m_endptr->blockSize = 0;
        m_endptr->nextFreeBlock = nullptr;

        /* To start with there is a single free block that is sized to take up the
         entire heap space, minus the space taken by pxEnd. */
        m_start.nextFreeBlock->blockSize = reinterpret_cast<size_t>(m_endptr) - aligned_heap;
        m_start.nextFreeBlock->nextFreeBlock = m_endptr;

        /* Only one block exists - and it covers the entire usable heap space. */
        m_memoryLowWatermark = m_start.nextFreeBlock->blockSize;
        m_freeBytesRemaining = m_start.nextFreeBlock->blockSize;
    }

    void uHeap::m_insertFreeBlock(uBlockLink *BlockToInsert)
    {
        uBlockLink *block_iterator = &m_start;

        /* Iterate through the list until a block is found that has a higher address
         than the block being inserted. */
        while (block_iterator->nextFreeBlock < BlockToInsert)
        {
            block_iterator = block_iterator->nextFreeBlock;
        }

        /* Do the block being inserted, and the block it is being inserted after
         make a contiguous block of memory? */
        //    uint8_t *puc = reinterpret_cast<uint8_t*>(block_iterator);
        if ((reinterpret_cast<uint8_t *>(block_iterator) + block_iterator->blockSize) ==
            reinterpret_cast<uint8_t *>(BlockToInsert))
        {
            block_iterator->blockSize += BlockToInsert->blockSize;
            BlockToInsert = block_iterator;
        }

        /* Do the block being inserted, and the block it is being inserted before
         make a contiguous block of memory? */
        //      puc = reinterpret_cast<uint8_t*>(BlockToInsert);
        if ((reinterpret_cast<uint8_t *>(BlockToInsert) + BlockToInsert->blockSize) ==
            reinterpret_cast<uint8_t *>(block_iterator->nextFreeBlock))
        {
            if (block_iterator->nextFreeBlock != m_endptr)
            {
                /* Form one big block from the two blocks. */
                BlockToInsert->blockSize += block_iterator->nextFreeBlock->blockSize;
                BlockToInsert->nextFreeBlock = block_iterator->nextFreeBlock->nextFreeBlock;
            } else
            {
                BlockToInsert->nextFreeBlock = m_endptr;
            }
        } else
        {
            BlockToInsert->nextFreeBlock = block_iterator->nextFreeBlock;
        }

        /* If the block being inserted plugged a gab, so was merged with the block
         before and the block after, then it's pxNextFreeBlock pointer will have
         already been set, and should not be set here as that would make it point
         to itself. */
        if (block_iterator != BlockToInsert) { block_iterator->nextFreeBlock = BlockToInsert; }
    }

    void *uHeap::m_malloc(size_t new_size)
    {
        uBlockLink *p_block;
        uBlockLink *p_previous_block;
        uBlockLink *p_new_block_link;
        void *p_return = nullptr;
        if (new_size == 0) { return nullptr; }
        if ((new_size > m_freeBytesRemaining) || (m_freeBytesRemaining < MINIMUM_BLOCK_SIZE))
        {
            heapFull();
            return nullptr;
        }
        // TODO: this scope must be in a Critical Section

        /* The wanted size is increased so it can contain a BlockLink_t
           structure in addition to the requested amount of bytes. */
        new_size += HeapStructSize;

        /* Ensure that blocks are always aligned to the required number of bytes. */
        if ((new_size & BYTE_ALIGNMENT_MASK) != 0x00)
        {
            /* Byte alignment required. */
            new_size += (BYTE_ALIGNMENT - (new_size & BYTE_ALIGNMENT_MASK));
            userheapASSERT((new_size & BYTE_ALIGNMENT_MASK) == 0);
        }

        if ((new_size > 0) && (new_size <= m_freeBytesRemaining))
        {
            /* Traverse the list from the start	(lowest address) block until
                 one	of adequate size is found. */
            p_previous_block = &m_start;
            p_block = m_start.nextFreeBlock;
            while ((p_block->blockSize < new_size) && (p_block->nextFreeBlock != nullptr))
            {
                p_previous_block = p_block;
                p_block = p_block->nextFreeBlock;
            }

            /* If the end marker was reached then a block of adequate size
                 was	not found. */
            if (p_block != m_endptr)
            {
                /* Return the memory space pointed to - jumping over the
                       BlockLink_t structure at its start. */
                p_return = reinterpret_cast<void *>(
                    (reinterpret_cast<uint8_t *>(p_previous_block->nextFreeBlock)) +
                    HeapStructSize);

                /* This block is being returned for use so must be taken out
                       of the list of free blocks. */
                p_previous_block->nextFreeBlock = p_block->nextFreeBlock;

                /* If the block is larger than required it can be split into
                       two. */
                if ((p_block->blockSize - new_size) > MINIMUM_BLOCK_SIZE)
                {
                    /* This block is to be split into two.  Create a new
                             block following the number of bytes requested. The void
                             cast is used to prevent byte alignment warnings from the
                             compiler. */
                    p_new_block_link = reinterpret_cast<uBlockLink *>(
                        (reinterpret_cast<uint8_t *>(p_block)) + new_size);
                    userheapASSERT((((size_t)p_new_block_link) & BYTE_ALIGNMENT_MASK) == 0);

                    /* Calculate the sizes of two blocks split from the
                             single block. */
                    p_new_block_link->blockSize = p_block->blockSize - new_size;
                    p_block->blockSize = new_size;

                    /* Insert the new block into the list of free blocks. */
                    m_insertFreeBlock(p_new_block_link);
                }

                m_freeBytesRemaining -= p_block->blockSize;

                if (m_freeBytesRemaining < m_memoryLowWatermark)
                {
                    m_memoryLowWatermark = m_freeBytesRemaining;
                }

                /* The block is being returned - it is allocated and owned
                       by the application and has no "next" block. */
                p_block->blockSize |= blockAllocatedBit;
                p_block->nextFreeBlock = nullptr;
            }
        }
        // TODO: this scope must be in a Critical Section
        userheapASSERT((((size_t)p_return) & (size_t)BYTE_ALIGNMENT_MASK) == 0);
        return p_return;
    }

    void uHeap::m_free(void *pv)
    {
        if (pv == nullptr) { return; }
        if (!isOwned(pv))
        {
            return;  // TODO(vader): Must cause "Not a heap"
        }

        /* The memory being freed will have an uBlockLink structure immediately
         before it. */
        uBlockLink *p_link =
            reinterpret_cast<uBlockLink *>(reinterpret_cast<uint8_t *>(pv) - HeapStructSize);

        /* Check the block is actually allocated. */
        if ((p_link->blockSize & blockAllocatedBit) == 0)
        {
            return;  // TODO: Must cause "double free or corrupted"
        }
        if (p_link->nextFreeBlock != nullptr)
        {
            return;  // TODO: Must cause "double free or corrupted"
        }

        /* The block is being returned to the heap - it is no longer
             allocated. */
        p_link->blockSize &= ~blockAllocatedBit;
        {
            /* Add this block to the list of free blocks. */
            m_freeBytesRemaining += p_link->blockSize;
            m_insertFreeBlock(p_link);
        }
    }

    const size_t& uHeap::getFreeBytesRemaining() const { return m_freeBytesRemaining; }

    const size_t& uHeap::getMemoryLowWatermark() const { return m_memoryLowWatermark; }

    void *uHeap::allocate(size_t new_size)
    {
        // LOCK (unlocked at scope exit)
        uGuard alloc_guard(m_lock);
        void *temp = m_malloc(new_size);

        U_DEBUG_ALLOCATE(new_size, m_freeBytesRemaining, m_memoryLowWatermark);
        return temp;
    }

    void uHeap::deallocate(void *pv)
    {
        // LOCK (unlocked at scope exit)
        uGuard dealloc_guard(m_lock);
        m_free(pv);

        U_DEBUG_DEALLOCATE(m_freeBytesRemaining, m_memoryLowWatermark);
    }

    void uHeap::heapError() { uHeapErrorHook(); }

    void uHeap::heapFull()
    {
        errno = ENOMEM;
        uHeapFullHook();
    }

    constexpr size_t uHeap::max_capacity() { return UHEAP_HEAP_SIZE; }

} /* namespace ufw */
