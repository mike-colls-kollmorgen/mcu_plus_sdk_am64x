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
 * \file  enet_soc.c
 *
 * \brief This file contains the implementation of AM64x SoC Ethernet.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <stdarg.h>
#include <drivers/sciclient.h>
#include <drivers/hw_include/cslr_soc.h>
#include <hw_include/cslr_icss.h>
#include <hw_include/csl_cpswitch.h>
#include <enet_cfg.h>
#include <priv/mod/cpsw_ale_priv.h>
#include <priv/mod/cpsw_cpts_priv.h>
#include <priv/mod/cpsw_hostport_priv.h>
#include <priv/mod/cpsw_macport_priv.h>
#include <priv/mod/mdio_priv.h>
#include <priv/mod/cpsw_stats_priv.h>
#include <priv/mod/cpsw_clks.h>
#include <priv/core/enet_rm_priv.h>
#include <include/core/enet_utils.h>
#include <include/core/enet_osal.h>
#include <include/core/enet_soc.h>
#include <include/core/enet_per.h>
#include <include/per/cpsw.h>
#include <include/per/icssg.h>
#include <priv/per/cpsw_priv.h>
#include <priv/per/icssg_priv.h>
#include <soc/k3/cpsw_soc.h>
#include <soc/k3/icssg_soc.h>
#include <src/dma/udma/enet_udma_priv.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static uint32_t EnetSoc_getCoreDevId(void);

static uint32_t EnetSoc_getMcuEnetControl(Enet_MacPort macPort,
                                          uint32_t *modeSel);

static uint32_t EnetSoc_getMainEnetControl(Enet_MacPort macPort,
                                           uint32_t *modeSel);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* -------------------------------- CPSW 3G --------------------------------- */
#if (ENET_ENABLE_PER_CPSW == 1)
const CpswSoc_Cfg gEnetSoc_cpsw3gSocCfg =
{
    /* As per the clocking specification the mcu_sysclk0 is 1000MHz with
     * fixed /3 divider. */
    .cppiClkFreqHz = 320000000LLU,
    .dmscDevId = TISCI_DEV_CPSW0,
    .intrs =
    {
        {   /* EVNT_PEND - Event pending interrupt (CPTS) */
            .intrId     = CPSW_INTR_EVNT_PEND,
            .coreIntNum = CSLR_R5FSS0_CORE0_INTR_CPSW0_EVNT_PEND_0,
            .srcIdx     = ENET_SOC_DIRECT_INTR_SRCIDX_INVALID,
        },
        {   /* STATS_PEND - Statistics pending interrupt */
            .intrId     = CPSW_INTR_STATS_PEND0,
            .coreIntNum = CSLR_R5FSS0_CORE0_INTR_CPSW0_STAT_PEND_0,
            .srcIdx     = ENET_SOC_DIRECT_INTR_SRCIDX_INVALID,
        },
        {   /* MDIO_PEND - MDIO interrupt */
            .intrId     = CPSW_INTR_MDIO_PEND,
            .coreIntNum = CSLR_R5FSS0_CORE0_INTR_CPSW0_MDIO_PEND_0,
            .srcIdx     = ENET_SOC_DIRECT_INTR_SRCIDX_INVALID,
        },
    },
    .clocks =
    {
        .cppiClkId        = TISCI_DEV_CPSW0_CPPI_CLK_CLK,
        .rgmii250MHzClkId = TISCI_DEV_CPSW0_RGMII_MHZ_250_CLK,
        .rgmii50MHzClkId  = TISCI_DEV_CPSW0_RGMII_MHZ_50_CLK,
        .rgmii5MHzClkId   = TISCI_DEV_CPSW0_RGMII_MHZ_5_CLK,
    },
    .txChPeerThreadId = CSL_PSILCFG_DMSS_CPSW2_PSILD_THREAD_OFFSET,
    .rxChPeerThreadId = CSL_PSILCFG_DMSS_CPSW2_PSILS_THREAD_OFFSET,
    .txChCount        = CSL_PSILCFG_DMSS_CPSW2_PSILD_THREAD_CNT,

    /* Note- Though CPSW supports 64 distinct flow Ids, there are only
     * 8 policer/classifier so can effectively assign only 8 flow ids in CPSW3G */
    .rxFlowCount     = 8U,
};

