/***************************************************************************//**
 * @file
 * @brief Coap Server Logic for the Base Station Node
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

#include <openthread/coap.h>

#include <string.h>
#include "printf.h"

static otCoapResource   mResource;
static char*            uri_path        = "question/answer";
static uint8_t          attr_state[256] = {0};

static void coap_server_handler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);

otError coap_server_init(otInstance *aInstance)
{
  otError error = OT_ERROR_NONE;

  // start coap server
  error = otCoapStart(aInstance, OT_DEFAULT_COAP_PORT);
  printf("coap server start: %s\r\n", otThreadErrorToString(error));
  if(error) {
      goto exit;
  }

  // server resource
  mResource.mUriPath = uri_path;
  mResource.mHandler = &coap_server_handler;
  mResource.mContext = aInstance;

  otCoapAddResource(aInstance, &mResource);

exit:
  return error;
}


static void coap_server_handler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
  otError    error        = OT_ERROR_NONE;

  otCoapCode message_code = otCoapMessageGetCode(aMessage);
  otCoapType message_type = otCoapMessageGetType(aMessage);

  otMessage  *response_message;

  response_message = otCoapNewMessage((otInstance *)aContext, NULL);

  // set up response message
  otCoapMessageInitResponse(response_message, aMessage, OT_COAP_TYPE_ACKNOWLEDGMENT, OT_COAP_CODE_CHANGED);

  // message set token
  otCoapMessageSetToken(aMessage, otCoapMessageGetToken(aMessage), otCoapMessageGetTokenLength(aMessage));

  // set payload marker
  otCoapMessageSetPayloadMarker(response_message);

  if(OT_COAP_CODE_GET == message_code)
  {
      error = otMessageAppend(response_message, attr_state, strlen((const char*) attr_state));
      printf("coap server get response append: %s\r\n", otThreadErrorToString(error));
      if(error)
      {
          goto exit;
      }

      error = otCoapSendResponse((otInstance *)aContext, response_message, aMessageInfo);
      printf("coap server send get response: %s\r\n", otThreadErrorToString(error));
      if(error)
      {
          goto exit;
      }

  }
  else if(OT_COAP_CODE_POST == message_code)
  {
      char data[32];
      uint16_t offset = otMessageGetOffset(aMessage);
      uint16_t read   = otMessageRead(aMessage, offset, data, sizeof(data));
      data[read]      = '\0';

      char ipaddr[OT_IP6_ADDRESS_STRING_SIZE];

      otIp6AddressToString(&aMessageInfo->mPeerAddr, ipaddr, OT_IP6_ADDRESS_STRING_SIZE);

      strcpy((char *)attr_state, data);
      printf("%s POST: %s\r\n", ipaddr, attr_state);

      if(OT_COAP_TYPE_CONFIRMABLE == message_type)
      {
          error = otMessageAppend(response_message, attr_state, strlen((const char*) attr_state));
          printf("coap server confirmable response: %s\r\n", otThreadErrorToString(error));
          if(error)
          {
              goto exit;
          }

          error = otCoapSendResponse((otInstance *)aContext, response_message, aMessageInfo);
          printf("coap server send confirm response: %s\r\n", otThreadErrorToString(error));
          if(error)
          {
              goto exit;
          }
      }
  }

exit:
  if(error != OT_ERROR_NONE && response_message != NULL)
  {
      otMessageFree(response_message);
  }
}
