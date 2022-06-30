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
 * Auto generated file 
 */

#include "ti_drivers_config.h"
#include <drivers/sciclient.h>

/*
 * OSPI
 */


/* Regions restricted for DMA. We should use CPU memcpy in these cases */
static OSPI_AddrRegion gOspiDmaRestrictRegions[] = 
{
    {
        .regionStartAddr = CSL_R5FSS0_ATCM_BASE,
        .regionSize      = CSL_R5FSS0_ATCM_SIZE,
    },  
    {
        .regionStartAddr = CSL_MCU_M4FSS0_IRAM_BASE,
        .regionSize      = CSL_MCU_M4FSS0_IRAM_SIZE,
    },  
    {
        .regionStartAddr = CSL_MCU_M4FSS0_DRAM_BASE,
        .regionSize      = CSL_MCU_M4FSS0_DRAM_SIZE,
    },  
    {
        .regionStartAddr = 0xFFFFFFFFU,
        .regionSize      = 0U,
    }
};

/* OSPI attributes */
static OSPI_Attrs gOspiAttrs[CONFIG_OSPI_NUM_INSTANCES] =
{
    {
        .baseAddr             = CSL_FSS0_OSPI0_CTRL_BASE,
        .dataBaseAddr         = CSL_FSS0_DAT_REG1_BASE,
        .inputClkFreq         = 133333333U,
        .intrNum              = 171U,
        .intrEnable           = FALSE,
        .intrPriority         = 4U,
        .dtrEnable            = TRUE,
        .dmaEnable            = FALSE,
        .phyEnable            = FALSE,
        .dacEnable            = FALSE,
        .xferLines            = OSPI_XFER_LINES_OCTAL,
        .chipSelect           = OSPI_CS0,
        .frmFmt               = OSPI_FF_POL0_PHA0,
        .decChipSelect        = OSPI_DECODER_SELECT4,
        .baudRateDiv          = 4,
        .dmaRestrictedRegions = gOspiDmaRestrictRegions,
    },
};
/* OSPI objects - initialized by the driver */
static OSPI_Object gOspiObjects[CONFIG_OSPI_NUM_INSTANCES];
/* OSPI driver configuration */
OSPI_Config gOspiConfig[CONFIG_OSPI_NUM_INSTANCES] =
{
    {
        &gOspiAttrs[CONFIG_OSPI0],
        &gOspiObjects[CONFIG_OSPI0],
    },
};

uint32_t gOspiConfigNum = CONFIG_OSPI_NUM_INSTANCES;

#include <drivers/ospi/v0/dma/ospi_dma.h>
OSPI_DmaConfig gOspiDmaConfig[CONFIG_OSPI_NUM_DMA_INSTANCES] =
{
};

uint32_t gOspiDmaConfigNum = CONFIG_OSPI_NUM_DMA_INSTANCES;

/*
 * BOOTLOADER
 */

/* Bootloader boot media specific arguments */
Bootloader_MemArgs gBootloader0Args =
{
    .curOffset        = 0,
    .appImageBaseAddr = 0x00000000,
};

/* Bootloader driver configuration */
Bootloader_Config gBootloaderConfig[CONFIG_BOOTLOADER_NUM_INSTANCES] =
{
    {
        &gBootloaderMemFxns,
        &gBootloader0Args,
        BOOTLOADER_MEDIA_MEM,
        0,
        0,
    },
};

uint32_t gBootloaderConfigNum = CONFIG_BOOTLOADER_NUM_INSTANCES;

/* DDR */

/* DDR register config .h file as generated from DDR SUBSYSTEM REGISTER CONFIGURATION tool
 * Make sure path to this file is specified in your application project/makefile include path
 */
#include "drivers/ddr/v0/soc/am64x_am243x/board_ddrReginit.h"

static DDR_Params gDdrParams =
{
    /* below values are set using the globals defined in drivers/ddr/v0/soc/am64x_am243x/board_ddrReginit.h */
    .clk1Freq              = DDRSS_PLL_FREQUENCY_1,
    .clk2Freq              = DDRSS_PLL_FREQUENCY_2,
    .ddrssCtlReg           = DDRSS_ctlReg,
    .ddrssPhyIndepReg      = DDRSS_phyIndepReg,
    .ddrssPhyReg           = DDRSS_phyReg,
    .ddrssCtlRegNum        = DDRSS_ctlRegNum,
    .ddrssPhyIndepRegNum   = DDRSS_phyIndepRegNum,
    .ddrssPhyRegNum        = DDRSS_phyRegNum,
    .ddrssCtlRegCount      = DDRSS_CTL_REG_INIT_COUNT,
    .ddrssPhyIndepRegCount = DDRSS_PHY_INDEP_REG_INIT_COUNT,
    .ddrssPhyRegCount      = DDRSS_PHY_REG_INIT_COUNT,
	.fshcount              = DDRSS_PLL_FHS_CNT,
};

/*
 * MMCSD
 */

/* MMCSD attributes */
static MMCSD_Attrs gMmcsdAttrs[CONFIG_MMCSD_NUM_INSTANCES] =
{
    {
        .ctrlBaseAddr         = CSL_MMCSD0_CTL_CFG_BASE,
        .ssBaseAddr           = CSL_MMCSD0_SS_CFG_BASE,
        .inputClkFreq         = 200000000U,
        .intrNum              = 165U,
        .intrEnable           = FALSE,
        .enableDma            = TRUE,
        .phyType              = MMCSD_PHY_TYPE_HW_PHY,
        .cardType             = MMCSD_CARD_TYPE_NO_DEVICE,
        .busWidth             = MMCSD_BUS_WIDTH_8BIT,
        .supportedModes       = MMCSD_SUPPORT_MMC_DS | MMCSD_SUPPORT_MMC_HS_SDR,
        .tuningType           = MMCSD_PHY_TUNING_TYPE_AUTO,
    },
};
/* MMCSD objects - initialized by the driver */
static MMCSD_Object gMmcsdObjects[CONFIG_MMCSD_NUM_INSTANCES];
/* MMCSD driver configuration */
MMCSD_Config gMmcsdConfig[CONFIG_MMCSD_NUM_INSTANCES] =
{
    {
        &gMmcsdAttrs[CONFIG_MMCSD0],
        &gMmcsdObjects[CONFIG_MMCSD0],
    },
};

uint32_t gMmcsdConfigNum = CONFIG_MMCSD_NUM_INSTANCES;


void Pinmux_init();
void PowerClock_init(void);
void PowerClock_deinit(void);

/*
 * Common Functions
 */
void System_init(void)
{
    /* DPL init sets up address transalation unit, on some CPUs this is needed
     * to access SCICLIENT services, hence this needs to happen first
     */
    Dpl_init();
    /* We should do sciclient init before we enable power and clock to the peripherals */
    /* SCICLIENT init */
    {
        int32_t retVal = SystemP_SUCCESS;

        retVal = Sciclient_init(CSL_CORE_ID_R5FSS0_0);
        DebugP_assertNoLog(SystemP_SUCCESS == retVal);
    }
    PowerClock_init();
    /* Now we can do pinmux */
    Pinmux_init();
    /* finally we initialize all peripheral drivers */
    OSPI_init();
	DDR_init(&gDdrParams);

    MMCSD_init();
}

void System_deinit(void)
{
    OSPI_deinit();
    MMCSD_deinit();
    PowerClock_deinit();
    /* SCICLIENT deinit */
    {
        int32_t         retVal = SystemP_SUCCESS;

        retVal = Sciclient_deinit();
        DebugP_assertNoLog(SystemP_SUCCESS == retVal);
    }
    Dpl_deinit();
}