#endif /*#if (ENET_ENABLE_PER_CPSW == 1)*/

#if (ENET_ENABLE_PER_ICSSG == 1)
/* --------------------------------- ICSS-G --------------------------------- */

/* ICSSG1 Dual-MAC DMA SoC data: 4 TX channels and 9 RX flows per slice */
const IcssgSoc_Cfg gEnetSoc_icssg1DMacSocCfg[ICSSG_MAC_PORT_MAX] =
{
    {
        .dmscDevId        = TISCI_DEV_PRU_ICSSG1,
        .txChPeerThreadId = CSL_PSILCFG_DMSS_ICSS_G1_PSILD_THREAD_OFFSET,
        .rxChPeerThreadId = CSL_PSILCFG_DMSS_ICSS_G1_PSILS_THREAD_OFFSET,
        .txChCount        = ICSSG_DUALMAC_TX_CH_NUM,
        .rxFlowCount      = ICSSG_DUALMAC_RX_FLOW_NUM,
    },
    {
        .dmscDevId        = TISCI_DEV_PRU_ICSSG1,
        .txChPeerThreadId = CSL_PSILCFG_DMSS_ICSS_G1_PSILD_THREAD_OFFSET +
                            ICSSG_DUALMAC_TX_CH_NUM,
        .rxChPeerThreadId = CSL_PSILCFG_DMSS_ICSS_G1_PSILS_THREAD_OFFSET +
                            1U,
        .txChCount        = ICSSG_DUALMAC_TX_CH_NUM,
        .rxFlowCount      = ICSSG_DUALMAC_RX_FLOW_NUM,
    }
};

/* ICSSG1 Switch DMA SoC data: 4 TX channels and 9 RX flows */
const IcssgSoc_Cfg gEnetSoc_icssg1SwtSocCfg =
{
    .dmscDevId        = TISCI_DEV_PRU_ICSSG1,
    .txChPeerThreadId = CSL_PSILCFG_DMSS_ICSS_G1_PSILD_THREAD_OFFSET,
    .rxChPeerThreadId = CSL_PSILCFG_DMSS_ICSS_G1_PSILS_THREAD_OFFSET,
    .txChCount        = ICSSG_SWITCH_TX_CH_NUM,
    .rxFlowCount      = ICSSG_SWITCH_RX_FLOW_NUM,
};

/* ICSSG0 Dual-MAC DMA SoC data: 4 TX channels and 9 RX flows per slice */
const IcssgSoc_Cfg gEnetSoc_icssg0DMacSocCfg[ICSSG_MAC_PORT_MAX] =
{
    {
        .dmscDevId        = TISCI_DEV_PRU_ICSSG0,
        .txChPeerThreadId = CSL_PSILCFG_DMSS_ICSS_G0_PSILD_THREAD_OFFSET,
        .rxChPeerThreadId = CSL_PSILCFG_DMSS_ICSS_G0_PSILS_THREAD_OFFSET,
        .txChCount        = ICSSG_DUALMAC_TX_CH_NUM,
        .rxFlowCount      = ICSSG_DUALMAC_RX_FLOW_NUM,
    },
    {
        .dmscDevId        = TISCI_DEV_PRU_ICSSG0,
        .txChPeerThreadId = CSL_PSILCFG_DMSS_ICSS_G0_PSILD_THREAD_OFFSET +
                            ICSSG_DUALMAC_TX_CH_NUM,
        .rxChPeerThreadId = CSL_PSILCFG_DMSS_ICSS_G0_PSILS_THREAD_OFFSET +
                            1U,
        .txChCount        = ICSSG_DUALMAC_TX_CH_NUM,
        .rxFlowCount      = ICSSG_DUALMAC_RX_FLOW_NUM,
    },
};

