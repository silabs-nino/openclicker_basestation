/***************************************************************************//**
 * @file
 * @brief Core Application Logic for the Base Station Node
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

// OpenThread Includes
#include <openthread/dataset_ftd.h>
#include <openthread/thread_ftd.h>

// Platform Drivers
#include "sl_button.h"
#include "sl_simple_button.h"
#include "sl_simple_button_instances.h"

// Utilities
#include "printf.h"

// Config
#include "base_station_config.h"

// Coap
#include "coap_server.h"

// GUI
#include "gui.h"



// declaring functions
static void openthread_event_handler(otChangedFlags event, void *aContext);
void joiner_callback(otCommissionerJoinerEvent aEvent, const otJoinerInfo *aJoinerInfo, const otExtAddress *aJoinerId, void *aContext);
void commissioner_callback(otCommissionerState aState, void *aContext);

static otOperationalDataset sDataset;
static otInstance*          sInstance = NULL;

/**************************************************************************//**
 * Base Station Init
 *
 * @param otInstance - pointer to openthread instance
 *****************************************************************************/
void base_station_init(otInstance* instance)
{
  otError error;

  // set openthread instance
  sInstance = instance;

  printf("Hello from the base station app_init\r\n");

  // register callback for Thread Stack Events
  error = otSetStateChangedCallback(sInstance, openthread_event_handler, (void *)sInstance);
  printf("registering event callback: %s\r\n", otThreadErrorToString(error));

  // create a new dataset
  error = otDatasetCreateNewNetwork(sInstance, &sDataset);
  printf("creating new dataset: %s\r\n", otThreadErrorToString(error));

  // change dataset network name to OPENTHREAD_NETWORK_NAME
  error = otNetworkNameFromString(&(sDataset.mNetworkName), OPENTHREAD_NETWORK_NAME);
  printf("setting network name to OpenClicker: %s\r\n", otThreadErrorToString(error));

  // set active dataset
  error = otDatasetSetActive(sInstance, &sDataset);
  printf("setting active dataset: %s\r\n", otThreadErrorToString(error));

  // enable interface
  error = otIp6SetEnabled(sInstance, true);
  printf("enabling interface: %s\r\n", otThreadErrorToString(error));

  // start thread radio stack
  error = otThreadSetEnabled(sInstance, true);
  printf("starting thread: %s\r\n", otThreadErrorToString(error));
}

/**************************************************************************//**
 * OpenThread Event Handler
 *
 * @param event - flags
 * @param aContext - openthread context
 *****************************************************************************/
static void openthread_event_handler(otChangedFlags event, void *aContext)
{
  otError error;

  if(event & OT_CHANGED_ACTIVE_DATASET)
  {
      printf("Active Dataset Changed\r\n");
  }

  if(event & OT_CHANGED_THREAD_NETDATA)
  {
      otOperationalDataset otDataset;
      error = otDatasetGetActive(aContext, &otDataset);
      if(!error)
      {
          gui_print_network_channel(otDataset.mChannel);
      }

      printf("Network Dataset Changed\r\n");
  }

  if(event & OT_CHANGED_THREAD_NETWORK_NAME)
  {
      printf("network name changed: %s\r\n", otThreadGetNetworkName(aContext));
      gui_print_network_name(otThreadGetNetworkName(aContext));
  }

  if(event & OT_CHANGED_THREAD_NETIF_STATE)
  {
      printf("Network Interface State Changed: %d\r\n", otIp6IsEnabled(aContext));
  }

  if(event & OT_CHANGED_THREAD_ROLE)
  {
      printf("Thread Device Role Changed: %s\r\n", otThreadDeviceRoleToString(otThreadGetDeviceRole(aContext)));

      gui_print_device_role(otThreadDeviceRoleToString(otThreadGetDeviceRole(aContext)));

      if(otThreadGetDeviceRole(aContext) == OT_DEVICE_ROLE_LEADER)
      {
          // start commissioner
          error = otCommissionerStart(aContext, commissioner_callback, joiner_callback, (void*)aContext);
          printf("commissioner start: %s\r\n", otThreadErrorToString(error));

          // start coap server
          coap_server_init(aContext);

          gui_print_log("[coap] start");
          gui_print_log("press 'B' to start");
      }
  }

  if(event & OT_CHANGED_JOINER_STATE)
  {
      printf("Thread Joiner State Changed: %d\r\n", otJoinerGetState(aContext));
  }

}

/**************************************************************************//**
 * Commissioner Joiner Callback Handler
 *****************************************************************************/
void joiner_callback(otCommissionerJoinerEvent aEvent, const otJoinerInfo *aJoinerInfo, const otExtAddress *aJoinerId, void *aContext)
{
  (void)aContext;
  (void)aJoinerInfo;
  (void)aJoinerId;

  const char* state[5] = {"start", "connected", "finalize", "end", "removed"};

  printf("joiner_callback event: %s\r\n", state[aEvent]);



}

/**************************************************************************//**
 * Commissioner State Change Callback
 *****************************************************************************/
void commissioner_callback(otCommissionerState aState, void *aContext)
{
  (void)aContext;

  const char* state[3] = {"disabled", "petition", "active"};

  printf("commissioner_callback event: %s\r\n", state[aState]);
}

/**************************************************************************//**
 * Simple Button Callback Handler
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if(handle == &sl_button_btn0)
  {
      if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED)
      {
          otError error;

          // start joiner
          error = otCommissionerAddJoiner(sInstance, NULL, COMMISSIONER_JOINER_PSKD, COMMISSIONER_JOINER_TIMEOUT);
          printf("start_joiner: %s\r\n", otThreadErrorToString(error));

          gui_print_log("[joiner] start");
      }
  }

  gui_button_handler(handle);
}
