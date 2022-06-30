/*!
 *  \example appLed.c
 *
 *  \brief
 *  Application LED task
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \date
 *  2021-07-14
 *
 *  \copyright
 *  Copyright (c) 2021, KUNBUS GmbH<br /><br />
 *  All rights reserved.<br />
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:<br />
 *  <ol>
 *  <li>Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.</li>
 *  <li>Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.</li>
 *  <li>Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.</li>
 *  </ol>
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "kernel/dpl/TaskP.h"
#include "osal.h"
#include "osal_error.h"
#include "appLed.h"
#include "appHwLed.h"
#include "appHwError.h"
#include "PN_API_DEV.h"

/*!
 *  \brief Application LED task's stack size.
 */
#define APP_LED_TASK_STACK_SIZE 4096

/*!
 *  \brief Application LED task's stack.
 */
static uint8_t APP_aLedTaskStack_g[APP_LED_TASK_STACK_SIZE] __attribute__((__aligned__ (32))) = { 0 };

/*!
 *  \brief Application LED task's object.
 */
static TaskP_Object ledTaskObj = { 0 };

/*!
 *  \brief Indicates, if DCP Signal LED shall blink.
 */
static bool dcpSignalLedBlinkingState_s = false;

/*!
 *  Function: APP_initializeAllLeds
 *
 *  \brief
 *  Initializes all LEDs.
 *
 *  \details
 *  Initializes all LEDs.
 *
 *  \param[in]     None
 *
 *  \return        Result of the operation as boolean.
 *  \retval        true        Operation completed successfully.
 *  \retval        false       Operation could not be completed successfully.
 */
static bool APP_initializeAllLeds(void)
{
    bool retVal = true;
    APP_HW_EError_t ledResult = APP_HW_LED_initGroup(APP_HW_LED_eCYCLIC_INST);
    if (ledResult == APP_HW_eNO_ERROR)
    {
        ledResult = APP_HW_LED_initGroup(APP_HW_LED_eSYS_FAILURE_INST);
    }
    if (ledResult == APP_HW_eNO_ERROR)
    {
        ledResult = APP_HW_LED_initGroup(APP_HW_LED_eDCP_SIGNAL_INST);
    }

    if (ledResult != APP_HW_eNO_ERROR)
    {
        OSAL_printf("\n[APP] ERROR: Failed to initialize LEDs : %d", ledResult);
        retVal = false;
    }

    return retVal;
}

/*!
 *  Function: APP_updateCyclicLedsFromPD
 *
 *  \brief
 *  Updates the cyclic LEDs from the process data.
 *
 *  \details
 *  Updates the cyclic LEDs by getting the PROFINET process data.
 *  The first byte of the output data is used to set the LED state.
 *  The output data will be set to 0, when there is no PROFINET connection.
 *
 *  \param[in]     None
 *
 *  \return        Result of the operation as boolean.
 *  \retval        true        Operation completed successfully.
 *  \retval        false       Operation could not be completed successfully.
 */
static bool APP_updateCyclicLedsFromPD(void)
{
    bool retVal = true;
    uint8_t *pBufOut = NULL;
    uint32_t result = PN_API_DEV_getBufferOutputData(&pBufOut);
    if (result == PN_API_DEV_eOK)
    {
        APP_HW_EError_t ledResult = APP_HW_LED_setGroup(APP_HW_LED_eCYCLIC_INST, pBufOut[0]);
        if (ledResult != APP_HW_eNO_ERROR)
        {
            OSAL_printf("\n[APP] ERROR: Failed to set cyclic data LEDs : %d", ledResult);
            retVal = false;
        }
    }
    else
    {
        OSAL_printf("\n[APP] ERROR: Failed to read PROFINET output data : %d", result);
        retVal = false;
    }
    return retVal;
}

/*!
 *  Function: APP_setSysFailureLed
 *
 *  \brief
 *  Sets the SYS FAILURE LED state.
 *
 *  \details
 *  This function sets the SYS FAILURE status LED to a particular state. It has to be called every
 *  #APP_LED_TASK_TICK_MS since it keeps track of the ticks passed to toggle the SYS FAILURE LED in the
 *  blink state.
 *
 *  \param[in]     ledState_p  state of the SYS FAILURE LED to be set.
 *
 *  \return        Result of the operation as boolean.
 *  \retval        true        Operation completed successfully.
 *  \retval        false       Operation could not be completed successfully.
 */