/* ICSSG0 Switch DMA SoC data: 4 TX channels and 9 RX flows */
const IcssgSoc_Cfg gEnetSoc_icssg0SwtSocCfg =
{
    .dmscDevId        = TISCI_DEV_PRU_ICSSG0,
    .txChPeerThreadId = CSL_PSILCFG_DMSS_ICSS_G0_PSILD_THREAD_OFFSET,
    .rxChPeerThreadId = CSL_PSILCFG_DMSS_ICSS_G0_PSILS_THREAD_OFFSET,
    .txChCount        = ICSSG_SWITCH_TX_CH_NUM,
    .rxFlowCount      = ICSSG_SWITCH_RX_FLOW_NUM,
};

#endif /*#if (ENET_ENABLE_PER_ICSSG == 1)*/

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

#if (ENET_ENABLE_PER_ICSSG == 1)
extern Icssg_Pruss gEnetSoc_PruIcssg0;
extern Icssg_Pruss gEnetSoc_PruIcssg1;
#endif

extern EnetUdma_DrvObjMemInfo gEnetSoc_dmaObjMemInfo;

int32_t EnetSoc_init(void)
{
#if (ENET_ENABLE_PER_ICSSG == 1)
    {
        memset(gEnetSoc_dmaObjMemInfo.drvObjMem, 0,
                gEnetSoc_dmaObjMemInfo.numObjs * sizeof(*(gEnetSoc_dmaObjMemInfo.drvObjMem)));

        gEnetSoc_PruIcssg0.lock = EnetOsal_createMutex();
        EnetSoc_assert((gEnetSoc_PruIcssg0.lock != NULL),
                    "Failed to create PRUICSSG0's mutex\n");

        gEnetSoc_PruIcssg1.lock = EnetOsal_createMutex();
        EnetSoc_assert((gEnetSoc_PruIcssg1.lock != NULL),
                    "Failed to create PRUICSSG1's mutex\n");
    }
#endif

    return ENET_SOK;
}

void EnetSoc_deinit(void)
{
#if (ENET_ENABLE_PER_ICSSG == 1)
    gEnetSoc_PruIcssg1.initialized = false;
    EnetOsal_deleteMutex(gEnetSoc_PruIcssg1.lock);
    gEnetSoc_PruIcssg1.lock = NULL;

    gEnetSoc_PruIcssg0.initialized = false;
    EnetOsal_deleteMutex(gEnetSoc_PruIcssg0.lock);
    gEnetSoc_PruIcssg0.lock = NULL;
#endif
}

static bool  EnetSoc_isValidEnetType(Enet_Type enetType)
{
    bool validInstanceType = false;

    #if (ENET_ENABLE_PER_CPSW == 1)
    if (enetType == ENET_CPSW_3G)
    {
        validInstanceType = true;
    }
    #endif
    #if (ENET_ENABLE_PER_ICSSG == 1)
    if ((enetType == ENET_ICSSG_DUALMAC)|| (enetType == ENET_ICSSG_SWITCH))
    {
        validInstanceType = true;
    }
    #endif
    return validInstanceType;
}

uint32_t EnetSoc_getCoreId(void)
{
    uint32_t coreId;

#if defined(BUILD_MCU1_0)
    coreId = CSL_CORE_ID_R5FSS0_0;
#else
#error "Enet AM64x SOC: Core not supported!!"
#endif

    return coreId;
}

uint32_t EnetSoc_getCoreKey(uint32_t coreId)
{
    return coreId;
}

bool EnetSoc_isCoreAllowed(Enet_Type enetType,
                           uint32_t instId,
                           uint32_t coreId)
{
    return true;
}

uint32_t EnetSoc_getMacPortMax(Enet_Type enetType,
                               uint32_t instId)
{
    uint32_t numPorts = 0U;

    if ((enetType == ENET_CPSW_3G) && (instId == 0U))
    {
        numPorts = 2;
    }
    else if (enetType == ENET_ICSSG_DUALMAC)
    {
        numPorts = 1U;
    }
    else if (enetType == ENET_ICSSG_SWITCH)
    {
        numPorts = 2U;
    }
    else
    {
        EnetSoc_assert(false, "Invalid peripheral (eneType=%u instId=%u)\n", enetType, instId);
    }

    return numPorts;
}

