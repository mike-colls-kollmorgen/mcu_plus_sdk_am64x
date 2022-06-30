/*
 *  Copyright (c) Texas Instruments Incorporated 2021
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
 * \file  l2_cpsw_cfg.c
 *
 * \brief This file contains the implementation of the APIs for peripheral configuration for l2 cpsw example.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "l2_cpsw_common.h"
#include "l2_cpsw_cfg.h"
#include "l2_cpsw_dataflow.h"
#include "ti_enet_open_close.h"
#include "ti_enet_config.h"
#include "ti_board_config.h"
#include "ti_drivers_config.h"
#include <networking/enet/utils/include/enet_apputils.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

extern uint32_t Board_getEthBoardId(void);
static EnetApp_PerCtxt * EnetApp_getPerCtxt(Enet_Type enetType,
                                            uint32_t instId);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t EnetApp_init(void)
{
    int32_t status = ENET_SOK;

    gEnetApp.coreId = EnetSoc_getCoreId();

    /* Initialize all queues */
    EnetQueue_initQ(&gEnetApp.txFreePktInfoQ);

    return status;
}

void EnetApp_deinit(void)
{
    EnetAppUtils_print("Deinit complete\r\n");

}

void EnetApp_showMenu(void)
{
    EnetAppUtils_print("\nEnet L2 cpsw Menu:\r\n");
    EnetAppUtils_print(" 's'  -  Print statistics\r\n");
    EnetAppUtils_print(" 'r'  -  Reset statistics\r\n");
    EnetAppUtils_print(" 'm'  -  Show allocated MAC addresses\r\n");
    EnetAppUtils_print(" 'x'  -  Stop the test\r\n\n");
}

void EnetApp_portLinkStatusChangeCb(Enet_MacPort macPort,
                                          bool isLinkUp,
                                          void *appArg)
{
    EnetAppUtils_print("MAC Port %u: link %s\r\n",
                       ENET_MACPORT_ID(macPort), isLinkUp ? "up" : "down");
}

void EnetApp_mdioLinkStatusChange(Cpsw_MdioLinkStateChangeInfo *info,
                                             void *appArg)
{
    static uint32_t linkUpCount = 0;
    if ((info->linkChanged) && (info->isLinked))
    {
        linkUpCount++;
    }
}

void EnetApp_initEnetLinkCbPrms(Cpsw_Cfg *cpswCfg)
{
    cpswCfg->mdioLinkStateChangeCb     = EnetApp_mdioLinkStatusChange;
    cpswCfg->mdioLinkStateChangeCbArg  = &gEnetApp;
    cpswCfg->portLinkStatusChangeCb    = &EnetApp_portLinkStatusChangeCb;
    cpswCfg->portLinkStatusChangeCbArg = &gEnetApp;
}

void EnetApp_initAleConfig(CpswAle_Cfg *aleCfg)
{
    aleCfg->modeFlags = CPSW_ALE_CFG_MODULE_EN;
    aleCfg->agingCfg.autoAgingEn = true;
    aleCfg->agingCfg.agingPeriodInMs = 1000;

    aleCfg->nwSecCfg.vid0ModeEn                = true;
    aleCfg->vlanCfg.aleVlanAwareMode           = FALSE;
    aleCfg->vlanCfg.cpswVlanAwareMode          = FALSE;
    aleCfg->vlanCfg.unknownUnregMcastFloodMask = CPSW_ALE_ALL_PORTS_MASK;
    aleCfg->vlanCfg.unknownRegMcastFloodMask   = CPSW_ALE_ALL_PORTS_MASK;
    aleCfg->vlanCfg.unknownVlanMemberListMask  = CPSW_ALE_ALL_PORTS_MASK;
    aleCfg->policerGlobalCfg.policingEn        = true;
    aleCfg->policerGlobalCfg.yellowDropEn      = false;
    /* Enables the ALE to drop the red colored packets. */
    aleCfg->policerGlobalCfg.redDropEn         = false;
    /* Policing match mode */
    aleCfg->policerGlobalCfg.policerNoMatchMode = CPSW_ALE_POLICER_NOMATCH_MODE_GREEN;
}

