/***************************************************************************//**
 * @file
 * @brief GUI Event Queue Header
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
#ifndef GUI_EVENT_QUEUE_H_
#define GUI_EVENT_QUEUE_H_

#include "ring_buffer.h"

#define GUI_EVENT_MSG_SIZE              32u

#define GUI_EVENT_FLAG_BTN0_PRESSED     (1 << 0)   // draw button right, true
#define GUI_EVENT_FLAG_BTN0_RELEASED    (1 << 1)   // draw button right, false

#define GUI_EVENT_FLAG_BTN1_PRESSED     (1 << 2)   // draw button left, true
#define GUI_EVENT_FLAG_BTN1_RELEASED    (1 << 3)   // draw button left, false

#define GUI_EVENT_FLAG_NTWK_NAME        (1 << 4)
#define GUI_EVENT_FLAG_NTWK_CH          (1 << 5)
#define GUI_EVENT_FLAG_NTWK_ADDR        (1 << 6)
#define GUI_EVENT_FLAG_NTWK_ROLE        (1 << 7)

#define GUI_EVENT_FLAG_LOG              (1 << 8)

typedef struct {
  uint32_t  flag;
  char      msg[GUI_EVENT_MSG_SIZE];
} gui_event_t;

extern ring_buffer_handle_t gui_event_queue;

sl_status_t gui_event_queue_init(void);

#endif /* GUI_EVENT_QUEUE_H_ */
