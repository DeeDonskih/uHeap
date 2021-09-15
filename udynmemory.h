/**
 * @file udynmemory.h
 * @author Dmitry Donskikh (deedonskihdev@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-05-15
 * 
 * Copyright © 2018-2021 Dmitriy Donskikh
 * All rights reserved.
 * 
 */

#ifndef ALLOCATION_UDYNMEMORY_H_
#define ALLOCATION_UDYNMEMORY_H_

#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @fn size_t getFreeHeap()
 * @brief returns count of free heap bytes
 *
 * @return
 */
size_t getFreeHeap();

#ifdef __cplusplus
}
#endif

#endif /* ALLOCATION_UDYNMEMORY_H_ */