void EnetApp_updateCpswInitCfg(Enet_Type enetType,  uint32_t instId,   Cpsw_Cfg *cpswCfg)
{
    EnetApp_PerCtxt *perCtxt = EnetApp_getPerCtxt(enetType, instId);

    EnetAppUtils_assert(perCtxt != NULL);
    /* Prepare init configuration for all peripherals */
    EnetAppUtils_print("\nInit all configs\r\n");
    EnetAppUtils_print("----------------------------------------------\r\n");
    EnetAppUtils_print("%s: init config\r\n", perCtxt->name);

        cpswCfg->vlanCfg.vlanAware          = false;
        cpswCfg->hostPortCfg.removeCrc      = true;
        cpswCfg->hostPortCfg.padShortPacket = true;
        cpswCfg->hostPortCfg.passCrcErrors  = true;
        EnetApp_initEnetLinkCbPrms(cpswCfg);
        EnetApp_initAleConfig(&cpswCfg->aleCfg);
}

int32_t EnetApp_open(EnetApp_PerCtxt *perCtxts,
                           uint32_t numPerCtxts)
{
    uint32_t i;
    int32_t status = ENET_SOK;

    /* Do peripheral dependent initalization */
    EnetAppUtils_print("\nInit all peripheral clocks\r\n");
    EnetAppUtils_print("----------------------------------------------\r\n");
    for (i = 0U; i < numPerCtxts; i++)
    {
        EnetApp_PerCtxt *perCtxt = &perCtxts[i];
        EnetAppUtils_enableClocks(perCtxt->enetType, perCtxt->instId);
    }

        /* Create RX tasks for each peripheral */
    if (status == ENET_SOK)
    {
        EnetAppUtils_print("\nCreate RX tasks\r\n");
        EnetAppUtils_print("----------------------------------------------\r\n");
        for (i = 0U; i < numPerCtxts; i++)
        {
            EnetApp_PerCtxt *perCtxt = &perCtxts[i];

            EnetAppUtils_print("%s: Create RX task\r\n", perCtxt->name);

            EnetApp_createRxTask(perCtxt);
        }
    }

    /* Open Enet driver for all peripherals */
    EnetAppUtils_print("\nOpen all peripherals\r\n");
    EnetAppUtils_print("----------------------------------------------\r\n");
    for (i = 0U; i < numPerCtxts; i++)
    {
        EnetApp_PerCtxt *perCtxt = &perCtxts[i];
        EnetApp_HandleInfo handleInfo;

        EnetAppUtils_print("%s: Open enet\r\n", perCtxt->name);
        status = EnetApp_driverOpen(perCtxt->enetType, perCtxt->instId);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("%s: failed to open enet\r\n", perCtxt->name);
            break;
        }
        EnetApp_acquireHandleInfo(perCtxt->enetType, perCtxt->instId, &handleInfo);
        perCtxt->hEnet = handleInfo.hEnet;
        perCtxt->hMainUdmaDrv = handleInfo.hUdmaDrv;
    }

    /* Attach the core with RM */
    if (status == ENET_SOK)
    {
        EnetAppUtils_print("\nAttach core id %u on all peripherals\r\n", gEnetApp.coreId);
        EnetAppUtils_print("----------------------------------------------\r\n");
        for (i = 0U; i < numPerCtxts; i++)
        {
            EnetApp_PerCtxt *perCtxt = &perCtxts[i];
            EnetPer_AttachCoreOutArgs attachCoreOutArgs;

            EnetAppUtils_print("%s: Attach core\r\n", perCtxt->name);

            EnetApp_coreAttach(perCtxt->enetType, perCtxt->instId, gEnetApp.coreId, &attachCoreOutArgs);
            perCtxt->coreKey = attachCoreOutArgs.coreKey;
        }
    }

    /* Open DMA for peripheral/port */
    if (status == ENET_SOK)
    {
        EnetAppUtils_print("%s: Open DMA\r\n", perCtxts->name);

        status = EnetApp_openDma(perCtxts);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("%s: failed to open DMA: %d\r\n", perCtxts->name, status);
        }
    }

    if (status == ENET_SOK)
    {
        status = EnetApp_waitForLinkUp(perCtxts);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("%s: Failed to wait for link up: %d\r\n", perCtxts->name, status);
        }
    }

    EnetAppUtils_print("%s: MAC port addr: ", perCtxts->name);

    EnetAppUtils_printMacAddr(&perCtxts->macAddr[0U]);

    return status;
}

