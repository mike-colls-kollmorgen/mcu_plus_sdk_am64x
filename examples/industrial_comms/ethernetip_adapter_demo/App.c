/*!
 *  \example App.c
 *
 *  \brief
 *  EtherNet/IP&trade; Adapter Example Application.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \date
 *  2021-06-09
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
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#include <api/EI_API.h>
#include <api/EI_API_def.h>
#include <CUST_PHY_base.h>

#include "AppPerm.h"
#include <kernel/dpl/TaskP.h>
#include <drivers/pinmux.h>
#include <drivers/pruicss.h>
#include <drivers/sciclient/include/sciclient_pm.h>
#include <appPhyReset.h>

#include "FreeRTOS.h"
#include "task.h"

#include <osal.h>
#include <osal_error.h>
#include <hwal.h>
#include <pru.h>
#include <Board.h>

#include <kbTrace.h>


#include <App.h>
#include <AppClass71.h>

#include "ti_drivers_config.h"
#include "ti_drivers_open_close.h"
#include "ti_board_config.h"
#include "ti_board_open_close.h"

//Board_IDInfo_v2 boardInfo;
extern PRUICSS_Handle prusshandle;
//int _vectors = 0;

// Global variables and pointers used in this example.
// has to stay, used in lib_eip_lwip_ip :-(
uint8_t EI_APP_aMacAddr_g[] = {0xc8, 0x3e, 0xa7, 0x00, 0x00, 0x59};


// Static variables and pointers used in this example.
static StackType_t EI_APP_aTaskStackMainStack_s[EI_APP_STACK_MAIN_TASK_STACK_SIZE] __attribute__((aligned(32), section(".threadstack"))) = {0};

static uint16_t EI_APP_aExtendedStatus[255] = {0};
static char aOutStream_s[0x200] = {0};

static EI_API_ADP_T*      pAdp_s = NULL;
static EI_API_CIP_NODE_T* pCip_s = NULL;

// Function prototypes.
static uint8_t* EI_APP_getMacAddr (void);
static bool     EI_APP_cipSetup(EI_API_CIP_NODE_T* pCipNode_p);
static bool     EI_APP_cipCreateCallback(EI_API_CIP_NODE_T* pCipNode_p);

uint32_t EI_APP_globalError_g = 0;


/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  This function deinitializes / cleans the objects created
 *  before an exit.
 *
 *  \details
 *  This function deinitializes CIP node and ADP objects. It also
 *  deinitializes the IDK board GPIOs before an exit.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \return     int               Exit code.
 *
 *  \retval     0                 Clean up success.
 *  \retval     1                 Clean up failed.
 *
 */
static int EI_APP_cleanup()
{
    int exit_code = 0;

    // Try to delete the CIP node.
    if (EI_API_CIP_NODE_delete(pCip_s))
    {
        // Fail.
        exit_code = 1;
    }

    // Try to delete the ADP.
    if (EI_API_ADP_delete(pAdp_s))
    {
        // Fail.
        exit_code = 1;
    }

    // Try to deinitialize the IDK board (unexport the GPIO pins).
    if (IDK_deInit())
    {
        // Fail
        exit_code = 1;
    }

    return exit_code;
}


/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Initialize EtherNet/IP&trade; Adapter data.
 *
 *  \details
 *  Enable the device implementer to override default data, such as for example
 *  Vendor ID, Device Type, Product Code, Revision, Product Name, etc.
 *  and read permanent saved data.
 */
void EI_APP_adpInit(void)
{
    uint32_t errCode = 0;
    uint16_t vendorId = 806;
    uint16_t deviceType = 0x002B;
    uint16_t productCode = PRODUCT_CODE_OF_CONFIGURATION;
    uint32_t serialNumber = 0x00000065;

    EI_API_ADP_SRevision_t revision;

    const char productName[]                = PRODUCT_NAME_OF_CONFIGURATION;
    const char timeSyncProductDescription[] = TIMESYNC_PRODUCT_DESCRIPTION_OF_CONFIGURATION;
    const char timeSyncManufactureID[]      = TIMESYNC_MANUFACTURE_ID_OF_CONFIGURATION;
    const char timeSyncRevisionData[]       = TIMESYNC_REVISION_DATA_OF_CONFIGURATION;


    errCode = EI_API_ADP_setVendorId(pAdp_s, vendorId);
    errCode = EI_API_ADP_setDeviceType(pAdp_s, deviceType);
    errCode = EI_API_ADP_setProductCode(pAdp_s, productCode);

    revision.major = 0x01;
    revision.minor = 0x04;
    errCode = EI_API_ADP_setRevision(pAdp_s, revision);

    errCode = EI_API_ADP_setSerialNumber(pAdp_s, serialNumber);
    errCode = EI_API_ADP_setProductName(pAdp_s, productName);

    errCode = EI_API_ADP_setTimeSyncManufactureID(pAdp_s, timeSyncManufactureID);
    errCode = EI_API_ADP_setTimeSyncRevisionData(pAdp_s, timeSyncRevisionData);
    errCode = EI_API_ADP_setTimeSyncProductDescription(pAdp_s, timeSyncProductDescription);

    // apply permanent saved data.
    EI_APP_PERM_read();
}

