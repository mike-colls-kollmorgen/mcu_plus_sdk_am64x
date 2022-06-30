/*
 * Copyright (c) 2001,2002 Florian Schulze.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the authors nor the names of the contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * test.c - This file is part of lwIP test
 *
 */

/* C runtime includes */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* lwIP core includes */
#include "lwip/opt.h"
#include "test_enet_lwip.h"
/* SDK includes */
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"
#include "ti_board_config.h"
#include "ti_enet_open_close.h"
#include "ti_enet_config.h"
#include <kernel/dpl/TaskP.h>
#include <kernel/dpl/ClockP.h>
#if (ENET_SYSCFG_ENABLE_EXTPHY == 1)
#include "enetextphy.h"
#include "enetextphy_phymdio_dflt.h"
#include "test_enet_extphy.h"
#endif

extern void Board_cpswMuxSel(void);
extern void Board_TxRxDelaySet(const EnetBoard_PhyCfg *boardPhyCfg);
extern uint32_t Board_getEthBoardId(void);
extern uint32_t Board_getEthType(void);

static void EnetApp_addMCastEntry(Enet_Type enetType,
                                  uint32_t instId,
                                  uint32_t coreId,
                                  const uint8_t *testMCastAddr,
                                  uint32_t portMask);
#if (ENET_SYSCFG_ENABLE_EXTPHY == 1U)
#include "portmacro.h"

#define ENETAPP_EXT_PHY_NUM_ENABLED_PORTS          (1U)

static void EnetApp_initExtPhy(Enet_Type enetType,
                               uint32_t instId,
                               Enet_MacPort macPort);
static void EnetApp_enableMdioStateMachine(Enet_Type enetType,
                                           uint32_t instId,
                                           uint32_t coreId);
static void EnetApp_waitForPhyAlive(Enet_Type enetType,
                                    uint32_t instId,
                                    uint32_t coreId,
                                    Enet_MacPort macPort);

#endif

void print_cpu_load()
{
    static uint32_t start_time = 0;
    uint32_t print_interval_in_secs = 5;
    uint32_t cur_time = ClockP_getTimeUsec()/1000;

    if(start_time==0)
    {
        start_time = cur_time;
    }
    else
    if( (cur_time-start_time) >= (print_interval_in_secs*1000) )
    {
        uint32_t cpu_load = TaskP_loadGetTotalCpuLoad();

        DebugP_log(" %6d.%3ds : CPU load = %3d.%02d %%\r\n",
            cur_time/1000, cur_time%1000,
            cpu_load/100, cpu_load%100 );

        start_time = cur_time;

        TaskP_loadResetAll();
    }
}

void EnetApp_initLinkArgs(Enet_Type enetType,
                          uint32_t instId,
                          EnetPer_PortLinkCfg *linkArgs,
                          Enet_MacPort macPort)
{
    EnetPhy_Cfg *phyCfg = &linkArgs->phyCfg;
    EnetMacPort_LinkCfg *linkCfg = &linkArgs->linkCfg;
    EnetMacPort_Interface *mii = &linkArgs->mii;
    EnetBoard_EthPort ethPort;
    const EnetBoard_PhyCfg *boardPhyCfg;
    int32_t status;

    /* Setup board for requested Ethernet port */
    ethPort.enetType = enetType;
    ethPort.instId   = instId;
    ethPort.macPort  = macPort;
    ethPort.boardId  = Board_getEthBoardId();
    ethPort.mii.layerType      = ENET_MAC_LAYER_GMII;
    ethPort.mii.sublayerType   = ENET_MAC_SUBLAYER_REDUCED;
    ethPort.mii.variantType    = ENET_MAC_VARIANT_FORCED;

    status = EnetBoard_setupPorts(&ethPort, 1U);
    EnetAppUtils_assert(status == ENET_SOK);

    if (Enet_isCpswFamily(ethPort.enetType))
    {
        CpswMacPort_Cfg *macCfg = (CpswMacPort_Cfg *)linkArgs->macCfg;
        CpswMacPort_initCfg(macCfg);
        if (EnetMacPort_isSgmii(mii) || EnetMacPort_isQsgmii(mii))
        {
            macCfg->sgmiiMode = ENET_MAC_SGMIIMODE_SGMII_WITH_PHY;
        }
        else
        {
            macCfg->sgmiiMode = ENET_MAC_SGMIIMODE_INVALID;
        }
    }

    boardPhyCfg = EnetBoard_getPhyCfg(&ethPort);
    if (boardPhyCfg != NULL)
    {
        EnetPhy_initCfg(phyCfg);
        phyCfg->phyAddr     = CONFIG_ENET_CPSW0_PHY0_ADDR;
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
        DebugP_log("No PHY configuration found for MAC port %u\r\n",
                           ENET_MACPORT_ID(ethPort.macPort));
        EnetAppUtils_assert(false);
    }

    mii->layerType     = ethPort.mii.layerType;
    mii->sublayerType  = ethPort.mii.sublayerType;
    mii->variantType   = ENET_MAC_VARIANT_FORCED;
    linkCfg->speed     = ENET_SPEED_AUTO;
    linkCfg->duplexity = ENET_DUPLEX_AUTO;

    if (Enet_isCpswFamily(ENET_CPSW_3G))
    {
        CpswMacPort_Cfg *macCfg = (CpswMacPort_Cfg *)linkArgs->macCfg;

        if (EnetMacPort_isSgmii(mii) || EnetMacPort_isQsgmii(mii))
        {
            macCfg->sgmiiMode = ENET_MAC_SGMIIMODE_SGMII_WITH_PHY;
        }
        else
        {
            macCfg->sgmiiMode = ENET_MAC_SGMIIMODE_INVALID;
        }
    }
}

