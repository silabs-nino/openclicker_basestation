/***************************************************************************//**
 * @file
 * @brief GUI Event Queue
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

#include "ring_buffer.h"
#include "gui_event_queue.h"

#define EVENT_QUEUE_BUFFER_SIZE 16

static gui_event_t gui_events[EVENT_QUEUE_BUFFER_SIZE];
static void*       buffer[EVENT_QUEUE_BUFFER_SIZE];

ring_buffer_handle_t  gui_event_queue = {
    .buffer   = &buffer,
    .head     = 0,
    .tail     = 0,
    .size     = sizeof(gui_event_t),
    .capacity = EVENT_QUEUE_BUFFER_SIZE,
};

sl_status_t gui_event_queue_init(void)
{
  sl_status_t error = SL_STATUS_OK;

  for(uint32_t i = 0; i < EVENT_QUEUE_BUFFER_SIZE; i++)
  {
      buffer[i] = &gui_events[i];
  }

  error = ring_buffer_init(&gui_event_queue);

  return error;
}