uint32_t EnetSoc_isIpSupported(Enet_Type enetType,
                               uint32_t instId)
{
    bool supported = false;

    if ((enetType == ENET_CPSW_3G) && (0U == instId))
    {
#if (ENET_ENABLE_PER_CPSW == 1)
        supported = true;
#else
        supported = false;
#endif
    }

    if (((enetType == ENET_ICSSG_DUALMAC) && (instId < 3U)) ||
             ((enetType == ENET_ICSSG_SWITCH) && (instId < 3U)))
    {
#if (ENET_ENABLE_PER_ICSSG == 1)
        supported = true;
#else
        supported = false;
#endif
    }

    EnetSoc_assert((EnetSoc_isValidEnetType(enetType) == true), "Invalid Enet & instId type %d, %d\n", enetType, instId);

    return supported;
}

uint32_t EnetSoc_getRxFlowCount(Enet_Type enetType,
                                uint32_t instId)
{
    uint32_t rxFlowCount = 0U;

#if (ENET_ENABLE_PER_CPSW == 1)
    /* Get SoC array index */
    if (enetType == ENET_CPSW_3G)
    {
        rxFlowCount = gEnetSoc_cpsw3gSocCfg.rxFlowCount;
    }
#endif
#if (ENET_ENABLE_PER_ICSSG == 1)
    if (enetType == ENET_ICSSG_DUALMAC)
    {
        switch (instId)
        {
            case 0:
                rxFlowCount = gEnetSoc_icssg0DMacSocCfg[0].rxFlowCount;
                break;
            case 1:
                rxFlowCount = gEnetSoc_icssg0DMacSocCfg[1].rxFlowCount;
                break;
            case 2:
                rxFlowCount = gEnetSoc_icssg1DMacSocCfg[0].rxFlowCount;
                break;
            case 3:
                rxFlowCount = gEnetSoc_icssg1DMacSocCfg[1].rxFlowCount;
                break;
            default:
                EnetSoc_assert(false, "Invalid peripheral (eneType=%u instId=%u)\n", enetType, instId);
                break;
        }
    }
    else if (enetType == ENET_ICSSG_SWITCH)
    {
        switch (instId)
        {
            case 0:
                rxFlowCount = gEnetSoc_icssg0SwtSocCfg.rxFlowCount;
                break;
            case 1:
                rxFlowCount = gEnetSoc_icssg1SwtSocCfg.rxFlowCount;
                break;
            default:
                EnetSoc_assert(false, "Invalid peripheral (eneType=%u instId=%u)\n", enetType, instId);
                break;
        }
    }
#endif

    EnetSoc_assert((EnetSoc_isValidEnetType(enetType) == true), "Invalid Enet type %d\n", enetType);

    return rxFlowCount;
}

uint32_t EnetSoc_getRxChPeerId(Enet_Type enetType,
                               uint32_t instId,
                               uint32_t chIdx)
{
    uint32_t peerChNum = 0U;

#if (ENET_ENABLE_PER_CPSW == 1)
    /* Get SoC array index */
    if (enetType == ENET_CPSW_3G)
    {

        peerChNum = gEnetSoc_cpsw3gSocCfg.rxChPeerThreadId;
    }
#endif
#if (ENET_ENABLE_PER_ICSSG == 1)
    if (enetType == ENET_ICSSG_DUALMAC)
    {
        switch (instId)
        {
            case 0:
                peerChNum = gEnetSoc_icssg0DMacSocCfg[0].rxChPeerThreadId;
                break;
            case 1:
                peerChNum = gEnetSoc_icssg0DMacSocCfg[1].rxChPeerThreadId;
                break;
            case 2:
                peerChNum = gEnetSoc_icssg1DMacSocCfg[0].rxChPeerThreadId;
                break;
            case 3:
                peerChNum = gEnetSoc_icssg1DMacSocCfg[1].rxChPeerThreadId;
                break;
            default:
                EnetSoc_assert(false, "Invalid peripheral (eneType=%u instId=%u)\n", enetType, instId);
                break;
        }
    }
    else if (enetType == ENET_ICSSG_SWITCH)
    {
        EnetSoc_assert(((chIdx == 0U) || (chIdx == 1U)), "Invalid channel index %u\n", chIdx);
        switch (instId)
        {
            case 0:
                peerChNum = gEnetSoc_icssg0SwtSocCfg.rxChPeerThreadId + chIdx;
                break;
            case 1:
                peerChNum = gEnetSoc_icssg1SwtSocCfg.rxChPeerThreadId + chIdx;
                break;
            default:
                EnetSoc_assert(false, "Invalid peripheral (eneType=%u instId=%u)\n", enetType, instId);
                break;
        }
    }
#endif
    EnetSoc_assert((EnetSoc_isValidEnetType(enetType) == true), "Invalid Enet type %d\n", enetType);

    return peerChNum;
}

