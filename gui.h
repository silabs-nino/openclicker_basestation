/***************************************************************************//**
 * @file
 * @brief GUI Header file for Remote Node
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

#ifndef GUI_H_
#define GUI_H_

#define TITLE_STR                 "OpenClicker Base"
#define TITLE_LINE                0
#define TITLE_OFFSET_X            0
#define TITLE_OFFSET_Y            1

#define THREAD_INFO_LINE          1
#define THREAD_INFO_OFFSET_X      2
#define THREAD_INFO_OFFSET_Y      4

#define LOG_LINE                  6
#define LOG_OFFSET_X              2
#define LOG_OFFSET_Y              0
#define LOG_BUFFER_LEN            4

#define ADDR_LINE                 10
#define ADDR_OFFSET_X             0
#define ADDR_OFFSET_Y             3

#define DISPLAY_LOG_MAX_STR_LEN   21

#include "glib.h"
#include "sl_button.h"

typedef struct {
  GLIB_Rectangle_t  rect;
  char              name;
} button_t;



void gui_init(void);
void gui_update(void);
void gui_button_handler(const sl_button_t *handle);
void gui_print_log(char *string);
void gui_print_network_name(const char *string);
void gui_print_network_channel(uint16_t ch);
void gui_print_device_role(const char *string);
void gui_print_mac_addr(char *mac_str);


#endif /* GUI_H_ */
