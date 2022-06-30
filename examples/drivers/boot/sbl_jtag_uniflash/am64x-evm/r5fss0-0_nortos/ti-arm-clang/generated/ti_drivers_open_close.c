/*
 *  Copyright (C) 2021 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Auto generated file - DO NOT MODIFY
 */

#include "ti_drivers_open_close.h"
#include <kernel/dpl/DebugP.h>

void Drivers_open(void)
{

    Drivers_mmcsdOpen();
    Drivers_ospiOpen();
}

void Drivers_close(void)
{
    Drivers_ospiClose();
    Drivers_mmcsdClose();
}

/*
 * OSPI
 */
/* OSPI Driver handles */
OSPI_Handle gOspiHandle[CONFIG_OSPI_NUM_INSTANCES];

/* OSPI Driver Parameters */
OSPI_Params gOspiParams[CONFIG_OSPI_NUM_INSTANCES] =
{
    {
        .ospiDmaChIndex = -1,
    },
};

void Drivers_ospiOpen(void)
{
    uint32_t instCnt;
    int32_t  status = SystemP_SUCCESS;

    for(instCnt = 0U; instCnt < CONFIG_OSPI_NUM_INSTANCES; instCnt++)
    {
        gOspiHandle[instCnt] = NULL;   /* Init to NULL so that we can exit gracefully */
    }

    /* Open all instances */
    for(instCnt = 0U; instCnt < CONFIG_OSPI_NUM_INSTANCES; instCnt++)
    {
        gOspiHandle[instCnt] = OSPI_open(instCnt, &gOspiParams[instCnt]);
        if(NULL == gOspiHandle[instCnt])
        {
            DebugP_logError("OSPI open failed for instance %d !!!\r\n", instCnt);
            status = SystemP_FAILURE;
            break;
        }
    }

    if(SystemP_FAILURE == status)
    {
        Drivers_ospiClose();   /* Exit gracefully */
    }

    return;
}

void Drivers_ospiClose(void)
{
    uint32_t instCnt;

    /* Close all instances that are open */
    for(instCnt = 0U; instCnt < CONFIG_OSPI_NUM_INSTANCES; instCnt++)
    {
        if(gOspiHandle[instCnt] != NULL)
        {
            OSPI_close(gOspiHandle[instCnt]);
            gOspiHandle[instCnt] = NULL;
        }
    }

    return;
}

/*
 * MMCSD
 */
/* MMCSD Device Data structures */

/* MMCSD temporary data buffers */


/* MMCSD Driver handles */
MMCSD_Handle gMmcsdHandle[CONFIG_MMCSD_NUM_INSTANCES];

/* MMCSD Driver Parameters */
MMCSD_Params gMmcsdParams[CONFIG_MMCSD_NUM_INSTANCES] =
{
    {
    },
};

void Drivers_mmcsdOpen(void)
{
    uint32_t instCnt;
    int32_t  status = SystemP_SUCCESS;

    for(instCnt = 0U; instCnt < CONFIG_MMCSD_NUM_INSTANCES; instCnt++)
    {
        gMmcsdHandle[instCnt] = NULL;   /* Init to NULL so that we can exit gracefully */
    }

    /* Open all instances */
    for(instCnt = 0U; instCnt < CONFIG_MMCSD_NUM_INSTANCES; instCnt++)
    {
        gMmcsdHandle[instCnt] = MMCSD_open(instCnt, &gMmcsdParams[instCnt]);
        if(NULL == gMmcsdHandle[instCnt])
        {
            DebugP_logError("MMCSD open failed for instance %d !!!\r\n", instCnt);
            status = SystemP_FAILURE;
            break;
        }
    }

    if(SystemP_FAILURE == status)
    {
        Drivers_mmcsdClose();   /* Exit gracefully */
    }

    return;
}

void Drivers_mmcsdClose(void)
{
    uint32_t instCnt;

    /* Close all instances that are open */
    for(instCnt = 0U; instCnt < CONFIG_MMCSD_NUM_INSTANCES; instCnt++)
    {
        if(gMmcsdHandle[instCnt] != NULL)
        {
            MMCSD_close(gMmcsdHandle[instCnt]);
            gMmcsdHandle[instCnt] = NULL;
        }
    }

    return;
}