static bool APP_setSysFailureLed(APP_ELedState_t ledState_p)
{
    bool retVal = true;
    static uint32_t counter = 0;
    APP_HW_EError_t ledResult = APP_HW_eNO_ERROR;
    switch(ledState_p)
    {
        case APP_eLedOn:
        {
            ledResult = APP_HW_LED_setGroup(APP_HW_LED_eSYS_FAILURE_INST, 1U);
            counter = 0;
            break;
        }
        case APP_eLedBlink:
        {
            if ((counter % APP_LED_SYS_FAILURE_BLINK_COUNTER) == 0)
            {
                ledResult = APP_HW_LED_toggleGroup(APP_HW_LED_eSYS_FAILURE_INST);
            }
            counter ++;
            break;
        }
        default :
        {
            ledResult = APP_HW_LED_setGroup(APP_HW_LED_eSYS_FAILURE_INST, 0U);
            counter = 0;
            break;
        }
    }

    if (ledResult != APP_HW_eNO_ERROR)
    {
        OSAL_printf("\n[APP] ERROR: Failed to set SYS FAILURE status LED : %d", ledResult);
        retVal = false;
    }
    return retVal;
}

/*!
 *  Function: APP_updateSysFailureLedFromPNState
 *
 *  \brief
 *  Updates the SYS Failure status LED.
 *
 *  \details
 *  This function gets the current PROFINET state and then updates the SYS FAILURE LED accordingly.
 *  The SYS FAILURE LED blinks at #APP_LED_SYS_FAILURE_BLINK_FREQ_HZ when there is no PROFINET connection.
 *
 *  \param[in]     None
 *
 *  \return        Result of the operation as boolean.
 *  \retval        true        Operation completed successfully.
 *  \retval        false       Operation could not be completed successfully.
 */
static bool APP_updateSysFailureLedFromPNState(void)
{
    bool retVal = true;
    PN_API_DEV_ERunState_t pnState = PN_API_DEV_eRS_NO_CONNECTION;
    uint32_t result = PN_API_DEV_getRunState(&pnState);
    if (result == PN_API_DEV_eOK)
    {
        switch (pnState)
        {
            case PN_API_DEV_eRS_RUN:
            {
                retVal = APP_setSysFailureLed(APP_eLedOff);
                break;
            }
            // In all other cases the SYS FAILURE LED should blink.
            default :
            {
                retVal = APP_setSysFailureLed(APP_eLedBlink);
                break;
            }
        }
    }
    else
    {
        OSAL_printf("\n[APP] ERROR: Failed to get PROFINET state : %d", result);
        retVal = false;
    }
    return retVal;
}

/*!
 *  Function: APP_setDcpSignalLed
 *
 *  \brief
 *  Sets the DCP Signal LED state.
 *
 *  \details
 *  This function sets the DCP Signal LED to a particular state. It has to be called every
 *  #APP_LED_TASK_TICK_MS since it keeps track of the ticks passed to toggle the DCP Signal LED in the
 *  blink state.
 *
 *  \param[in]     ledState_p  state of the DCP Signal LED to be set.
 *
 *  \return        Result of the operation as boolean.
 *  \retval        true        Operation completed successfully.
 *  \retval        false       Operation could not be completed successfully.
 */
static bool APP_setDcpSignalLed(APP_ELedState_t ledState_p)
{
    bool retVal = true;
    static uint32_t counter = 0;
    APP_HW_EError_t ledResult = APP_HW_eNO_ERROR;
    switch (ledState_p)
    {
        case APP_eLedOn:
        {
            ledResult = APP_HW_LED_setGroup(APP_HW_LED_eDCP_SIGNAL_INST, 1U);
            counter = 0;
            break;
        }
        case APP_eLedBlink:
        {
            if ((counter % APP_LED_DCP_SIGNAL_BLINK_COUNTER) == 0)
            {
                ledResult = APP_HW_LED_toggleGroup(APP_HW_LED_eDCP_SIGNAL_INST);
            }
            counter++;
            break;
        }
        default:
        {
            ledResult = APP_HW_LED_setGroup(APP_HW_LED_eDCP_SIGNAL_INST, 0U);
            counter = 0;
            break;
        }
    }

    if (ledResult != APP_HW_eNO_ERROR)
    {
        OSAL_printf("\n[APP] ERROR: Failed to set DCP Signal status LED : %d", ledResult);
        retVal = false;
    }
    return retVal;
}

