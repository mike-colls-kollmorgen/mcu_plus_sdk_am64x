
let common = system.getScript("/common");
let pinmux = system.getScript("/drivers/pinmux/pinmux");

function getInterfaceName(inst, peripheralName)
{
    return `${peripheralName}`;
}

function getInterfacePinList(inst, peripheralName)
{
    let interfaceName = getInterfaceName(inst, peripheralName);
    let pinList = [];

    pinList = pinmux.getInterfacePinList(interfaceName);

    return pinList;
}


function getPeripheralRequirements(inst, peripheralName)
{
    let interfaceName = getInterfaceName(inst, peripheralName);
    let resources = [];
    let pinResource = {};

    if(peripheralName == "RGMII")
    {
        pinResource = pinmux.getPinRequirements(interfaceName, "RD0", "RD0");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "RD1", "RD1");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "RD2", "RD2");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "RD3", "RD3");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "RCTL", "RX_CTL");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "RCLK", "RX_RXC");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "TD0", "TD0");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "TD1", "TD1");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "TD2", "TD2");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "TD3", "TD3");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "TCTL", "TX_CTL");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "TCLK", "TX_RXC");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
    }
    else if(peripheralName == "RMII")
    {
        pinResource = pinmux.getPinRequirements(interfaceName, "RXD0", "RXD0");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "RXD1", "RXD1");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "RXER", "RX_ER");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "TXD0", "TXD0");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "TXD1", "TXD1");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "DV", "CRS_DV");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "REFCLK", "REFCLK");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "TXEN", "TXEN");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
    }
    else
    {
        let pinList = getInterfacePinList(inst, interfaceName);
        for(let pin of pinList)
        {
            pinResource = pinmux.getPinRequirements(interfaceName, pin);

            /* make all pins as "rx" and then override to make "rx" as false as needed  */
            pinmux.setConfigurableDefault( pinResource, "rx", true );

            resources.push( pinResource );
        }
    }

    let peripheralRequirements = {
        name: interfaceName,
        displayName: interfaceName,
        interfaceName: interfaceName,
        resources: resources,
    };

    return peripheralRequirements;
}

function pinmuxRequirements(inst) {

    let mdio = getPeripheralRequirements(inst, "MDIO");

    /* set default values for "rx" for different pins, based on use case */
    pinmux.setPeripheralPinConfigurableDefault( mdio, "MDC", "rx", false);

    if( inst.phyToMacInterfaceMode === "RMII")
    {
        let rmii = getPeripheralRequirements(inst, "RMII");

        pinmux.setPeripheralPinConfigurableDefault( rmii, "RMII_TXD0", "rx", false);
        pinmux.setPeripheralPinConfigurableDefault( rmii, "RMII_TXD1", "rx", false);
        pinmux.setPeripheralPinConfigurableDefault( rmii, "RMII_TXEN", "rx", false);

        return [mdio, rmii];
    }
    else
    {
        let rgmii = getPeripheralRequirements(inst, "RGMII");

        return [mdio, rgmii];
    }
}



function getInterfaceNameList(inst) {

    if(inst.phyToMacInterfaceMode === "RMII")
    {
        return [
            getInterfaceName(inst, "MDIO"),
            getInterfaceName(inst, "RMII" ),
        ];
    }
    else
    {
        return [
            getInterfaceName(inst, "MDIO"),
            getInterfaceName(inst, "RGMII" ),
        ];

    }
}

function getPeripheralPinNames(inst)
{
    let pinList = [];

    if(inst.phyToMacInterfaceMode === "RMII")
    {
        pinList = pinList.concat( getInterfacePinList(inst, "MDIO"),
                        getInterfacePinList(inst, "RMII" )
        );
    }
    else
    {
        pinList = pinList.concat( getInterfacePinList(inst, "MDIO"),
                        getInterfacePinList(inst, "RGMII" )
        );

    }
    return pinList;
}

let cpsw_input_clk_freq = 150000000;

const enet_config = [
    {
        clockIds        : [ "SOC_RcmPeripheralId_MSS_CPSW" ],
        clockFrequencies: [
            {
                moduleId: "SOC_RcmPeripheralId_MSS_CPSW",
                clkId   : "SOC_RcmPeripheralClockSource_SYS_CLK",
                clkRate : cpsw_input_clk_freq,
            },
        ],
    },
];

function getClockEnableIds(instance) {
    let instConfig = enet_config;
    return instConfig[0].clockIds;
}

function getClockFrequencies(inst) {

    let instConfig = enet_config;

    return instConfig[0].clockFrequencies;
}

let enet_cpsw_module_name = "/networking/enet_cpsw/enet_cpsw";

let enet_cpsw_module = {

    displayName: "Enet (CPSW)",
    templates: {
        "/drivers/pinmux/pinmux_config.c.xdt": {
            moduleName: enet_cpsw_module_name,
        },
        "/drivers/system/system_config.h.xdt": {
            driver_config: "/networking/enet_cpsw/templates/enet_cpsw_v2.h.xdt",
            moduleName: enet_cpsw_module_name,
        },
        "/drivers/system/power_clock_config.c.xdt": {
            moduleName: enet_cpsw_module_name,
        },
        "/board/board/board_open_close.c.xdt": {
            board_open: "/networking/enet_cpsw/templates/cpsw_board_open.c.xdt",
        },
        "/board/board/board_open_close.h.xdt": {
            board_open_close_config: "/networking/enet_cpsw/templates/cpsw_board_config.h.xdt",
        },
        "/drivers/system/system_config.c.xdt": {
            driver_config: "/networking/enet_cpsw/templates/enet_app_memutils_cfg.c.xdt",
            moduleName: enet_cpsw_module_name,
        },
    },
    defaultInstanceName: "CONFIG_ENET_CPSW",
    config: [
        {
            name: "mode",
            displayName: "EMAC Mode",
            default: "SWITCH",
        },
        {
            name: "phyToMacInterfaceMode",
            displayName: "RMII/RGMII",
            default: "RMII",
            options: [
                {
                    name: "RMII",
                },
                {
                    name: "RGMII",
                },
            ],
        },
        {
            name: "phyAddr1",
            description: "Phy Address of the port 1. Value MUST be between 0 .. 31",
            displayName: "Phy Address",
            default: 1,
        },
        {
            name: "PktPoolEnable",
            description: "Flag to enable allocation of Pkt pool. Not required for LWIP Based examples",
            displayName: "Pkt Pool Flag",
            default: true,
        },
        {
            name: "TxPacketsCount",
            description: "No of Tx packets required for DMA channel",
            displayName: "Number of Tx Packets",
            default: 0,
        },
        {
            name: "RxPacketsCount",
            description: "No of Rx packets required for DMA channel",
            displayName: "Number of Rx Packets",
            default: 0,
        },
        {
            name: "RingsTypes",
            description: "Number of Rings required for DMA Channel",
            displayName: "Number of Rings Types",
            default: 0,
        },
    ],
    moduleStatic: {
        modules: function(inst) {
            return [{
                name: "system_common",
                moduleName: "/system_common",
            }]
        },
    },
    pinmuxRequirements,
    getInterfaceNameList,
    getPeripheralPinNames,
    getClockEnableIds,
    getClockFrequencies,
};

exports = enet_cpsw_module;