int enet_lwip_example(void *args)
{
    Enet_Type enetType;
    uint32_t instId;
    int32_t status;

    Drivers_open();
    Board_driversOpen();

    Board_cpswMuxSel();

    DebugP_log("==========================\r\n");
    DebugP_log("      ENET LWIP App       \r\n");
    DebugP_log("==========================\r\n");

    EnetApp_getEnetInstInfo(&enetType,
                            &instId);


    EnetAppUtils_enableClocks(enetType, instId);
    status = EnetApp_driverOpen(enetType, instId);

    if (status != ENET_SOK)
    {
        EnetAppUtils_print("Failed to open ENET: %d\r\n", status);
    }

    EnetAppUtils_assert(status == ENET_SOK);
    {
        const uint8_t testBCastAddr[ENET_MAC_ADDR_LEN] =
        {
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
        };
        EnetApp_addMCastEntry(enetType,
                              instId,
                              EnetSoc_getCoreId(),
                              testBCastAddr,
                              CPSW_ALE_ALL_PORTS_MASK);
    }
    #if (ENET_SYSCFG_ENABLE_EXTPHY == 1U)
    {
        uint8_t numMacPorts;
        uint32_t i;
        Enet_MacPort macPortList[ENET_SYSCFG_MAX_MAC_PORTS];

        EnetApp_getEnetInstMacInfo(enetType,
                                   instId,
                                   macPortList,
                                   &numMacPorts);
        EnetApp_mdioLinkIntHandlerTask(enetType, instId);
        portENTER_CRITICAL();
        EnetApp_enableMdioStateMachine(enetType,
                                       instId,
                                       EnetSoc_getCoreId());
        for (i = 0; i < ENETAPP_EXT_PHY_NUM_ENABLED_PORTS; i++)
        {
            EnetApp_waitForPhyAlive(enetType,
                                    instId,
                                    EnetSoc_getCoreId(),
                                    macPortList[i]);
            EnetApp_initExtPhy(enetType,
                               instId,
                               macPortList[i]);
        }
        portEXIT_CRITICAL();
    }
    #endif
    /* no stdio-buffering, please! */
    //setvbuf(stdout, NULL,_IONBF, 0);
    main_loop(NULL);
    return 0;
}

static void EnetApp_addMCastEntry(Enet_Type enetType,
                                  uint32_t instId,
                                  uint32_t coreId,
                                  const uint8_t *testMCastAddr,
                                  uint32_t portMask)
{
    Enet_IoctlPrms prms;
    int32_t status;
    CpswAle_SetMcastEntryInArgs setMcastInArgs;
    uint32_t setMcastOutArgs;

    if (Enet_isCpswFamily(enetType))
    {
        Enet_Handle hEnet = Enet_getHandle(enetType, instId);

        EnetAppUtils_assert(hEnet != NULL);
        memset(&setMcastInArgs, 0, sizeof(setMcastInArgs));
        memcpy(&setMcastInArgs.addr.addr[0U], testMCastAddr,
               sizeof(setMcastInArgs.addr.addr));
        setMcastInArgs.addr.vlanId  = 0;
        setMcastInArgs.info.super = false;
        setMcastInArgs.info.numIgnBits = 0;
        setMcastInArgs.info.fwdState = CPSW_ALE_FWDSTLVL_FWD;
        setMcastInArgs.info.portMask = portMask;
        ENET_IOCTL_SET_INOUT_ARGS(&prms, &setMcastInArgs, &setMcastOutArgs);
        status = Enet_ioctl(hEnet, coreId, CPSW_ALE_IOCTL_ADD_MCAST, &prms);
        if (status != ENET_SOK)
        {
           EnetAppUtils_print("EnetTestBcastMcastLimit_AddAleEntry() failed CPSW_ALE_IOCTL_ADD_MCAST: %d\n",
                               status);
        }
    }
}