uint32_t EnetSoc_getTxChCount(Enet_Type enetType,
                              uint32_t instId)
{
    uint32_t txChCount = 0U;

#if (ENET_ENABLE_PER_CPSW == 1)
    /* Get SoC array index */
    if (enetType == ENET_CPSW_3G)
    {
        txChCount = gEnetSoc_cpsw3gSocCfg.txChCount;
    }
#endif
#if (ENET_ENABLE_PER_ICSSG == 1)
    if (enetType == ENET_ICSSG_DUALMAC)
    {
        switch (instId)
        {
            case 0:
                txChCount = gEnetSoc_icssg0DMacSocCfg[0].txChCount;
                break;
            case 1:
                txChCount = gEnetSoc_icssg0DMacSocCfg[1].txChCount;
                break;
            case 2:
                txChCount = gEnetSoc_icssg1DMacSocCfg[0].txChCount;
                break;
            case 3:
                txChCount = gEnetSoc_icssg1DMacSocCfg[1].txChCount;
                break;
            default:
                EnetSoc_assert(false, "Invalid peripheral (eneType=%u instId=%u)\n", enetType, instId);
                break;
        }
    }
    else if (enetType == ENET_ICSSG_SWITCH)
    {
        switch (instId)
        {
            case 0:
                txChCount = gEnetSoc_icssg0SwtSocCfg.txChCount;
                break;
            case 1:
                txChCount = gEnetSoc_icssg1SwtSocCfg.txChCount;
                break;
            default:
                EnetSoc_assert(false, "Invalid peripheral (eneType=%u instId=%u)\n", enetType, instId);
                break;
        }
    }
#endif
    EnetSoc_assert((EnetSoc_isValidEnetType(enetType) == true), "Invalid Enet type %d\n", enetType);

    return txChCount;
}

uint32_t EnetSoc_getTxChPeerId(Enet_Type enetType,
                               uint32_t instId,
                               uint32_t chNum)
{
    uint32_t peerChNum = 0U;

#if (ENET_ENABLE_PER_CPSW == 1)
    /* Get SoC array index */
    if (enetType == ENET_CPSW_3G)
    {
        peerChNum = gEnetSoc_cpsw3gSocCfg.txChPeerThreadId;
    }
#endif
#if (ENET_ENABLE_PER_ICSSG == 1)
    if (enetType == ENET_ICSSG_DUALMAC)
    {
        switch (instId)
        {
            case 0:
                peerChNum = gEnetSoc_icssg0DMacSocCfg[0].txChPeerThreadId;
                break;
            case 1:
                peerChNum = gEnetSoc_icssg0DMacSocCfg[1].txChPeerThreadId;
                break;
            case 2:
                peerChNum = gEnetSoc_icssg1DMacSocCfg[0].txChPeerThreadId;
                break;
            case 3:
                peerChNum = gEnetSoc_icssg1DMacSocCfg[1].txChPeerThreadId;
                break;
            default:
                EnetSoc_assert(false, "Invalid peripheral (eneType=%u instId=%u)\n", enetType, instId);
                break;
        }
    }
    else if (enetType == ENET_ICSSG_SWITCH)
    {
        switch (instId)
        {
            case 0:
                peerChNum = gEnetSoc_icssg0SwtSocCfg.txChPeerThreadId;
                break;
            case 1:
                peerChNum = gEnetSoc_icssg1SwtSocCfg.txChPeerThreadId;
                break;
            default:
                EnetSoc_assert(false, "Invalid peripheral (eneType=%u instId=%u)\n", enetType, instId);
                break;
        }
    }
#endif
    EnetSoc_assert((EnetSoc_isValidEnetType(enetType) == true), "Invalid Enet type %d\n", enetType);

    /* Get PSI-L destination thread offset for Tx channel */
    peerChNum = (peerChNum + chNum);

    return peerChNum;
}

