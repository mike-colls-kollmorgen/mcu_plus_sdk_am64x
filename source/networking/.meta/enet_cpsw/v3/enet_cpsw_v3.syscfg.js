
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
        pinResource = pinmux.getPinRequirements(interfaceName, "RX_CTL", "RX_CTL");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "RXC", "RX_RXC");
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
        pinResource = pinmux.getPinRequirements(interfaceName, "TX_CTL", "TX_CTL");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "TXC", "TX_RXC");
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
        pinResource = pinmux.getPinRequirements(interfaceName, "RX_ER", "RX_ER");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "TXD0", "TXD0");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "TXD1", "TXD1");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "CRS_DV", "CRS_DV");
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


    if( inst.phyToMacInterfaceMode === "RMII")
    {
        let rmii = getPeripheralRequirements(inst, "RMII");

        pinmux.setPeripheralPinConfigurableDefault( rmii, "RMII1_TXD0", "rx", false);
        pinmux.setPeripheralPinConfigurableDefault( rmii, "RMII1_TXD1", "rx", false);
        pinmux.setPeripheralPinConfigurableDefault( rmii, "RMII1_TX_EN", "rx", false);
        pinmux.setPeripheralPinConfigurableDefault( rmii, "RMII2_TXD0", "rx", false);
        pinmux.setPeripheralPinConfigurableDefault( rmii, "RMII2_TXD1", "rx", false);
        pinmux.setPeripheralPinConfigurableDefault( rmii, "RMII2_TX_EN", "rx", false);

        return [rmii];
    }
    else
    {
        let rgmii = getPeripheralRequirements(inst, "RGMII");

        return [rgmii];
    }
}



function getInterfaceNameList(inst) {

    if(inst.phyToMacInterfaceMode === "RMII")
    {
        return [
            getInterfaceName(inst, "RMII" ),
        ];
    }
    else
    {
        return [
            getInterfaceName(inst, "RGMII" ),
        ];

    }
}

function getPeripheralPinNames(inst)
{
    let pinList = [];

    if(inst.phyToMacInterfaceMode === "RMII")
    {
        pinList = getInterfacePinList(inst, "RMII" );
    }
    else
    {
        pinList = getInterfacePinList(inst, "RGMII" );

    }
    return pinList;
}

let cpsw_input_clk_freq = 200000000;

const enet_config = [
    {
        clockIds        : [ "SOC_RcmPeripheralId_CPTS" ],
        clockFrequencies: [
            {
                moduleId: "SOC_RcmPeripheralId_CPTS",
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
            driver_config: "/networking/enet_cpsw/templates/enet_cpsw_v3.h.xdt",
            moduleName: enet_cpsw_module_name,
        },
        "/drivers/system/power_clock_config.c.xdt": {
            moduleName: enet_cpsw_module_name,
        },
        "/drivers/system/system_config.c.xdt": {
            driver_config: "/networking/enet_cpsw/templates/enet_app_memutils_cfg.c.xdt",
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
            default: 0,
        },
        {
            name: "phyAddr2",
            description: "Phy Address of the port 2. Value MUST be between 0 .. 31",
            displayName: "Phy Address",
            default: 0,
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