static int32_t EnetApp_getPerIdx(Enet_Type enetType, uint32_t instId, uint32_t *perIdx)
{
    uint32_t i;
    int32_t status = ENET_SOK;

    /* Initialize async IOCTL and TX timestamp semaphores */
    for (i = 0U; i < gEnetApp.numPerCtxts; i++)
    {
        EnetApp_PerCtxt *perCtxt = &(gEnetApp.perCtxt[i]);
        if ((perCtxt->enetType == enetType) && (perCtxt->instId == instId))
        {
            break;
        }
    }
    if (i < gEnetApp.numPerCtxts)
    {
        *perIdx = i;
        status = ENET_SOK;
    }
    else
    {
        status = ENET_ENOTFOUND;
    }
    return status;
}

static EnetApp_PerCtxt * EnetApp_getPerCtxt(Enet_Type enetType,
                                            uint32_t instId)
{
    uint32_t perIdx;
    int32_t status;

    status = EnetApp_getPerIdx(enetType, instId, &perIdx);
    EnetAppUtils_assert(status == ENET_SOK);
    EnetAppUtils_assert(perIdx < ENET_ARRAYSIZE(gEnetApp.perCtxt));
    return (&gEnetApp.perCtxt[perIdx]);
}

void EnetApp_close(EnetApp_PerCtxt *perCtxts,
                         uint32_t numPerCtxts)
{
    uint32_t i;

    EnetAppUtils_print("\nClose Ports for all peripherals\r\n");
    EnetAppUtils_print("----------------------------------------------\r\n");
    for (i = 0U; i < numPerCtxts; i++)
    {
        EnetApp_PerCtxt *perCtxt = &perCtxts[i];

        EnetAppUtils_print("%s: Close Port\r\n", perCtxt->name);

        EnetApp_closePort(perCtxt);
    }

    EnetAppUtils_print("\nClose DMA for all peripherals\r\n");
    EnetAppUtils_print("----------------------------------------------\r\n");
    for (i = 0U; i < numPerCtxts; i++)
    {
        EnetApp_PerCtxt *perCtxt = &perCtxts[i];

        EnetAppUtils_print("%s: Close DMA\r\n", perCtxt->name);

        EnetApp_closeDma(perCtxt);
    }

    /* Delete RX tasks created for all peripherals */
    EnetAppUtils_print("\nDelete RX tasks\r\n");
    EnetAppUtils_print("----------------------------------------------\r\n");
    for (i = 0U; i < numPerCtxts; i++)
    {
        EnetApp_destroyRxTask(&perCtxts[i]);
    }

    /* Detach core */
    EnetAppUtils_print("\nDetach core from all peripherals\r\n");
    EnetAppUtils_print("----------------------------------------------\r\n");
    for (i = 0U; i < numPerCtxts; i++)
    {
        EnetApp_PerCtxt *perCtxt = &perCtxts[i];

        EnetAppUtils_print("%s: Detach core\r\n", perCtxt->name);

        EnetApp_coreDetach(perCtxt->enetType, perCtxt->instId,
                            gEnetApp.coreId,
                            perCtxt->coreKey);
    }

    /* Close opened Enet drivers if any peripheral failed */
    EnetAppUtils_print("\nClose all peripherals\r\n");
    EnetAppUtils_print("----------------------------------------------\r\n");
    for (i = 0U; i < numPerCtxts; i++)
    {
        EnetApp_PerCtxt *perCtxt = &perCtxts[i];
        EnetAppUtils_print("%s: Close enet\r\n", perCtxt->name);
        EnetApp_releaseHandleInfo(perCtxt->enetType, perCtxt->instId);
        perCtxt->hEnet = NULL;
    }

    /* Do peripheral dependent initalization */
    EnetAppUtils_print("\nDeinit all peripheral clocks\r\n");
    EnetAppUtils_print("----------------------------------------------\r\n");
    for (i = 0U; i < numPerCtxts; i++)
    {
        EnetApp_PerCtxt *perCtxt = &perCtxts[i];
        EnetAppUtils_disableClocks(perCtxt->enetType, perCtxt->instId);
    }
}