uint32_t EnetSoc_getClkFreq(Enet_Type enetType,
                            uint32_t instId,
                            uint32_t clkId)
{
    uint32_t freq = 0U;

    if (clkId == CPSW_CPPI_CLK)
    {
#if (ENET_ENABLE_PER_CPSW == 1)
        if ((enetType == ENET_CPSW_3G) && (instId == 0U))
        {
            freq = gEnetSoc_cpsw3gSocCfg.cppiClkFreqHz;
        }
#endif
#if (ENET_ENABLE_PER_ICSSG == 1)
        if ((enetType == ENET_ICSSG_DUALMAC)|| (enetType == ENET_ICSSG_SWITCH))
        {
            //TODO: Freq is hard coded here.
            freq = 320000000LLU;
        }
#endif
        EnetSoc_assert((EnetSoc_isValidEnetType(enetType) == true), "Invalid Enet type %d\n", enetType);
    }
    else
    {
        EnetSoc_assert(false, "Invalid clk id %u\n", clkId);
    }

    return freq;
}

#if (ENET_ENABLE_PER_CPSW == 1)
int32_t EnetSoc_setupIntrCfg(Enet_Type enetType,
                             uint32_t instId,
                             uint32_t intrId)
{
    const EnetSoc_IntrConnCfg *socIntrs = NULL;
    uint32_t numSocIntrs = 0U;
    uint16_t coreDevId = EnetSoc_getCoreDevId();
    uint16_t perDevId = 0U;
    int32_t status = ENET_SOK;


    if ((enetType == ENET_CPSW_3G) && (instId == 0U))
    {
        perDevId    = gEnetSoc_cpsw3gSocCfg.dmscDevId;
        socIntrs    = gEnetSoc_cpsw3gSocCfg.intrs;
        numSocIntrs = ENET_ARRAYSIZE(gEnetSoc_cpsw3gSocCfg.intrs);
    }
    else
    {
        EnetSoc_assert(false, "Invalid peripheral (eneType=%u instId=%u)\n", enetType, instId);
        status = ENET_EINVALIDPARAMS;
    }

    if (status == ENET_SOK)
    {
        status = EnetSocJ7x_setupIntrCfg(intrId, coreDevId, perDevId, socIntrs, numSocIntrs);
    }

    return status;
}

int32_t EnetSoc_releaseIntrCfg(Enet_Type enetType,
                               uint32_t instId,
                               uint32_t intrId)
{
    const EnetSoc_IntrConnCfg *socIntrs = NULL;
    uint32_t numSocIntrs = 0U;
    uint16_t coreDevId = EnetSoc_getCoreDevId();
    uint16_t perDevId = 0U;
    int32_t status = ENET_SOK;

    if ((enetType == ENET_CPSW_3G) && (instId == 0U))
    {
        perDevId    = gEnetSoc_cpsw3gSocCfg.dmscDevId;
        socIntrs    = gEnetSoc_cpsw3gSocCfg.intrs;
        numSocIntrs = ENET_ARRAYSIZE(gEnetSoc_cpsw3gSocCfg.intrs);
    }
    else
    {
        EnetSoc_assert(false, "Invalid peripheral (eneType=%u instId=%u)\n", enetType, instId);
        status = ENET_EINVALIDPARAMS;
    }

    if (status == ENET_SOK)
    {
        status = EnetSocJ7x_releaseIntrCfg(intrId, coreDevId, perDevId, socIntrs, numSocIntrs);
    }

    return status;
}

