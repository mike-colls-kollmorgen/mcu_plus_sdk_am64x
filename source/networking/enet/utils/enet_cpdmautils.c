/*
 *  Copyright (c) Texas Instruments Incorporated 2020
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

/*!
 * \file  enet_cpdmautils.c
 *
 * \brief This file contains the implementation of the Enet CPDMA utils functions.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <stdarg.h>

#include <enet.h>
#include <enet_cfg.h>
#include <include/per/cpsw.h>

#include <include/core/enet_dma.h>
#include <include/core/enet_utils.h>
#include "include/enet_appmemutils.h"
#include "include/enet_appmemutils_cfg.h"
#include "include/enet_apputils.h"
#include "include/enet_appsoc.h"
#include "include/enet_apprm.h"


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void EnetAppUtils_freePktInfoQ(EnetDma_PktQ *pPktInfoQ)
{
    EnetDma_Pkt *pktInfo;
    uint32_t pktCnt, i;

    pktCnt = EnetQueue_getQCount(pPktInfoQ);
    /* Free all retrieved packets from DMA */
    for (i = 0U; i < pktCnt; i++)
    {
        pktInfo = (EnetDma_Pkt *)EnetQueue_deq(pPktInfoQ);
        EnetMem_freeEthPkt(pktInfo);
    }
}

uint64_t EnetAppUtils_virtToPhyFxn(const void *virtAddr,
                                   void *appData)
{
    return((uint64_t)virtAddr);
}

void *EnetAppUtils_phyToVirtFxn(uint64_t phyAddr,
                                void *appData)
{
#if defined(__aarch64__)
    uint64_t temp = phyAddr;
#else
    /* R5 is 32-bit machine, need to truncate to avoid void * typecast error */
    uint32_t temp = (uint32_t)phyAddr;
#endif
    return((void *)temp);
}

void EnetAppUtils_openTxCh(Enet_Handle hEnet,
                           uint32_t coreKey,
                           uint32_t coreId,
                           uint32_t *pTxChNum,
                           EnetDma_TxChHandle *pTxChHandle,
                           EnetCpdma_OpenTxChPrms *pCpswTxChCfg)
{
    EnetDma_Handle hDma = Enet_getDmaHandle(hEnet);

    EnetAppUtils_assert(hDma != NULL);

    pCpswTxChCfg->hEnet = hEnet;
    pCpswTxChCfg->chNum = *pTxChNum;

    *pTxChHandle = EnetDma_openTxCh(hDma, (void *)pCpswTxChCfg);
    EnetAppUtils_assert(NULL != *pTxChHandle);
}

void EnetAppUtils_closeTxCh(Enet_Handle hEnet,
                            uint32_t coreKey,
                            uint32_t coreId,
                            EnetDma_PktQ *pFqPktInfoQ,
                            EnetDma_PktQ *pCqPktInfoQ,
                            EnetDma_TxChHandle hTxChHandle,
                            uint32_t txChNum)
{
    int32_t status;

    EnetQueue_initQ(pFqPktInfoQ);
    EnetQueue_initQ(pCqPktInfoQ);

    EnetDma_disableTxEvent(hTxChHandle);
    status = EnetDma_closeTxCh(hTxChHandle, pFqPktInfoQ, pCqPktInfoQ);
    EnetAppUtils_assert(ENET_SOK == status);
}

void EnetAppUtils_openRxCh(Enet_Handle hEnet,
                           uint32_t coreKey,
                           uint32_t coreId,
                           uint32_t *pRxChNum,
                           EnetDma_RxChHandle *pRxChHandle,
                           EnetCpdma_OpenRxChPrms *pCpswRxChCfg,
                           bool allocHostMacAddr,
                           uint8_t macAddr[])
{
    EnetDma_Handle hDma = Enet_getDmaHandle(hEnet);
    int32_t status;

    EnetAppUtils_assert(hDma != NULL);

    pCpswRxChCfg->hEnet = hEnet;
    pCpswRxChCfg->chNum = *pRxChNum;

    *pRxChHandle = EnetDma_openRxCh(hDma, (void *)pCpswRxChCfg);
    EnetAppUtils_assert(NULL != *pRxChHandle);

    if (allocHostMacAddr)
    {
        status = EnetAppUtils_allocMac(hEnet,
                                       coreKey,
                                       coreId,
                                       macAddr);
        EnetAppUtils_assert(ENET_SOK == status);
        EnetAppUtils_addHostPortEntry(hEnet, coreId, macAddr);
    }
}

void EnetAppUtils_closeRxCh(Enet_Handle hEnet,
                            uint32_t coreKey,
                            uint32_t coreId,
                            EnetDma_PktQ *pFqPktInfoQ,
                            EnetDma_PktQ *pCqPktInfoQ,
                            EnetDma_RxChHandle hRxChHandle,
                            uint32_t rxChNum,
                            bool freeHostMacAddr,
                            uint8_t macAddr[])
{
    int32_t status;

    EnetQueue_initQ(pFqPktInfoQ);
    EnetQueue_initQ(pCqPktInfoQ);

    status = EnetDma_closeRxCh(hRxChHandle, pFqPktInfoQ, pCqPktInfoQ);
    EnetAppUtils_assert(ENET_SOK == status);
    if (freeHostMacAddr)
    {
        EnetAppUtils_delAddrEntry(hEnet, coreId, macAddr);
        EnetAppUtils_freeMac(hEnet,
                             coreKey,
                             coreId,
                             macAddr);
    }
}