void EnetApp_printStats(EnetApp_PerCtxt *perCtxts,
                              uint32_t numPerCtxts)
{
    Enet_IoctlPrms prms;
    Enet_MacPort macPort;
    uint32_t i, j;
    int32_t status;

    EnetAppUtils_print("\nPrint statistics\r\n");
    EnetAppUtils_print("----------------------------------------------\r\n");
    for (i = 0U; i < numPerCtxts; i++)
    {
        EnetApp_PerCtxt *perCtxt = &gEnetApp.perCtxt[i];

		for (j = 0U; j < perCtxt->macPortNum; j++)
        {
		    macPort = perCtxt->macPort[j];
			ENET_IOCTL_SET_OUT_ARGS(&prms, &gEnetApp_cpswStats);

			status = Enet_ioctl(perCtxt->hEnet, gEnetApp.coreId, ENET_STATS_IOCTL_GET_HOSTPORT_STATS, &prms);
			if (status != ENET_SOK)
			{
				EnetAppUtils_print("%s: Failed to get port stats\r\n", perCtxt->name);
				continue;
			}
			EnetAppUtils_printHostPortStats9G((CpswStats_HostPort_Ng *)&gEnetApp_cpswStats);




			EnetAppUtils_print("\n %s - Port %u statistics\r\n", perCtxt->name, ENET_MACPORT_ID(macPort));
			EnetAppUtils_print("--------------------------------\r\n");

			ENET_IOCTL_SET_INOUT_ARGS(&prms, &macPort, &gEnetApp_cpswStats);

			status = Enet_ioctl(perCtxt->hEnet, gEnetApp.coreId, ENET_STATS_IOCTL_GET_MACPORT_STATS, &prms);
			if (status != ENET_SOK)
			{
				EnetAppUtils_print("%s: Failed to get port %u stats\r\n", perCtxt->name, ENET_MACPORT_ID(macPort));
				continue;
			}

			EnetAppUtils_printMacPortStats9G((CpswStats_MacPort_Ng *)&gEnetApp_cpswStats);

			EnetAppUtils_print("\n");
		}
    }
}

void EnetApp_resetStats(EnetApp_PerCtxt *perCtxts,
                              uint32_t numPerCtxts)
{
    Enet_IoctlPrms prms;
    Enet_MacPort macPort;
    uint32_t i, j;
    int32_t status;

    EnetAppUtils_print("\nReset statistics\r\n");
    EnetAppUtils_print("----------------------------------------------\r\n");
    for (i = 0U; i < numPerCtxts; i++)
    {
        EnetApp_PerCtxt *perCtxt = &gEnetApp.perCtxt[i];

        EnetAppUtils_print("%s: Reset statistics\r\n", perCtxt->name);

        for (j = 0U; j < perCtxt->macPortNum; j++)
        {
			macPort = perCtxt->macPort[j];
			ENET_IOCTL_SET_NO_ARGS(&prms);
			status = Enet_ioctl(perCtxt->hEnet, gEnetApp.coreId, ENET_STATS_IOCTL_RESET_HOSTPORT_STATS, &prms);
			if (status != ENET_SOK)
			{
				EnetAppUtils_print("%s: Failed to reset  host port stats\r\n", perCtxt->name);
				continue;
			}

            ENET_IOCTL_SET_IN_ARGS(&prms, &macPort);
            status = Enet_ioctl(perCtxt->hEnet, gEnetApp.coreId, ENET_STATS_IOCTL_RESET_MACPORT_STATS, &prms);
            if (status != ENET_SOK)
            {
                EnetAppUtils_print("%s: Failed to reset port %u stats\r\n", perCtxt->name, ENET_MACPORT_ID(macPort));
                continue;
            }
		}
    }
}

