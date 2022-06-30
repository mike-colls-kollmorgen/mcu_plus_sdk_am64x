
let common = system.getScript("/common");
let pinmux = system.getScript("/drivers/pinmux/pinmux");
let soc = system.getScript(`/drivers/fsi_rx/soc/fsi_rx_${common.getSocName()}`);

function getStaticConfigArr() {
    return system.getScript(`/drivers/fsi_rx/soc/fsi_rx_${common.getSocName()}`).getStaticConfigArr();
}

function getInstanceConfig(moduleInstance) {
    let solution = moduleInstance[getInterfaceName(moduleInstance)].$solution;
    let staticConfigArr = getStaticConfigArr();
    let staticConfig = staticConfigArr.find( o => o.name === solution.peripheralName);

    return {
        ...staticConfig,
        ...moduleInstance
    }
}

function pinmuxRequirements(inst) {
    let interfaceName = getInterfaceName(inst);

    let resources = [];
    let pinResource = {};

    pinResource = pinmux.getPinRequirements(interfaceName, "CLK", "FSI Clock");
    pinmux.setConfigurableDefault( pinResource, "rx", true );
    resources.push( pinResource);
    pinResource = pinmux.getPinRequirements(interfaceName, "D0", "FSI RX Data 0");
    pinmux.setConfigurableDefault( pinResource, "rx", true );
    resources.push( pinResource);
    pinResource = pinmux.getPinRequirements(interfaceName, "D1", "FSI RX Data 1");
    pinmux.setConfigurableDefault( pinResource, "rx", true );
    resources.push( pinResource);

    let peripheral = {
        name          : interfaceName,
        displayName   : "FSI RX Instance",
        interfaceName : interfaceName,
        resources : resources,
    };

    return [peripheral];
}

function getInterfaceName(inst) {
    return soc.getInterfaceName(inst);
}

function getPeripheralPinNames(inst) {
    return [ "CLK", "D0", "D1" ];
}

function getClockEnableIds(inst) {
    let instConfig = getInstanceConfig(inst);
    return instConfig.clockIds;
}

function validate(instance, report) {
    /* None. Verified by SYSCFG based on selected pin */
}

let fsi_rx_module_name = "/drivers/fsi_rx/fsi_rx";

let fsi_rx_module = {
    displayName: "FSI_RX",
    templates: {
        "/drivers/system/system_config.h.xdt": {
            driver_config: "/drivers/fsi_rx/templates/fsi_rx.h.xdt",
            moduleName: fsi_rx_module_name,
        },
        "/drivers/pinmux/pinmux_config.c.xdt": {
            moduleName: fsi_rx_module_name,
        },
    },
    defaultInstanceName: "CONFIG_FSI_RX",
    validate: validate,
    modules: function(inst) {
        return [{
            name: "system_common",
            moduleName: "/system_common",
        }]
    },
    pinmuxRequirements,
    getInstanceConfig,
    getInterfaceName,
    getPeripheralPinNames,
    getClockEnableIds,
};

exports = fsi_rx_module;