uint32_t EnetSoc_getIntrNum(Enet_Type enetType,
                            uint32_t instId,
                            uint32_t intrId)
{
    const EnetSoc_IntrConnCfg *socIntrs = NULL;
    uint32_t numSocIntrs = 0U;
    uint32_t intrNum = 0U;
    int32_t status = ENET_SOK;

    if ((enetType == ENET_CPSW_3G) && (instId == 0U))
    {
        socIntrs    = gEnetSoc_cpsw3gSocCfg.intrs;
        numSocIntrs = ENET_ARRAYSIZE(gEnetSoc_cpsw3gSocCfg.intrs);
    }
    else
    {
        EnetSoc_assert(false, "Invalid peripheral (eneType=%u instId=%u)\n", enetType, instId);
        status = ENET_EINVALIDPARAMS;
    }

    if (status == ENET_SOK)
    {
        intrNum = EnetSocJ7x_getIntrNum(intrId, socIntrs, numSocIntrs);
    }

    return intrNum;
}
#endif


uint32_t EnetSoc_getIntrTriggerType(Enet_Type enetType,
                                    uint32_t instId,
                                    uint32_t intrId)
{
    return ENETOSAL_ARM_GIC_TRIG_TYPE_LEVEL;
}
/* Stupid hack to workaround FAE board issues */
//#define ENET_MAC_ADDR_HACK (TRUE)
int32_t EnetSoc_getEFusedMacAddrs(uint8_t macAddr[][ENET_MAC_ADDR_LEN],
                                  uint32_t *num)
{
#ifndef ENET_MAC_ADDR_HACK
    CSL_main_ctrl_mmr_cfg0Regs *mmrRegs;
    uint32_t val;

    if (*num >= 1U)
    {
        mmrRegs = (CSL_main_ctrl_mmr_cfg0Regs *)(uintptr_t)CSL_CTRL_MMR0_CFG0_BASE;

        val = CSL_REG32_RD(&mmrRegs->MAC_ID0);
        macAddr[0][5] = (uint8_t)((val & 0x000000FFU) >> 0U);
        macAddr[0][4] = (uint8_t)((val & 0x0000FF00U) >> 8U);
        macAddr[0][3] = (uint8_t)((val & 0x00FF0000U) >> 16U);
        macAddr[0][2] = (uint8_t)((val & 0xFF000000U) >> 24U);

        val = CSL_REG32_RD(&mmrRegs->MAC_ID1);
        macAddr[0][1] = (uint8_t)((val & 0x000000FFU) >> 0U);
        macAddr[0][0] = (uint8_t)((val & 0x0000FF00U) >> 8U);

        *num = 1U;
    }
#else
    macAddr[0][0] = 0xF4;
    macAddr[0][1] = 0x84;
    macAddr[0][2] = 0x4c;
    macAddr[0][3] = 0xf9;
    macAddr[0][4] = 0x4d;
    macAddr[0][5] = 0x29;
    *num = 1U;
#endif
    return ENET_SOK;
}

uint32_t EnetSoc_getMacPortCaps(Enet_Type enetType,
                                uint32_t instId,
                                Enet_MacPort macPort)
{
    uint32_t linkCaps = 0U;

    switch (enetType)
    {
        case ENET_CPSW_3G:
            if (macPort <= ENET_MAC_PORT_2)
            {
                linkCaps = (ENETPHY_LINK_CAP_HD10 | ENETPHY_LINK_CAP_FD10 |
                            ENETPHY_LINK_CAP_HD100 | ENETPHY_LINK_CAP_FD100 |
                            ENETPHY_LINK_CAP_FD1000);
            }
            break;

        case ENET_ICSSG_DUALMAC:
        case ENET_ICSSG_SWITCH:
            if (macPort <= ENET_MAC_PORT_8)
            {
                linkCaps = (ENETPHY_LINK_CAP_HD10 | ENETPHY_LINK_CAP_FD10 |
                            ENETPHY_LINK_CAP_HD100 | ENETPHY_LINK_CAP_FD100 |
                            ENETPHY_LINK_CAP_FD1000);
            }
            break;

        default:
            EnetSoc_assert(false, "Invalid peripheral type: %u\n", enetType);
            break;

    }

    return linkCaps;
}

