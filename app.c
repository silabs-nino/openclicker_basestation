/***************************************************************************//**
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
// Define module name for Power Manager debuging feature.
#define CURRENT_MODULE_NAME    "OPENTHREAD_CLICKER_REMOTE"

#include <assert.h>
#include <openthread-core-config.h>
#include <openthread/config.h>

#include <openthread/diag.h>
#include <openthread/tasklet.h>
#include <openthread/dataset_ftd.h>
#include <openthread/thread_ftd.h>

#include "openthread-system.h"
#include "app.h"


#include "base_station.h"
#include "gui.h"


#include "sl_component_catalog.h"
#ifdef SL_CATALOG_POWER_MANAGER_PRESENT
#include "sl_power_manager.h"
#endif

#ifndef OPENTHREAD_ENABLE_COVERAGE
#define OPENTHREAD_ENABLE_COVERAGE 0
#endif

#include "printf.h"

#if OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE
void *otPlatCAlloc(size_t aNum, size_t aSize)
{
    return calloc(aNum, aSize);
}

void otPlatFree(void *aPtr)
{
    free(aPtr);
}
#endif

static otInstance *    sInstance       = NULL;

otInstance *otGetInstance(void)
{
    return sInstance;
}


void otSysEventSignalPending(void)
{

}


/*
 * Provide, if required an "otPlatLog()" function
 */
#if OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_APP
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    OT_UNUSED_VARIABLE(aLogLevel);
    OT_UNUSED_VARIABLE(aLogRegion);
    OT_UNUSED_VARIABLE(aFormat);

    va_list ap;
    va_start(ap, aFormat);
//    otCliPlatLogv(aLogLevel, aLogRegion, aFormat, ap);
    printf(aFormat, ap);
    va_end(ap);
}
#endif

void sl_ot_create_instance(void)
{
    sInstance = otInstanceInitSingle();
    assert(sInstance);
}

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
void app_init(void)
{
  gui_init();
  base_station_init();
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
void app_process_action(void)
{
    otTaskletsProcess(sInstance);
    otSysProcessDrivers(sInstance);
    gui_update();
}

/**************************************************************************//**
 * Application Exit.
 *****************************************************************************/
void app_exit(void)
{
    otInstanceFinalize(sInstance);
#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
    free(otInstanceBuffer);
#endif
    // TO DO : pseudo reset?
}