#if (ENET_SYSCFG_ENABLE_EXTPHY == 1U)

EnetExtPhy_Handle ghExtPhy[ENET_SYSCFG_MAX_MAC_PORTS] = {NULL};

static void EnetApp_initExtPhyArgs(Enet_Type enetType,
                            uint32_t instId,
                            EnetExtPhy_Mii *mii,
                            EnetExtPhy_LinkCfg *linkCfg,
                            Enet_MacPort macPort,
                            EnetExtPhy_Cfg *phyCfg)
{
    EnetBoard_EthPort ethPort;
    const EnetBoard_PhyCfg *boardPhyCfg;

    /* Setup board for requested Ethernet port */
    ethPort.instId   = instId;
    ethPort.macPort  = macPort;
    ethPort.boardId  = Board_getEthBoardId();
    ethPort.enetType = enetType;
    ethPort.mii.layerType      = ENET_MAC_LAYER_GMII;
    ethPort.mii.sublayerType   = ENET_MAC_SUBLAYER_REDUCED;
    ethPort.mii.variantType    = ENET_MAC_VARIANT_FORCED;
    *mii = (EnetExtPhy_Mii) EnetUtils_macToPhyMii(&ethPort.mii);

    boardPhyCfg = EnetBoard_getPhyCfg(&ethPort);
    if (boardPhyCfg != NULL)
    {
        EnetExtPhy_initCfg(phyCfg);
        phyCfg->phyAddr     = boardPhyCfg->phyAddr;
        phyCfg->skipExtendedCfg = boardPhyCfg->skipExtendedCfg;
        phyCfg->extendedCfgSize = boardPhyCfg->extendedCfgSize;
        /* Setting Tx and Rx skew delay values */
        Board_TxRxDelaySet(boardPhyCfg);
        memcpy(phyCfg->extendedCfg, boardPhyCfg->extendedCfg, phyCfg->extendedCfgSize);
    }
    else
    {
        DebugP_log("No PHY configuration found for MAC port %u\r\n",
                           ENET_MACPORT_ID(ethPort.macPort));
        EnetAppUtils_assert(false);
    }

    linkCfg->speed     = ENETEXTPHY_SPEED_AUTO;
    linkCfg->duplexity = ENETEXTPHY_DUPLEX_AUTO;

}

static void EnetApp_initExtPhy(Enet_Type enetType,
                               uint32_t instId,
                               Enet_MacPort macPort)
{
    Enet_Handle hEnet;
    EnetExtPhy_Mii mii;
    EnetExtPhy_LinkCfg linkCfg;
    EnetExtPhy_Cfg phyCfg;
    EnetExtPhy_MdioHandle hMdio;
    uint32_t macPortCaps;


    hEnet = Enet_getHandle(enetType, instId);
    EnetAppUtils_assert(hEnet != NULL);
    hMdio = EnetExtPhyMdioDflt_getPhyMdio();
    EnetAppUtils_assert(hEnet != NULL);
    macPortCaps = EnetSoc_getMacPortCaps(enetType, instId, macPort);

    EnetApp_initExtPhyArgs(enetType,
                           instId,
                           &mii,
                           &linkCfg,
                           macPort,
                           &phyCfg);
    ghExtPhy[ENET_MACPORT_NORM(macPort)] = EnetExtPhy_open(&phyCfg,
                               mii,
                               &linkCfg,
                               macPortCaps,
                               hMdio,
                               hEnet);
    EnetAppUtils_assert(ghExtPhy[ENET_MACPORT_NORM(macPort)] != NULL);

}