int32_t EnetSoc_getMacPortMii(Enet_Type enetType,
                              uint32_t instId,
                              Enet_MacPort macPort,
                              EnetMacPort_Interface *mii)
{
    EnetMac_LayerType *enetLayer = &mii->layerType;
    EnetMac_SublayerType *enetSublayer = &mii->sublayerType;
    uint32_t modeSel = CPSW_ENET_CTRL_MODE_RGMII;
    int32_t status = ENET_EFAIL;

    switch (enetType)
    {
        case ENET_CPSW_3G:
            status = EnetSoc_getMcuEnetControl(macPort, &modeSel);
            break;

        case ENET_ICSSG_DUALMAC:
        case ENET_ICSSG_SWITCH:
            status = EnetSoc_getMainEnetControl(macPort, &modeSel);
            break;

        default:
            EnetSoc_assert(false, "Invalid peripheral type: %u\n", enetType);
            break;
    }

    if (status == ENET_SOK)
    {
        switch (modeSel)
        {
            /* RMII */
            case CPSW_ENET_CTRL_MODE_RMII:
                *enetLayer    = ENET_MAC_LAYER_MII;
                *enetSublayer = ENET_MAC_SUBLAYER_REDUCED;
                break;

            /* RGMII */
            case CPSW_ENET_CTRL_MODE_RGMII:
                *enetLayer    = ENET_MAC_LAYER_GMII;
                *enetSublayer = ENET_MAC_SUBLAYER_REDUCED;
                break;

            default:
                status = ENET_EINVALIDPARAMS;
                break;
        }
    }

    return status;
}

static uint32_t EnetSoc_getMcuEnetControl(Enet_MacPort macPort,
                                          uint32_t *modeSel)
{
    CSL_main_ctrl_mmr_cfg0Regs *regs =
        (CSL_main_ctrl_mmr_cfg0Regs *)(uintptr_t)CSL_CTRL_MMR0_CFG0_BASE;
    int32_t status = ENET_SOK;

    switch (macPort)
    {
        case ENET_MAC_PORT_1:
            *modeSel = CSL_FEXT(regs->ENET1_CTRL, MAIN_CTRL_MMR_CFG0_ENET1_CTRL_PORT_MODE_SEL);
            break;

        case ENET_MAC_PORT_2:
            *modeSel = CSL_FEXT(regs->ENET2_CTRL, MAIN_CTRL_MMR_CFG0_ENET2_CTRL_PORT_MODE_SEL);
            break;

        default:
            status = ENET_EINVALIDPARAMS;
            break;
    }

    if (status == ENET_SOK)
    {
        switch (*modeSel)
        {
            case CPSW_ENET_CTRL_MODE_RMII:
            case CPSW_ENET_CTRL_MODE_RGMII:
                break;

            default:
                status = ENET_EINVALIDPARAMS;
                break;
        }
    }

    return status;
}

static uint32_t EnetSoc_getMainEnetControl(Enet_MacPort macPort,
                                           uint32_t *modeSel)
{
    int32_t status = ENET_SOK;

    return status;
}

static uint32_t EnetSoc_getCoreDevId(void)
{
    uint32_t coreDevId;

#if defined(BUILD_MCU1_0)
    coreDevId = TISCI_DEV_R5FSS0_CORE0;
#else
#error "Enet AM65xx SOC: Core not supported!!"
#endif

    return coreDevId;
}

int32_t EnetSoc_validateQsgmiiCfg(Enet_Type enetType,
                                  uint32_t instId)
{
    return ENET_ENOTSUPPORTED;
}

int32_t EnetSoc_mapPort2QsgmiiId(Enet_Type enetType,
                                 uint32_t instId,
                                 Enet_MacPort portNum,
                                 uint32_t *qsgmiiId)
{
    return ENET_ENOTSUPPORTED;
}