/*!
*  <!-- Description: -->
*
*  \brief
*  Generates attributes and services for a CIP&trade;class.
*
*  \details
*  Create a CIP class with a Class IDs using the value specified in parameter classId.<br />
*  Generates attributes and services for that class.<br />
*  Adds read and write services.<br />
*  Adds 64 8-bit attributes with callback function.<br />
*  Adds 32 16-bit attributes.<br />
*  Adds 16 32-bit attributes.<br />
*  Adds 8 64-bit attributes.
*
*/
void EI_APP_cipGenerateContent(EI_API_CIP_NODE_T* pCipNode_p, uint16_t classId_p, uint16_t instanceId_p)
{
    EI_API_CIP_SService_t service;
    uint16_t i = 0;

    EI_API_CIP_createClass(pCip_s, classId_p);

    service.getAttrAllResponseCnt = 0;
    service.callback = NULL;
    service.code = EI_API_CIP_eSC_GETATTRSINGLE;
    EI_API_CIP_addClassService(pCip_s, classId_p, &service);
    service.code = EI_API_CIP_eSC_SETATTRSINGLE;
    EI_API_CIP_addClassService(pCip_s, classId_p, &service);

    EI_API_CIP_createInstance(pCip_s, classId_p, instanceId_p);

    service.code = EI_API_CIP_eSC_GETATTRSINGLE;
    EI_API_CIP_addInstanceService(pCip_s, classId_p, instanceId_p, &service);
    service.code = EI_API_CIP_eSC_SETATTRSINGLE;
    EI_API_CIP_addInstanceService(pCip_s, classId_p, instanceId_p, &service);

    uint16_t attribID = 0x300;

    // 64 USINT (uint8_t).
    for (i = 0; i < 64; i++)
    {
        EI_API_CIP_SAttr_t attr;
        memset(&attr, 0, sizeof(attr));
        attr.id = attribID;
        attr.edt = EI_API_CIP_eEDT_USINT;
        attr.accessRule = EI_API_CIP_eAR_GET_AND_SET;
        attr.pvValue = &i;

        EI_API_CIP_addInstanceAttr(pCip_s, classId_p, instanceId_p, &attr);
        EI_API_CIP_setInstanceAttr(pCip_s, classId_p, instanceId_p, &attr);

        attribID++;
    }

    // 32 UINT (uint16_t).
    for (i = 0; i < 32; i++)
    {
        EI_API_CIP_SAttr_t attr;
        memset(&attr, 0, sizeof(attr));
        attr.id = attribID;
        attr.edt = EI_API_CIP_eEDT_UINT;
        attr.accessRule = EI_API_CIP_eAR_GET_AND_SET;
        attr.pvValue = &i;

        EI_API_CIP_addInstanceAttr(pCip_s, classId_p, instanceId_p, &attr);
        EI_API_CIP_setInstanceAttr(pCip_s, classId_p, instanceId_p, &attr);

        attribID++;
    }

    // 16 UDINT (uint32_t).
    for (i = 0; i < 16; i++)
    {
        EI_API_CIP_SAttr_t attr;
        memset(&attr, 0, sizeof(attr));
        attr.id = attribID;
        attr.edt = EI_API_CIP_eEDT_UDINT;
        attr.accessRule = EI_API_CIP_eAR_GET_AND_SET;
        attr.pvValue = &i;

        EI_API_CIP_addInstanceAttr(pCip_s, classId_p, instanceId_p, &attr);
        EI_API_CIP_setInstanceAttr(pCip_s, classId_p, instanceId_p, &attr);

        attribID++;
    }

    // 8 ULINT (uint64_t).
    for (i = 0; i < 8; i++)
    {
        EI_API_CIP_SAttr_t attr;
        memset(&attr, 0, sizeof(attr));
        attr.id = attribID;
        attr.edt = EI_API_CIP_eEDT_ULINT;
        attr.accessRule = EI_API_CIP_eAR_GET_AND_SET;
        attr.pvValue = &i;

        EI_API_CIP_addInstanceAttr(pCip_s, classId_p, instanceId_p, &attr);
        EI_API_CIP_setInstanceAttr(pCip_s, classId_p, instanceId_p, &attr);

        attribID++;
    }
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Setup the application with classes, instances, attributes, and assemblies.
 *
 *  \details
 *  Setup the application with classes, instances, attributes, and assemblies.<br />
 *  For the assemblies, use instances in the Vendor Specific range of IDs.
 *
 */
static bool EI_APP_cipSetup(EI_API_CIP_NODE_T* pCipNode_p)
{
    uint32_t errCode = 0;
    uint16_t i = 0;

    uint16_t classId = 0x70;
    uint16_t instanceId = 0x01;


    // Create new class 0x70 with read and write service and several attributes.
    EI_APP_cipGenerateContent(pCipNode_p, classId, instanceId);

    errCode = EI_API_CIP_createAssembly(pCipNode_p, 0xfe, EI_API_CIP_eAR_GET); // Input-only.
    errCode = EI_API_CIP_createAssembly(pCipNode_p, 0xff, EI_API_CIP_eAR_GET); // Listen-only.

    errCode = EI_API_CIP_createAssembly(pCipNode_p, 0x64, EI_API_CIP_eAR_GET_AND_SET);
    errCode = EI_API_CIP_createAssembly(pCipNode_p, 0x65, EI_API_CIP_eAR_GET_AND_SET);

    for (i = 0x300; i < 0x305; i++)
    {
        errCode = EI_API_CIP_addAssemblyMember(pCipNode_p, 0x64, classId, instanceId, i);
        if (errCode != EI_API_CIP_eERR_OK)
        {
            OSAL_printf("Failed to add Class ID %#x, Instance ID %#x, Attribute ID %#x to Assembly Instance 0x64:  Error code: 0x%08x\n", classId, instanceId, (uint16_t)i, errCode);
        }

        errCode = EI_API_CIP_addAssemblyMember(pCipNode_p, 0x65, classId, instanceId, (uint16_t)(8 + i));
        if (errCode != EI_API_CIP_eERR_OK) {
            OSAL_printf("Failed to add Class ID %#x, Instance ID %#x, Attribute ID %#x to Assembly Instance 0x65:  Error code: 0x%08x\n", classId, instanceId, (uint16_t)(8 + i), errCode);
        }

    }

    return true;
}


/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Creates several callbacks.
 *
 *  \details
 *  Creates and registers callbacks which are needed for program flow.
 *  - Store non-volatile data callback.
 *  - Set_Attribute_Single service callback.
 *  - Reset service callback.
 *
 */
static bool EI_APP_cipCreateCallback(EI_API_CIP_NODE_T* pCipNode_p)
{
    uint8_t errorCnt = 0;   // Variable to check sum up return errors.

    // Your callback function which stores your data when triggered.
    EI_API_CIP_CBService ptr_my_config_cb = EI_APP_PERM_configCb;

    // Register callbacks for Set_Attribute_Single service.
    EI_API_CIP_SService_t srvc = { EI_API_CIP_eSC_SETATTRSINGLE, 0, NULL, ptr_my_config_cb };
    EI_API_CIP_setInstanceServiceFunc(pCipNode_p, 0x00F5, 0x0001, &srvc);
    EI_API_CIP_setInstanceServiceFunc(pCipNode_p, 0x00F6, 0x0001, &srvc);
    EI_API_CIP_setInstanceServiceFunc(pCipNode_p, 0x00F6, 0x0002, &srvc);
    EI_API_CIP_setInstanceServiceFunc(pCipNode_p, 0x0043, 0x0001, &srvc);
    EI_API_CIP_setInstanceServiceFunc(pCipNode_p, 0x0048, 0x0001, &srvc);

    // Add callback for Reset service of class ID 0x01.
    EI_API_CIP_SService_t srvcReset = { EI_API_CIP_eSC_RESET, 0, NULL, EI_APP_PERM_reset };
    if (EI_API_CIP_setInstanceServiceFunc(pCipNode_p, 0x01, 0x01, &srvcReset) != EI_API_CIP_eERR_OK) errorCnt++;

    return true;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Callback function for ForwardOpen, LargeForwardOpen and ForwardClose.
 *
 *  \details
 *  Callback function which is called when a request for the services
 *  ForwardOpen, LargeForwardOpen and ForwardClose was received.
 *
 */
EI_API_ADP_SEipStatus_t EI_APP_CmgrCb(uint32_t serviceCode_p, EI_API_ADP_UCmgrInfo_u cmgrInfo)
{

    EI_API_ADP_SEipStatus_t ret_val= {.gen_status=0,
                                      .extended_status_size=0,
                                      .extended_status_arr=EI_APP_aExtendedStatus};

    switch(serviceCode_p)
    {
    case 0x54:
        // OSAL_printf("Forward open Connection Serial Number: 0x%04x\r\n", cmgrInfo.forwardOpenInfo.conSerialNum);
        break;
    case 0x5b:
        // OSAL_printf("Large forward open Connection Serial Number: 0x%04x\r\n", cmgrInfo.forwardOpenInfo.conSerialNum);
        break;
    case 0x4e:
        // OSAL_printf("Forward close Connection Serial Number: 0x%04x\r\n", cmgrInfo.forwardCloseInfo.conSerialNum);
        break;
    default:
        printf("unknown service code");
    }

    return ret_val;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Basic initialization function.
 *
 *  \details
 *  Creates a new EtherNet/IP&trade; Adapter.<br />
 *  Initializes data structures from non-volatile storage.<br />
 *  Registers stack error handler.<br />
 *  Initializes the Adapter.<br />
 *  Create a CIP&trade; node.<br />
 *
 */
bool EI_APP_init(void)
{
    bool result = 1;
    EI_API_ADP_SParam_t macAddr;


    // Initialize adapter for 1 (one) interface.
    pAdp_s = EI_API_ADP_new(1);

    // Init module for permanent data.
    EI_APP_PERM_init(pAdp_s);

#if defined(TIME_SYNC)
    EI_API_ADP_setTimeSyncSupported(pAdp_s);
#endif

    // Setup error handler for the EtherNet/IP stack.
    EI_API_ADP_setErrorHandlerFunc(EI_APP_stackErrorHandlerCb);

    EI_APP_getMacAddr();

    // Initialize data for the adapter.
    EI_APP_adpInit();

#if defined(QUICK_CONNECT)
    // Enable QuickConnect
    EI_API_ADP_setQuickConnectSupported(pAdp_s);
#endif

    EI_APP_stackInit();


    // Create a CIP node.
    pCip_s = EI_API_CIP_NODE_new();

    // Create callbacks for changed values.
    EI_APP_cipCreateCallback(pCip_s);

    // Create callback for ForwardOpen, LargeForwardOpen and ForwardClose.
    EI_API_ADP_setCmgrCb(EI_APP_CmgrCb);

    // Create vendor specific classes.
    EI_APP_cipSetup(pCip_s);

    IDK_init();

    EI_APP_CLASS71_init(pCip_s);

    // Finally apply.
    EI_API_ADP_init(pAdp_s);

    EI_API_ADP_getMacAddr(pAdp_s, &macAddr);

    OSAL_printf("EI_API_ADP_getMacAddr:  %02x:%02x:%02x:%02x:%02x:%02x\r\n",
        macAddr.data[0],
        macAddr.data[1],
        macAddr.data[2],
        macAddr.data[3],
        macAddr.data[4],
        macAddr.data[5]);

    return result;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Cyclically called run function.
 *
 *  \details
 *  Cyclically called run function, handles the EtherNet/IP stack and hardware
 *  specific functionality, such as reading switches, reading inputs, setting outputs
 *  and LEDs.
 *
 */
void EI_APP_run(void)
{
    uint32_t errCode = EI_API_CIP_eERR_OK;
    uint8_t attrValue = 0;
    uint16_t attr;

    EI_API_ADP_run();

    // Mirror I/O data
    for(attr = 0; attr < 5; attr++)
    {
        errCode = EI_API_CIP_getAttr_usint(pCip_s, 0x0070, 0x0001, attr + 0x0308, &attrValue);
        if (attr == 0 && errCode == EI_API_CIP_eERR_OK)
        {
            IDK_setIndustrialLeds(attrValue);
        }
        EI_API_CIP_setAttr_usint(pCip_s, 0x0070, 0x0001, attr + 0x0300, attrValue);
    }

    //EI_APP_CLASS71_run();
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Cyclically called run function.
 *
 *  \details
 *  Cyclically called run function, handles the EtherNet/IP stack and hardware
 *  specific functionality, such as reading switches, reading inputs, setting outputs
 *  and LEDs.
 *
 *  We return with the following exit codes from this function:
 *  Code 10 for EIP Identity Reset Type 0.
 *  Code 11 for EIP Identity Reset Type 1.
 *  Code 12 for EIP Identity Reset Type 2.
 *  Code 130 on reception of SIGINT signal.
 *  Code 143 on reception of SIGTERM signal.
 *
 *
 */
void EI_APP_mainTask(
    void* pvTaskArg_p)
{
    uint32_t err;

    Board_init();

    Drivers_open();
    Board_driversOpen();

    OSAL_registerPrintOut(NULL, EI_APP_osPrintfCb);

    err = HWAL_init ();
    if (err != OSAL_NO_ERROR)
    {
        goto laError;
    }

    CUST_PHY_CBregisterLibDetect(CUST_PHY_detect, NULL);

    int16_t resetServiceFlag;
    if (false == EI_APP_init())
    {
        OSAL_printf("Fatal error: application initialization failed\n");
        return;
    }

    for (;;)
    {
        EI_APP_run();
        resetServiceFlag = EI_APP_PERM_getResetRequired();
        if (resetServiceFlag  != -1)
        {
            break;
        }
        if(EI_APP_PERM_getConfigChanged())
        {
            EI_APP_PERM_write();
        }
        OSAL_SCHED_yield();
    }

laError:

    EI_APP_cleanup();

    EI_API_ADP_pruicssStop();

    Board_driversClose();

    Drivers_close();

    Board_deinit();

    System_deinit();

    Sciclient_pmDeviceReset(0xFFFFFFFFU);
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Main entry point.
 *
 *  \details
 *  Main entry point.<br />
 *  Initializes the general operating system abstraction layer,
 *  starts the EtherNet/IP&trade; main application task, and starts the
 *  operating system abstraction layer.
 *
 */
int main(
    int argc,
    char* argv[])
{
    uint32_t err;

    System_init ();

    OSAL_registerErrorHandler (EI_APP_osErrorHandlerCb);

#if defined(QUICK_CONNECT)
    OSAL_printfSuppress(true);
#endif

    err = OSAL_init ();
    if (err != OSAL_NO_ERROR)
    {
        goto laError;
    }

    TaskP_Object appThreadHandle;
    TaskP_Params appThreadParam;

    TaskP_Params_init(&appThreadParam);

    appThreadParam.name      = "main_task";
    appThreadParam.stackSize = EI_APP_STACK_MAIN_TASK_STACK_SIZE_BYTE;
    appThreadParam.stack     = (uint8_t*)EI_APP_aTaskStackMainStack_s;
    appThreadParam.priority  = TaskP_PRIORITY_LOWEST+4;
    appThreadParam.taskMain  = (TaskP_FxnMain)EI_APP_mainTask;
    appThreadParam.args      = NULL;

    err = TaskP_construct(&appThreadHandle, &appThreadParam);

    if (SystemP_SUCCESS != err)
    {
        OSAL_printf("Error setting create thread of %s (%ld)\r\n", appThreadParam.name, err);
        OSAL_error(__FILE__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    OSAL_startOs ();

    return (0);

//-------------------------------------------------------------------------------------------------
laError:

    return (-1);
}


/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  General stack error handler.
 *
 *  \details
 *  General stack error handler. Reporting of stack internal errors.
 *
 */
void EI_APP_stackErrorHandlerCb(
    uint32_t i32uErrorCode_p,
    uint8_t  bFatal_p,
    uint8_t  i8uNumOfPara_p,
    va_list  argptr_p)
{
    OSAL_printf("###### Stack Error: 0x%08x, %s ######\n", i32uErrorCode_p, bFatal_p == 0 ? "non fatal" : "fatal");

    if (bFatal_p)
    {
        // TODO: handle the app_cleanup return codes.
        EI_APP_cleanup();
        // TODO: Return an appropriate exit code in this case.
        exit(255);
    }

}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  General OS error handler.
 *
 *  \details
 *  General OS error handler. Reporting of OS errors.
 *
 */
void EI_APP_osErrorHandlerCb (
    uint32_t errorCode_p,           //!< [in] Error code
    bool fatal_p,                   //!< [in] Is Error fatal
    uint8_t paraCnt_p,              //!< [in] parameter counter
    va_list argptr_p)               //!< [in] Error arguments

{
    EI_APP_globalError_g = errorCode_p;

    OSAL_printf ("\nError: 0x%8.8x, Fatal: %s", errorCode_p, fatal_p ? "yes" : "no");

    if (fatal_p == true)
    {
        exit (1);
    }
}

/*!
* <!-- Description: -->
*
* \brief
* Printf output callback.
*
* \details
* Printing of specific string to UART output.
*
*/
void EI_APP_osPrintfCb(void* pContext_p, const char* __restrict pFormat_p, va_list argptr_p)
{
    /* @cppcheck_justify{unusedVariable} false-positive: variable is used */
    //cppcheck-suppress unusedVariable
    int32_t transferOK;
    /* @cppcheck_justify{unusedVariable} false-positive: variable is used */
    //cppcheck-suppress unusedVariable
    UART_Transaction transaction;

    OSALUNREF_PARM(pContext_p);

    UART_Transaction_init(&transaction);

    memset(aOutStream_s, 0, sizeof(aOutStream_s));
    (void)vsnprintf(aOutStream_s, sizeof(aOutStream_s), pFormat_p, argptr_p);

    transaction.count = strlen(aOutStream_s);
    transaction.buf = (void *)aOutStream_s;
    transaction.args = NULL;
    transferOK = UART_write(gUartHandle[CONFIG_UART_CONSOLE], &transaction);

    (void)transferOK;
}


//*************************************************************************************************
void EI_APP_stackInit (void)
{
    uint32_t err;
    EIP_SLoadParameter tParam;

    err = APP_initPhyResetGpio ();
    if (err != OSAL_NO_ERROR)
    {
        goto laError;
    }

    memset (&tParam, 0, sizeof (EIP_SLoadParameter));
    memmove (tParam.ai8uMacAddr, EI_APP_aMacAddr_g, 6);

    tParam.pruInstance  = CONFIG_PRU_ICSS1; /* SysConfig definition of PRU-ICSS block instance used by stack */
    tParam.phy0Instance = CONFIG_ETHPHY0;   /* SysConfig definition of PHY0 instance used by stack */
    tParam.phy1Instance = CONFIG_ETHPHY1;   /* SysConfig definition of PHY1 instance used by stack */

    err = EI_API_ADP_pruicssInit (&tParam);
    if (err)
    {
        goto laError;
    }

    OSAL_printf("+EI_API_ADP_pruicssStart\r\n");
    EI_API_ADP_pruicssStart();
    OSAL_printf("-EI_API_ADP_pruicssStart\r\n");

    return;

//-------------------------------------------------------------------------------------------------
laError:

    printf ("\nStack Init Error: 0x%8.8x", err);
    return;
}

//*************************************************************************************************
//| Function:
//|
//! \brief
//!
//! \detailed
//!
//!
//!
//! \ingroup
//-------------------------------------------------------------------------------------------------
static uint8_t* EI_APP_getMacAddr (void)
{
#ifndef _DEBUG_USE_KUNBUS_MAC_ADDRESS
    static uint8_t mac_addr[6];
    const uint32_t mac_address_upper_16_bits = *IDK_CTRLMMR0_MAC_ID1;
    const uint32_t mac_address_lower_32_bits = *IDK_CTRLMMR0_MAC_ID0;
    mac_addr[0] = (uint8_t) (mac_address_upper_16_bits >> 8);
    mac_addr[1] = (uint8_t) (mac_address_upper_16_bits);
    mac_addr[2] = (uint8_t) (mac_address_lower_32_bits >> 24);
    mac_addr[3] = (uint8_t) (mac_address_lower_32_bits >> 16);
    mac_addr[4] = (uint8_t) (mac_address_lower_32_bits >> 8);
    mac_addr[5] = (uint8_t) (mac_address_lower_32_bits);
    // EI_APP_aMacAddr_g is directly used in lwip
    memcpy(EI_APP_aMacAddr_g, mac_addr, 6);
    return mac_addr;
#else
    return EI_APP_aMacAddr_g;
#endif
}



//*************************************************************************************************
