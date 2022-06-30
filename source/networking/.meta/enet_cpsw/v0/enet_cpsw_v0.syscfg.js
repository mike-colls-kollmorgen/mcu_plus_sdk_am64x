
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
    else if(peripheralName == "RMII1")
    {
        pinResource = pinmux.getPinRequirements(interfaceName, "RMII1_RXD0", "RXD0");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "RMII1_RXD1", "RXD1");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "RMII1_RX_ER", "RX_ER");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "RMII1_TXD0", "TXD0");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "RMII1_TXD1", "TXD1");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "RMII1_CRS_DV", "CRS_DV");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
    }
	else if(peripheralName == "RMII2")
    {
        pinResource = pinmux.getPinRequirements(interfaceName, "RMII2_RXD0", "RXD0");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "RMII2_RXD1", "RXD1");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "RMII2_RX_ER", "RX_ER");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "RMII2_TXD0", "TXD0");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "RMII2_TXD1", "TXD1");
        pinmux.setConfigurableDefault( pinResource, "rx", true );
        resources.push( pinResource);
        pinResource = pinmux.getPinRequirements(interfaceName, "RMII2_CRS_DV", "CRS_DV");
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

        pinmux.setPeripheralPinConfigurableDefault( rmii, "RMII0_TXD0", "rx", false);
        pinmux.setPeripheralPinConfigurableDefault( rmii, "RMII0_TXD1", "rx", false);
        pinmux.setPeripheralPinConfigurableDefault( rmii, "RMII0_TXEN", "rx", false);
        pinmux.setPeripheralPinConfigurableDefault( rmii, "RMII1_TXD0", "rx", false);
        pinmux.setPeripheralPinConfigurableDefault( rmii, "RMII1_TXD1", "rx", false);
        pinmux.setPeripheralPinConfigurableDefault( rmii, "RMII1_TXEN", "rx", false);

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

function getDmaInterface(instance) {
    let cpswInstInfo = getCpswInstInfo(instance);
    return cpswInstInfo.dmaIf;
}

function getCpswInstInfo(instance) {
    const cpswInstInfoMap = new Map(
                               [
                                 ['awr294x',{enetType: 'ENET_CPSW_2G', numMacPorts: '1', instId: '0', dmaIf:'ENET_SOC_HOSTPORT_DMA_TYPE_CPDMA', macPortList:['ENET_MAC_PORT_1']}],
                                 ['am273x', {enetType: 'ENET_CPSW_2G', numMacPorts: '1', instId: '0', dmaIf:'ENET_SOC_HOSTPORT_DMA_TYPE_CPDMA', macPortList:['ENET_MAC_PORT_1']}],
                                 ['am263x',{enetType: 'ENET_CPSW_3G', numMacPorts: '2', instId: '0', dmaIf:'ENET_SOC_HOSTPORT_DMA_TYPE_CPDMA', macPortList:['ENET_MAC_PORT_1', 'ENET_MAC_PORT_2']}],
                                 ['am243x',{enetType: 'ENET_CPSW_3G', numMacPorts: '2', instId: '0', dmaIf:'ENET_SOC_HOSTPORT_DMA_TYPE_UDMA', macPortList:['ENET_MAC_PORT_1', 'ENET_MAC_PORT_2']}],
                                 ['am64x',{enetType: 'ENET_CPSW_3G', numMacPorts: '2', instId: '0', dmaIf:'ENET_SOC_HOSTPORT_DMA_TYPE_UDMA', macPortList:['ENET_MAC_PORT_1', 'ENET_MAC_PORT_2']}],
                               ],
                             );
    let instInfo =  cpswInstInfoMap.get(common.getSocName());
    instInfo.macPortList = instInfo.macPortList.filter(function(macPort, index,arr){
        let includeEntry = true;
        if ((macPort === 'ENET_MAC_PORT_1') && (instance.DisableMacPort1 === true))
        {
            includeEntry = false;
        }
        if ((macPort === 'ENET_MAC_PORT_2') && (instance.DisableMacPort2 === true))
        {
            includeEntry = false;
        }
        return includeEntry;
    });
    instInfo.numMacPorts = instInfo.macPortList.length;
    return instInfo;
}

function getBoardConfigTemplateInfo() {
    const boardConfigTemplate = new Map(
                               [
                                 ['am64x',{Cfile: "/networking/enet_cpsw/templates/am64x_am243x/enet_board_cfg.c.xdt",
                                  Header: "/networking/enet_cpsw/templates/am64x_am243x/enet_board_cfg.h.xdt"}],
                                 ['am243x',{Cfile: "/networking/enet_cpsw/templates/am64x_am243x/enet_board_cfg.c.xdt",
                                  Header: "/networking/enet_cpsw/templates/am64x_am243x/enet_board_cfg.h.xdt"}],
                                 ['awr294x',{Cfile: "/networking/enet_cpsw/templates/awr294x/enet_board_cfg.c.xdt",
                                  Header: "/networking/enet_cpsw/templates/awr294x/enet_board_cfg.h.xdt"}],
                                 ['am273x', {Cfile: "/networking/enet_cpsw/templates/am273x/enet_board_cfg.c.xdt",
                                 Header: "/networking/enet_cpsw/templates/am273x/enet_board_cfg.h.xdt"}],
                                 ['am263x',{Cfile: "/networking/enet_cpsw/templates/am263x/enet_board_cfg.c.xdt",
                                 Header: "/networking/enet_cpsw/templates/am263x/enet_board_cfg.h.xdt"}],
                               ],
                             );
    return boardConfigTemplate.get(common.getSocName());
}