void EnetApp_showMacAddrs(EnetApp_PerCtxt *perCtxts,
                                uint32_t numPerCtxts)
{
    uint32_t i;

    EnetAppUtils_print("\nAllocated MAC addresses\r\n");
    EnetAppUtils_print("----------------------------------------------\r\n");
    for (i = 0U; i < numPerCtxts; i++)
    {
        EnetApp_PerCtxt *perCtxt = &gEnetApp.perCtxt[i];

        EnetAppUtils_print("%s: \t", perCtxt->name);
        EnetAppUtils_printMacAddr(&perCtxt->macAddr[0U]);
    }
}

void EnetApp_initLinkArgs(Enet_Type enetType,
                          uint32_t instId,
                          EnetPer_PortLinkCfg *linkArgs,
                          Enet_MacPort macPort)
{
    EnetBoard_EthPort ethPort;
    const EnetBoard_PhyCfg *boardPhyCfg;
    CpswMacPort_Cfg *cpswMacCfg;
    EnetMacPort_LinkCfg *linkCfg = &linkArgs->linkCfg;
    EnetMacPort_Interface *mii = &linkArgs->mii;
    EnetPhy_Cfg *phyCfg = &linkArgs->phyCfg;
    int32_t status = ENET_SOK;
    EnetApp_PerCtxt *perCtxt;

    perCtxt = EnetApp_getPerCtxt(enetType, instId);

    EnetAppUtils_print("%s: Open port %u\r\n", perCtxt->name, ENET_MACPORT_ID(macPort));

    /* Setup board for requested Ethernet port */
    ethPort.enetType = perCtxt->enetType;
    ethPort.instId   = perCtxt->instId;
    ethPort.macPort  = macPort;
    ethPort.boardId  = Board_getEthBoardId();
    EnetApp_macMode2MacMii(RGMII, &ethPort.mii);

    status = EnetBoard_setupPorts(&ethPort, 1U);
    if (status != ENET_SOK)
    {
        EnetAppUtils_print("%s: Failed to setup MAC port %u\r\n", perCtxt->name, ENET_MACPORT_ID(macPort));
        EnetAppUtils_assert(false);
    }

    cpswMacCfg = linkArgs->macCfg;

    /* Set port link params */
    linkArgs->macPort = macPort;

    mii->layerType     = ethPort.mii.layerType;
    mii->sublayerType  = ethPort.mii.sublayerType;
    mii->variantType   = ENET_MAC_VARIANT_FORCED;

    linkCfg->speed     = ENET_SPEED_AUTO;
    linkCfg->duplexity = ENET_DUPLEX_AUTO;

    boardPhyCfg = EnetBoard_getPhyCfg(&ethPort);
    if (boardPhyCfg != NULL)
    {

        phyCfg->phyAddr     = boardPhyCfg->phyAddr;
        phyCfg->isStrapped  = boardPhyCfg->isStrapped;
        phyCfg->loopbackEn  = false;
        phyCfg->skipExtendedCfg = boardPhyCfg->skipExtendedCfg;
        phyCfg->extendedCfgSize = boardPhyCfg->extendedCfgSize;
        /* Setting Tx and Rx skew delay values */
        Board_TxRxDelaySet(boardPhyCfg);
        memcpy(phyCfg->extendedCfg, boardPhyCfg->extendedCfg, phyCfg->extendedCfgSize);

    }
    else
    {
        EnetAppUtils_print("%s: No PHY configuration found\r\n", perCtxt->name);
        EnetAppUtils_assert(false);
    }
}