int32_t   EnetApp_getExtPhyLinkCfgInfo(Enet_Type enetType,
                                       uint32_t instId,
                                       Enet_MacPort *macPort,
                                       uint32_t phyAddr,
                                       EnetPhy_LinkCfg *phyLinkCfg)
{
    EnetExtPhy_LinkCfg extPhylinkCfg;
    int32_t status;
    uint8_t numMacPorts;
    uint32_t i;
    Enet_MacPort macPortList[ENET_SYSCFG_MAX_MAC_PORTS];

    EnetApp_getEnetInstMacInfo(enetType,
                               instId,
                               macPortList,
                               &numMacPorts);
    for (i = 0; i < numMacPorts; i++)
    {
        EnetBoard_EthPort ethPort;
        const EnetBoard_PhyCfg *boardPhyCfg;

        /* Setup board for requested Ethernet port */
        ethPort.instId   = instId;
        ethPort.macPort  = ENET_MACPORT_DENORM(i);
        ethPort.boardId  = Board_getEthBoardId();
        ethPort.enetType = enetType;
        ethPort.mii.layerType      = ENET_MAC_LAYER_GMII;
        ethPort.mii.sublayerType   = ENET_MAC_SUBLAYER_REDUCED;
        ethPort.mii.variantType    = ENET_MAC_VARIANT_FORCED;
        boardPhyCfg = EnetBoard_getPhyCfg(&ethPort);
        EnetAppUtils_assert(boardPhyCfg != NULL);
        if (phyAddr == boardPhyCfg->phyAddr)
        {
            break;
        }
    }
    EnetAppUtils_assert (i < numMacPorts);
    *macPort = ENET_MACPORT_DENORM(i);
    status = EnetExtPhy_getLinkCfg(ghExtPhy[ENET_MACPORT_NORM(*macPort)], &extPhylinkCfg);
    if (status == ENETEXTPHY_SOK)
    {
        phyLinkCfg->speed     =  (EnetPhy_Speed) extPhylinkCfg.speed;
        phyLinkCfg->duplexity =  (EnetPhy_Duplexity) extPhylinkCfg.duplexity;
    }
    return status;
}

bool EnetApp_isPortLinked(Enet_Handle hEnet)
{
    Enet_Type enetType;
    uint32_t instId;
    uint32_t linkUpMask = 0U;
    bool linkUp;
    Enet_MacPort macPortList[ENET_MAC_PORT_NUM];
    uint8_t numMacPorts;
    int32_t status;
    uint32_t i;

    status = Enet_getHandleInfo(hEnet, &enetType, &instId);
    EnetAppUtils_assert(status == ENET_SOK);
    EnetApp_getEnetInstMacInfo(enetType, instId, macPortList, &numMacPorts);
    for (i = 0; i < ENETAPP_EXT_PHY_NUM_ENABLED_PORTS; i++)
    {
        linkUpMask |= (EnetExtPhy_isLinked(ghExtPhy[i])  << i);
    }
    linkUp =  (linkUpMask != 0) ? true : false;
    return linkUp;
}

static void EnetApp_enableMdioStateMachine(Enet_Type enetType,
                                           uint32_t instId,
                                           uint32_t coreId)
{
    Enet_IoctlPrms prms;
    int32_t status;
    Enet_Handle hEnet = Enet_getHandle(enetType, instId);

    EnetAppUtils_assert(hEnet != NULL);
    ENET_IOCTL_SET_NO_ARGS(&prms);
    status = Enet_ioctl(hEnet, coreId, ENET_MDIO_IOCTL_ENABLE_STATE_MACHINE, &prms);
    if (status != ENET_SOK)
    {
       EnetAppUtils_print("EnetApp_enableMdioStateMachine() failed ENET_MDIO_IOCTL_ENABLE_STATE_MACHINE: %d\n",
                           status);
    }
    EnetAppUtils_assert(ENET_SOK == status);
}

static void EnetApp_waitForPhyAlive(Enet_Type enetType,
                                    uint32_t instId,
                                    uint32_t coreId,
                                    Enet_MacPort macPort)
{
    bool alive;
    Enet_IoctlPrms prms;
    int32_t status;
    Enet_Handle hEnet = Enet_getHandle(enetType, instId);
    int8_t phyAddr;
    EnetBoard_EthPort ethPort;
    const EnetBoard_PhyCfg *boardPhyCfg;

    EnetAppUtils_assert(hEnet != NULL);

    /* Setup board for requested Ethernet port */
    ethPort.instId   = instId;
    ethPort.macPort  = macPort;
    ethPort.boardId  = Board_getEthBoardId();
    ethPort.enetType = enetType;
    ethPort.mii.layerType      = ENET_MAC_LAYER_GMII;
    ethPort.mii.sublayerType   = ENET_MAC_SUBLAYER_REDUCED;
    ethPort.mii.variantType    = ENET_MAC_VARIANT_FORCED;
    boardPhyCfg = EnetBoard_getPhyCfg(&ethPort);
    EnetAppUtils_assert(boardPhyCfg != NULL);
    phyAddr = boardPhyCfg->phyAddr;

    do {
        ENET_IOCTL_SET_INOUT_ARGS(&prms, &phyAddr, &alive);
        status = Enet_ioctl(hEnet,
                            coreId,
                            ENET_MDIO_IOCTL_IS_ALIVE,
                            &prms);
    } while ((status == ENET_SOK) && (alive != true));
    EnetAppUtils_assert (status == ENET_SOK);
}
#endif /* #if (ENET_SYSCFG_ENABLE_EXTPHY == 1U) */