int32_t EnetAppUtils_freeTxCh(Enet_Handle hEnet,
                              uint32_t coreKey,
                              uint32_t coreId,
                              uint32_t txChNum)
{
    int32_t status = ENET_SOK;
    //FIXME - AM273X openRxCh doesn't allocate channel and uses hard coded ch 0
    return status;
}

int32_t EnetAppUtils_freeRxFlow(Enet_Handle hEnet,
                                uint32_t coreKey,
                                uint32_t coreId,
                                uint32_t rxFlowIdx)
{
    int32_t status = ENET_SOK;
    //FIXME - AM273X openRxCh doesn't allocate channel and uses hard coded ch 0
    return status;
}

#if ((__ARM_ARCH == 7) && (__ARM_ARCH_PROFILE == 'R'))
#include <kernel/dpl/MpuP_armv7.h>
/* these are defined as part of SysConfig */

#define MPU_SIZE_KB (1024U)
#define MPU_SIZE_MB (MPU_SIZE_KB * MPU_SIZE_KB)
#define MPU_SIZE_GB (MPU_SIZE_KB * MPU_SIZE_MB)

extern MpuP_Config gMpuConfig;
extern MpuP_RegionConfig gMpuRegionConfig[];
const uint32_t gMpuRegionSizeInBytes[] = 
{
    [MpuP_RegionSize_32]   =  32,
    [MpuP_RegionSize_64]   =  64,
    [MpuP_RegionSize_128]  = 128,
    [MpuP_RegionSize_256]  = 256,
    [MpuP_RegionSize_512]  = 512,
    [MpuP_RegionSize_1K]   = (1   * MPU_SIZE_KB),
    [MpuP_RegionSize_2K]   = (2   * MPU_SIZE_KB),
    [MpuP_RegionSize_4K]   = (4   * MPU_SIZE_KB),
    [MpuP_RegionSize_8K]   = (8   * MPU_SIZE_KB),
    [MpuP_RegionSize_16K]  = (16  * MPU_SIZE_KB),
    [MpuP_RegionSize_32K]  = (32  * MPU_SIZE_KB),
    [MpuP_RegionSize_64K]  = (64  * MPU_SIZE_KB),
    [MpuP_RegionSize_128K] = (128 * MPU_SIZE_KB),
    [MpuP_RegionSize_256K] = (256 * MPU_SIZE_KB),
    [MpuP_RegionSize_512K] = (512 * MPU_SIZE_KB),
    [MpuP_RegionSize_1M] =   (1   * MPU_SIZE_MB),
    [MpuP_RegionSize_2M] =   (2   * MPU_SIZE_MB),
    [MpuP_RegionSize_4M] =   (4   * MPU_SIZE_MB),
    [MpuP_RegionSize_8M] =   (8   * MPU_SIZE_MB),
    [MpuP_RegionSize_16M] =  (16  * MPU_SIZE_MB),
    [MpuP_RegionSize_32M] =  (32  * MPU_SIZE_MB),
    [MpuP_RegionSize_64M] =  (64  * MPU_SIZE_MB),
    [MpuP_RegionSize_128M] = (128 * MPU_SIZE_MB),
    [MpuP_RegionSize_256M] = (256 * MPU_SIZE_MB),
    [MpuP_RegionSize_512M] = (512 * MPU_SIZE_MB),
    [MpuP_RegionSize_1G] =   (1   * MPU_SIZE_GB),
    [MpuP_RegionSize_2G] =   (2   * MPU_SIZE_GB),
};


bool EnetAppUtils_isDescCached(void)
{
    int32_t i;
    Enet_Type enetType;
    uintptr_t descStartAddr, descEndAddr;
    uint32_t descSize;

#if defined(SOC_AM263X)
    enetType = ENET_CPSW_3G;
#elif (defined(SOC_AM273X) || defined(SOC_AWR294X))
    enetType = ENET_CPSW_2G;
#else
#error "Unknown SOC"
#endif

    EnetSoc_getCppiDescInfo(enetType,0, &descStartAddr, &descSize);
    descEndAddr   = (uintptr_t) (descStartAddr + descSize);
    for (i = (gMpuConfig.numRegions - 1); i >= 0; i--)
    {
        
        if (gMpuRegionConfig[i].size != MpuP_RegionSize_4G)
        {
            EnetAppUtils_assert(gMpuRegionConfig[i].size >= MpuP_RegionSize_32);
            EnetAppUtils_assert(gMpuRegionConfig[i].size <= ENET_ARRAYSIZE(gMpuRegionSizeInBytes));
            if ((descStartAddr >= gMpuRegionConfig[i].baseAddr)
                &&
                (descEndAddr <= gMpuRegionConfig[i].baseAddr + gMpuRegionSizeInBytes[gMpuRegionConfig[i].size]))
            {
                break;
            }
        }
    }
    EnetAppUtils_assert(i >= 0);
    return (gMpuRegionConfig[i].attrs.isCacheable);
}

#endif