void EnetApp_closePort(EnetApp_PerCtxt *perCtxt)
{
    Enet_IoctlPrms prms;
    Enet_MacPort macPort;
	uint32_t i;
    int32_t status;

    for (i = 0U; i < perCtxt->macPortNum; i++)
    {
        macPort = perCtxt->macPort[i];

        EnetAppUtils_print("%s: Close port %u\r\n", perCtxt->name, ENET_MACPORT_ID(macPort));

        /* Close port link */
        ENET_IOCTL_SET_IN_ARGS(&prms, &macPort);

        EnetAppUtils_print("%s: Close port %u link\r\n", perCtxt->name, ENET_MACPORT_ID(macPort));
        status = Enet_ioctl(perCtxt->hEnet, gEnetApp.coreId, ENET_PER_IOCTL_CLOSE_PORT_LINK, &prms);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("%s: Failed to close port link: %d\r\n", perCtxt->name, status);
        }
	}
}

int32_t EnetApp_waitForLinkUp(EnetApp_PerCtxt *perCtxt)
{
    Enet_IoctlPrms prms;
    Enet_MacPort macPort;
    bool linked;
	uint32_t i;
    int32_t status = ENET_SOK;

    EnetAppUtils_print("%s: Waiting for link up...\r\n", perCtxt->name);

    for (i = 0U; i < perCtxt->macPortNum; i++)
    {
        macPort = perCtxt->macPort[i];
        linked = false;

        while (gEnetApp.run && !linked)
        {
            ENET_IOCTL_SET_INOUT_ARGS(&prms, &macPort, &linked);

            status = Enet_ioctl(perCtxt->hEnet, gEnetApp.coreId, ENET_PER_IOCTL_IS_PORT_LINK_UP, &prms);
            if (status != ENET_SOK)
            {
                EnetAppUtils_print("%s: Failed to get port %u link status: %d\r\n",
                                    perCtxt->name, ENET_MACPORT_ID(macPort), status);
                linked = false;
                break;
            }

            if (!linked)
            {
                ClockP_sleep(1);
            }
		}

        if (gEnetApp.run)
        {
            EnetAppUtils_print("%s: Port %u link is %s\r\n",
                                perCtxt->name, ENET_MACPORT_ID(macPort), linked ? "up" : "down");

            /* Set port to 'Forward' state */
            if (status == ENET_SOK)
            {

                CpswAle_SetPortStateInArgs setPortStateInArgs;
                setPortStateInArgs.portNum   = CPSW_ALE_HOST_PORT_NUM;
                setPortStateInArgs.portState = CPSW_ALE_PORTSTATE_FORWARD;
                ENET_IOCTL_SET_IN_ARGS(&prms, &setPortStateInArgs);
                prms.outArgs = NULL;
                status = Enet_ioctl(perCtxt->hEnet,
                                    gEnetApp.coreId,
                                    CPSW_ALE_IOCTL_SET_PORT_STATE,
                                    &prms);
                if (status != ENET_SOK)
                {
                    EnetAppUtils_print("%s: CPSW_ALE_IOCTL_SET_PORT_STATE IOCTL failed : %d\r\n",
                                        perCtxt->name, status);
                }
                if (status == ENET_SOK)
                {
                    ENET_IOCTL_SET_NO_ARGS(&prms);
                    status = Enet_ioctl(perCtxt->hEnet,
                                        gEnetApp.coreId,
                                        ENET_HOSTPORT_IOCTL_ENABLE,
                                        &prms);
                    if (status != ENET_SOK)
                    {
                        EnetAppUtils_print("%s: Failed to enable host port: %d\r\n", perCtxt->name, status);
                    }
                }
            }
        }
	}
    return status;
}

void EnetApp_macMode2MacMii(emac_mode macMode,
                                  EnetMacPort_Interface *mii)
{
    switch (macMode)
    {
        case RMII:
            mii->layerType    = ENET_MAC_LAYER_MII;
            mii->sublayerType = ENET_MAC_SUBLAYER_REDUCED;
            mii->variantType  = ENET_MAC_VARIANT_NONE;
            break;

        case RGMII:
            mii->layerType    = ENET_MAC_LAYER_GMII;
            mii->sublayerType = ENET_MAC_SUBLAYER_REDUCED;
            mii->variantType  = ENET_MAC_VARIANT_FORCED;
            break;
        default:
            EnetAppUtils_print("Invalid MAC mode: %u\r\n", macMode);
            EnetAppUtils_assert(false);
            break;
    }
}