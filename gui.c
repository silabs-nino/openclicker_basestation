/***************************************************************************//**
 * @file
 * @brief GUI for the Remote Node
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

// display drivers and graphics library
#include "glib.h"
#include "dmd.h"

// platform includes
#include "sl_simple_button_instances.h"
#include "printf.h"

#include "gui.h"
#include "gui_event_queue.h"

// local functions
static  void display_init(void);
static  void draw_button(const button_t* button, bool pressed);

// local vars
static  char                     log_buffer[LOG_BUFFER_LEN][DISPLAY_LOG_MAX_STR_LEN + 1];
static  uint8_t                  log_index;
static  const GLIB_Rectangle_t   log_window = {0, 55, 127, 98};

static  GLIB_Context_t           glib_context;
static  bool                     update_display;

static  const button_t           button_left     = {{ 1, 113,  62, 126}, 'A'};
static  const button_t           button_right    = {{65, 113, 126, 126}, 'B'};


static void display_init(void)
{
  // initialize dot matrix display driver
  DMD_init(0);

  // get glib handle
  GLIB_contextInit(&glib_context);

  // set colors
  glib_context.backgroundColor  = White;
  glib_context.foregroundColor  = Black;

  // set font
  glib_context.font             = GLIB_FontNarrow6x8;

  // clear display
  GLIB_clear(&glib_context);

  // mark display update needed
  update_display = true;
}

static void draw_button(const button_t* button, bool pressed)
{
  int32_t char_x, char_y;

  // only modify button area
  GLIB_setClippingRegion(&glib_context, &button->rect);

  // clear button area
  GLIB_clearRegion(&glib_context);

  GLIB_resetClippingRegion(&glib_context);
  GLIB_resetDisplayClippingArea(&glib_context);

  // we want to center text in button
  // drawChar expects (x,y) of upper left corner
  // find the center of area then offset by half font width & height
  char_x = (button->rect.xMax + button->rect.xMin) / 2;
  char_x -= glib_context.font.fontWidth / 2;

  char_y = (button->rect.yMax + button->rect.yMin) / 2;
  char_y -= glib_context.font.fontHeight / 2;
  char_y += 2;

  if(pressed) {
      // dark button looks like it's pressed :)
      GLIB_drawRectFilled(&glib_context, &button->rect);
      // set color to contrast dark background
      glib_context.foregroundColor = White;
  } else {
      // draw outlined box
      GLIB_drawRect(&glib_context, &button->rect);
  }

  GLIB_drawChar(&glib_context, button->name, char_x, char_y, false);
  glib_context.foregroundColor = Black;

  // mark display update needed
  update_display = true;
}

void gui_init(void)
{
  log_index = 0;

  // initialize event queue
  gui_event_queue_init();

  // initialize GLIB handler
  display_init();

  // title header
  GLIB_drawStringOnLine(&glib_context, TITLE_STR, TITLE_LINE, GLIB_ALIGN_CENTER,
                        TITLE_OFFSET_X, TITLE_OFFSET_Y, false);

  // title divider
  GLIB_drawLineH(&glib_context, 0,  9, 127);
  GLIB_drawLineH(&glib_context, 0, 11, 127);

  // log divider
  GLIB_drawLineH(&glib_context, 0, 54, 127);

  // IP addr divider
  GLIB_drawLineH(&glib_context, 0,  99, 127);
  GLIB_drawLineH(&glib_context, 0, 101, 127);

  // button divider
  GLIB_drawLineH(&glib_context, 0, 111, 127);

  draw_button(&button_left,  false);
  draw_button(&button_right, false);

  // mark display update needed
  update_display = true;
}

void gui_update(void)
{
  sl_status_t status;
  gui_event_t event;

  // read events from the queue
  do {
      status = ring_buffer_get(&gui_event_queue, &event);

      if(status != SL_STATUS_OK)
      {
          break;
      }

      printf("\tflag: %u, msg: %s\r\n", event.flag, event.msg);

      switch(event.flag) {
        case GUI_EVENT_FLAG_BTN0_PRESSED:
          draw_button(&button_right, true);
          break;

        case GUI_EVENT_FLAG_BTN0_RELEASED:
          draw_button(&button_right, false);
          break;

        case GUI_EVENT_FLAG_BTN1_PRESSED:
          draw_button(&button_left, true);
          break;

        case GUI_EVENT_FLAG_BTN1_RELEASED:
          draw_button(&button_left, false);
          break;

        case GUI_EVENT_FLAG_NTWK_NAME:
          gui_print_network_name((char *)&event.msg);
          break;

        case GUI_EVENT_FLAG_NTWK_CH:
          gui_print_network_channel((char *)&event.msg);
          break;

        case GUI_EVENT_FLAG_NTWK_ADDR:
          gui_print_mac_addr((char *)&event.msg);
          break;

        case GUI_EVENT_FLAG_NTWK_ROLE:
          gui_print_device_role((char *)&event.msg);
          break;

        case GUI_EVENT_FLAG_LOG:
          gui_print_log((char *)&event.msg);
          break;

        default:
          break;
      }

  } while (status == SL_STATUS_OK);

  // only update when needed
  if(update_display)
  {
      DMD_updateDisplay();
      update_display = false;
  }
}

void gui_button_handler(const sl_button_t *handle)
{
  gui_event_t event = {
      .flag = 0,
      .msg  = {0},
  };

  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (&sl_button_btn0 == handle) {
        event.flag = GUI_EVENT_FLAG_BTN0_PRESSED;
        ring_buffer_add(&gui_event_queue, &event);
    }

    if (&sl_button_btn1 == handle) {
        event.flag = GUI_EVENT_FLAG_BTN1_PRESSED;
        ring_buffer_add(&gui_event_queue, &event);
    }
  }
  else if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_RELEASED) {
    if (&sl_button_btn0 == handle) {
        event.flag = GUI_EVENT_FLAG_BTN0_RELEASED;
        ring_buffer_add(&gui_event_queue, &event);
    }

    if (&sl_button_btn1 == handle) {
        event.flag = GUI_EVENT_FLAG_BTN1_RELEASED;
        ring_buffer_add(&gui_event_queue, &event);
    }
  }
}

void gui_print_log(char *string)
{
  uint8_t temp_ind;

  // add entry to log buffer
  strncpy((char *)&log_buffer[log_index], string, DISPLAY_LOG_MAX_STR_LEN);

  if(strlen(string) > DISPLAY_LOG_MAX_STR_LEN)
  {
      // mark last as null
      // mark second to last as asterisk
      log_buffer[log_index][DISPLAY_LOG_MAX_STR_LEN - 1] = '*';
  }

  // mark last char as empty in the case that string is longer than DISPLAY_LOG_MAX_STR_LEN
  log_buffer[log_index][DISPLAY_LOG_MAX_STR_LEN] = '\0';

  // clear log area
  GLIB_setClippingRegion(&glib_context, &log_window);
  GLIB_clearRegion(&glib_context);

  GLIB_resetClippingRegion(&glib_context);
  GLIB_resetDisplayClippingArea(&glib_context);

  // reverse print the log buffer to the display
  temp_ind = log_index;
  for(int8_t x = LOG_BUFFER_LEN - 1; x >= 0; x--)
  {
      GLIB_drawStringOnLine(&glib_context, (const char*) &log_buffer[temp_ind],
                            LOG_LINE + x, GLIB_ALIGN_LEFT,
                            LOG_OFFSET_X, LOG_OFFSET_Y,
                            false);

      temp_ind = (temp_ind == 0) ? 3 : temp_ind - 1;
  }

  // increment and loop around log index
  log_index = (log_index + 1) % LOG_BUFFER_LEN;

  // mark display update needed
  update_display = true;
}

void gui_print_network_name(char *string)
{
  char temp[20];

  // blank line
  memset(&temp, ' ', 20);
  temp[19] = '\0';

  GLIB_drawStringOnLine(&glib_context, temp,
                            THREAD_INFO_LINE, GLIB_ALIGN_LEFT,
                            THREAD_INFO_OFFSET_X, THREAD_INFO_OFFSET_Y,
                            true);

  // print network name
  snprintf((char *)&temp, 20, "name:  %s", string);
  temp[19] = '\0';

  GLIB_drawStringOnLine(&glib_context, temp,
                          THREAD_INFO_LINE, GLIB_ALIGN_LEFT,
                          THREAD_INFO_OFFSET_X, THREAD_INFO_OFFSET_Y,
                          false);

  // mark display update needed
  update_display = true;
}

void gui_print_network_channel(char *ch)
{
  char temp[20];

  // blank line
  memset(&temp, ' ', 20);
  temp[19] = '\0';

  GLIB_drawStringOnLine(&glib_context, temp,
                            THREAD_INFO_LINE + 1, GLIB_ALIGN_LEFT,
                            THREAD_INFO_OFFSET_X, THREAD_INFO_OFFSET_Y,
                            true);

  // print thread channel
  snprintf((char *)&temp, 20, "ch:    %s", ch);
  temp[19] = '\0';

  GLIB_drawStringOnLine(&glib_context, temp,
                          THREAD_INFO_LINE + 1, GLIB_ALIGN_LEFT,
                          THREAD_INFO_OFFSET_X, THREAD_INFO_OFFSET_Y,
                          false);

  // mark display update needed
  update_display = true;
}

void gui_print_device_role(char *string)
{
  char temp[20];

  // blank line
  memset(&temp, ' ', 20);
  temp[19] = '\0';

  GLIB_drawStringOnLine(&glib_context, temp,
                            THREAD_INFO_LINE + 2, GLIB_ALIGN_LEFT,
                            THREAD_INFO_OFFSET_X, THREAD_INFO_OFFSET_Y,
                            true);

  // print device state
  snprintf((char *)&temp, 20, "state: %s", string);
  temp[19] = '\0';

  GLIB_drawStringOnLine(&glib_context, temp,
                          THREAD_INFO_LINE + 2, GLIB_ALIGN_LEFT,
                          THREAD_INFO_OFFSET_X, THREAD_INFO_OFFSET_Y,
                          false);

  // mark display update needed
  update_display = true;
}

void gui_print_mac_addr(char *mac_addr)
{
  char temp[20];

  // blank line
  memset(&temp, ' ', 20);
  temp[19] = '\0';

  GLIB_drawStringOnLine(&glib_context, temp,
                          ADDR_LINE, GLIB_ALIGN_CENTER,
                          ADDR_OFFSET_X, ADDR_OFFSET_Y,
                          true);

  // print mac address
  GLIB_drawStringOnLine(&glib_context, mac_addr,
                          ADDR_LINE, GLIB_ALIGN_CENTER,
                          ADDR_OFFSET_X, ADDR_OFFSET_Y,
                          false);

  // mark display update needed
  update_display = true;
}