let boardConfigInfo = getBoardConfigTemplateInfo();

function getPhyMask(instance) {
    let cpswInstInfo = getCpswInstInfo(instance);
    let phyMask = '(' + '0';

    for (var i in cpswInstInfo.macPortList)
    {
        if (cpswInstInfo.macPortList[i] == 'ENET_MAC_PORT_1')
        {
            phyMask += ' | ' + '(1 << ' + instance.phyAddr1 + ' )';
        }
        if (cpswInstInfo.macPortList[i] == 'ENET_MAC_PORT_2')
        {
            phyMask += ' | ' + '(1 << ' + instance.phyAddr2 + ' )';
        }
    }
    phyMask += ')';
    return phyMask;
}

let enet_cpsw_module_name = "/networking/enet_cpsw/enet_cpsw";

let enet_cpsw_module = {

    displayName: "Enet (CPSW)",
	longDescription: "Driver for Common Port SWitch (CPSW). Support MAC, Switch and used in auto and industrial Ethernet to run TCP/IP, AVB etc. applications. TSN is supported via CPSW",
    templates: {
        "/drivers/pinmux/pinmux_config.c.xdt": {
            moduleName: enet_cpsw_module_name,
        },
        "/drivers/system/system_config.h.xdt": {
            driver_config: "/networking/enet_cpsw/templates/enet_cpsw_v0.h.xdt",
            moduleName: enet_cpsw_module_name,
        },
        "/board/board/board_config.h.xdt": {
            board_config: boardConfigInfo.Header,
            moduleName: enet_cpsw_module_name,
        },
        "/board/board/board_config.c.xdt": {
            board_config: boardConfigInfo.Cfile,
            moduleName: enet_cpsw_module_name,
        },
        "/networking/common/enet_config.c.xdt": {
            enet_mem_config: "/networking/enet_cpsw/templates/enet_app_memutils_cfg_udma.c.xdt",
            enet_syscfg_info: "/networking/enet_cpsw/templates/enet_app_syscfg_info.c.xdt",
            moduleName: enet_cpsw_module_name,
        },
        "/networking/common/enet_config.h.xdt": {
            enet_config: "/networking/enet_cpsw/templates/enet_syscfg.h.xdt",
            moduleName: enet_cpsw_module_name,
        },
        "/networking/common/enet_open.c.xdt": {
            enet_open: "/networking/enet_cpsw/templates/enet_init.c.xdt",
            enet_init_config: "/networking/enet_cpsw/templates/enet_app_cpsw_cfg.c.xdt",
            moduleName: enet_cpsw_module_name,
        },
        "/networking/common/enet_open.h.xdt": {
            enet_open: "/networking/enet_cpsw/templates/enet_init.h.xdt",
            moduleName: enet_cpsw_module_name,
        },
    },
    defaultInstanceName: "CONFIG_ENET_CPSW",
    config: [
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
            default: 15,
        },
        {
            name: "phyAddr2",
            description: "Phy Address of the port 2. Value MUST be between 0 .. 31",
            displayName: "Phy Address",
            default: 3,
        },
        {
            name: "PktPoolEnable",
            description: "Flag to enable packet allocation from enet utils library. It should be disabled to avoid utils memory wastage, in case aplication allots packet via other mechanism. (Ex- Lwip pools)",
            displayName: "Pkt Pool Flag",
            default: true,
        },
        {
            name: "TxPacketsCount",
            description: "No of Tx packets required for DMA channel",
            displayName: "Number of Tx Packets",
            default: 16,
        },
        {
            name: "RxPacketsCount",
            description: "No of Rx packets required for DMA channel",
            displayName: "Number of Rx Packets",
            default: 32,
        },
        {
            name: "McmEnable",
            description: "Flag to enable multi-client manager. Required for multi-core, multiple Enet client use cases",
            displayName: "Mcm Enable",
            default: false,
        },
        {
            name: "ExternalPhyMgmtEnable",
            description: "Flag to enable phy management in application. The enet driver internal phy functions including phy state machine is bypassed in this mode",
            displayName: "External Phy Management Enable",
            default: false,
        },
        {
            name: "AppLinkUpPortMask",
            description: "Application config to determine which macPorts should be polled for linkup to indicate link is up.Applicable in multi port scenario only",
            displayName: "AppLinkUpPortMask Config",
            default: "ANY_PORT",
            options: [
                {
                    name: "ALL_PORTS",
                },
                {
                    name: "ANY_PORT",
                },
            ],
        },
        {
            name: "DisableMacPort1",
            description: "Flag to selectively disable MACport1. For CPSW3G both external mac ports are enabled by default. Application may selectively choose to disable some external ports",
            displayName: "Disable Mac Port1",
            default: false,
        },
        {
            name: "DisableMacPort2",
            description: "Flag to selectively disable MACport2. For CPSW3G both external mac ports are enabled by default. Application may selectively choose to disable some external ports",
            displayName: "Disable Mac Port2",
            default: false,
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
    getDmaInterface,
    getCpswInstInfo,
    getPhyMask,
};

exports = enet_cpsw_module;