/*!
 *  Function: APP_updateDcpSignalLed
 *
 *  \brief
 *  Updates the DCP Signal LED.
 *
 *  \details
 *  This function checks, if DCP Signal LED shall blink or be off.
 *  Thereafter it sets the LED to appropriate state.
 *
 *  \param[in]     None
 *
 *  \return        Result of the operation as boolean.
 *  \retval        true        Operation completed successfully.
 *  \retval        false       Operation could not be completed successfully.
 */
static bool APP_updateDcpSignalLed(void)
{
    bool retVal = true;
    APP_HW_EError_t ledResult = APP_HW_eNO_ERROR;

    if (dcpSignalLedBlinkingState_s == false)
    {
        retVal = APP_setDcpSignalLed(APP_eLedOff);
    }
    else
    {
        retVal = APP_setDcpSignalLed(APP_eLedBlink);
    }

    if (ledResult != APP_HW_eNO_ERROR)
    {
        OSAL_printf("\n[APP] ERROR: Failed to set DCP Signal LED : %d", ledResult);
        retVal = false;
    }

    return retVal;
}

/*!
 *  Function: APP_LedTask
 *
 *  \brief
 *  LED task function.
 *
 *  \details
 *  LED task function. It calls functions to update the cyclic data LEDs and System Failure status LEDs.
 *  If any of these calls fail we exit the task. Task is run every #APP_LED_TASK_TICK_MS.
 *
 *  \param[in]     pArgs_p    pointer to general arguments passed when starting the task.
 *
 *  \return        None
 */
static void APP_LedTask(void *pArgs_p)
{
    while(1)
    {
        if(APP_updateCyclicLedsFromPD() == false)
        {
            break;
        }

        if(APP_updateSysFailureLedFromPNState() == false)
        {
            break;
        }

        if (APP_updateDcpSignalLed() == false)
        {
            break;
        }

        OSAL_SCHED_sleep(APP_LED_TASK_TICK_MS);
    }

    TaskP_exit();
}

/*!
 *  Function: APP_LED_startTask
 *
 *  \brief
 *  Function to start the LED task.
 *
 *  \details
 *  Function to start the LED task, it also initializes all the LEDs.
 *
 *  \param[in]     None
 *
 *  \return        Result of the operation as boolean.
 *  \retval        true        Operation completed successfully.
 *  \retval        false       Operation could not be completed successfully.
 */
bool APP_startLedTask(void)
{
    bool result = APP_initializeAllLeds();
    if(result == true)
    {
        int32_t taskErr = SystemP_SUCCESS;
        TaskP_Params ledTaskParam = { 0 };

        TaskP_Params_init (&ledTaskParam);

        ledTaskParam.name        = "led_task";
        ledTaskParam.stackSize   = sizeof(APP_aLedTaskStack_g);
        ledTaskParam.stack       = (uint8_t*)APP_aLedTaskStack_g;
        ledTaskParam.priority    = OSAL_TASK_ePRIO_1;
        ledTaskParam.taskMain    = (TaskP_FxnMain)APP_LedTask;
        ledTaskParam.args        = NULL;

        taskErr = TaskP_construct(&ledTaskObj, &ledTaskParam);
        if (taskErr != SystemP_SUCCESS)
        {
            OSAL_printf("[APP] ERROR: Failed to create task %s (%ld)\r\n", ledTaskParam.name, taskErr);
            TaskP_destruct(&ledTaskObj);
            result = false;
        }
    }
    return result;
}

/*!
 *  Function: APP_startDcpSignalLedBlinking
 *
 *  \brief
 *  Function to start blinking of the DCP Signal LED.
 *
 *  \details
 *  After executing this function DCP Signal LED will start blinking with #APP_LED_DCP_SIGNAL_BLINK_FREQ_HZ frequency in the main LED task.
 *
 *  \param[in]     None.
 *
 *  \return        None.
 */
void APP_startDcpSignalLedBlinking(void)
{
    dcpSignalLedBlinkingState_s = true;
}

/*!
 *  Function: APP_stopDcpSignalLedBlinking
 *
 *  \brief
 *  Function to stop blinking of the DCP Signal LED.
 *
 *  \details
 *  After executing this function DCP Signal LED will stop blinking in the main LED task.
 *
 *  \param[in]     None.
 *
 *  \return        None.
 */
void APP_stopDcpSignalLedBlinking(void)
{
    dcpSignalLedBlinkingState_s = false;
}