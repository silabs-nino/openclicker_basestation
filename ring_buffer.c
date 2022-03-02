/***************************************************************************//**
 * @file
 * @brief Ring Buffer Implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 * # Experimental Quality
 * This code has not been formally tested and is provided as-is. It is not
 * suitable for production environments. In addition, this code will not be
 * maintained and there may be no bug maintenance planned for these resources.
 * Silicon Labs may update projects from time to time.
 ******************************************************************************/
#include <string.h>
#include <stdbool.h>
#include "sl_status.h"
#include "ring_buffer.h"

#define BUFFER_SIZE 10

#define CHECK_NULL(p)   {if(p == 0) return SL_STATUS_NULL_POINTER;}

static inline uint32_t  _ring_buffer_count( ring_buffer_handle_t* handle )
{
  return (handle->head - handle->tail);
}

static inline uint32_t  _ring_buffer_capacity( ring_buffer_handle_t* handle )
{
  return handle->capacity;
}

static inline bool      _ring_buffer_full( ring_buffer_handle_t* handle )
{
  return (_ring_buffer_count(handle) == _ring_buffer_capacity(handle));
}

static inline bool      _ring_buffer_empty( ring_buffer_handle_t* handle )
{
  return (handle->tail == handle->head);
}

static inline uint32_t _ring_buffer_mask( ring_buffer_handle_t* handle, uint32_t value)
{
  return value & (handle->capacity - 1);
}


// initialization
sl_status_t ring_buffer_init( ring_buffer_handle_t* handle )
{
  CHECK_NULL(handle);

  CHECK_NULL(handle->buffer);

  // reset head and tail
  handle->head = 0;
  handle->tail = 0;

  return SL_STATUS_OK;
}

// reset
sl_status_t ring_buffer_reset( ring_buffer_handle_t* handle )
{
  CHECK_NULL(handle);

  // this might be redundant, could just call init
  handle->head = 0;
  handle->tail = 0;

  return SL_STATUS_OK;
}

// add
sl_status_t ring_buffer_add( ring_buffer_handle_t* handle, void* data)
{
  uint32_t* src, dst;

  CHECK_NULL(handle);
  CHECK_NULL(data);

  if( _ring_buffer_full(handle) )
  {
      return SL_STATUS_FULL;
  }

  src = data;
  dst = handle->buffer[ _ring_buffer_mask(handle, handle->head++) ];

  // copy data to buffer @ head
  memcpy(dst, src, handle->size);

  return SL_STATUS_OK;
}

// get
sl_status_t ring_buffer_get( ring_buffer_handle_t* handle, void* data)
{
  uint32_t* src, dst;

  CHECK_NULL(handle);
  CHECK_NULL(data);

  if( _ring_buffer_empty(handle) )
  {
      return SL_STATUS_EMPTY;
  }

  src = handle->buffer[ _ring_buffer_mask(handle, handle->tail++) ];
  dst = data;

  // copy buffer to data
  memcpy(dst, src, handle->size);

  return SL_STATUS_OK;
}
