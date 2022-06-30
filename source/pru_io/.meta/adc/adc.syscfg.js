// This module shows the basic configurable types that are supported for ADC.
//
let common = system.getScript("/common");

function getInstanceConfig(moduleInstance) {
    return {
        ...moduleInstance
    }
}

// ------------------------------------------------------------------------
//  TODO: Make functions to return config array according to the selected ADC IC
// ------------------------------------------------------------------------
var getAdcConfig = function (inst, ui) {
    let hideConfig = true;
    if(inst.adcIC == "ADS8598H" || inst.adcIC == "ADS8588H" || inst.adcIC == "ADS8598S") {
        ui.overSampling.hidden = !hideConfig;
        ui.inputRange.hidden   = !hideConfig;
        ui.reference.hidden    = !hideConfig;
        }
    };

function onValidate(inst, report) {
    /* None. Verified by SYSCFG based on selected pin */
}

function sharedModuleInstances(instance) {
    let modInstances = new Array();

    // should this come under sharedModuleInstances or moduleInstances?
    modInstances.push({
        name: "icss",
        displayName: "PRU Configuration",
        moduleName: '/drivers/pruicss/pruicss',
        requiredArgs: {
            instance: instance.icssInstance,
            // clock speeds fixed for now
            coreClk: 333333333,
            iepSyncMode: false,
            iepClk: 500*1000000,
        },
    });

    return (modInstances);
}

function moduleInstances(instance) {
    let modInstances = new Array();

    modInstances.push({
        name: "adcConfig",
        displayName: "ADC Configuration",
        moduleName: '/pru_io/adc/ads8598h_pinmux',
        useArray: true,
        minInstanceCount : 1,
        defaultInstanceCount: 1,
        maxInstanceCount : 1,
        requiredArgs: {
            icssInstance: instance.icssInstance,
            adaptor: instance.adaptor,
        },
        collapsed: false,
    });

    modInstances.push({
        name: "pruIpc",
        displayName: "PRU IPC",
        moduleName: '/pru_io/pru_ipc/pru_ipc',
        collapsed: false,
        useArray: true,
        minInstanceCount : 0,
        defaultInstanceCount: 0,
        args: {
            icssInstance: instance.icssInstance,
            noOfBuffers: parseInt(instance.channelsInUse),
            pruCore: "PRU0",
        },
    });

    return (modInstances);
}

function getAdcPruCore(instance) {
    return  instance.adcConfig.PRU_ICSSG0_PRU?.$solution.peripheralName.substring(11) ||
            instance.adcConfig.PRU_ICSSG1_PRU?.$solution.peripheralName.substring(11);
}

let adc_top_module_name = "/pru_io/adc/adc";

let adc_top_module = {
    displayName: "ADC",

    templates: {
        "/pru_io/common/pru_io_config.inc.xdt": {
            pru_io_config: "/pru_io/adc/templates/adc_config.inc.xdt",
            moduleName: adc_top_module_name,
        },
    },
    defaultInstanceName: "CONFIG_ADC",
    config: [
        {
            name: "adcIC",
            displayName: "ADC IC",
            options: [{
                name: "ADS8598H",
            },
            {
                name: "ADS8598S",
            },
            {
                name: "ADS8588H",
            },
            {
                name: "ADS8588S",
            }],
            default: "ADS8598H",
            // onChange: getAdcConfig,
        },
        {
            name: "adaptor",
            displayName: "Adaptor Card",
            options: [{
                name: "initialAdapterBoard",
                displayName: "T&M SEM Adapter Board",
                description: "Adaptor Board with both ADC and DAC interface options",
            },
            {
                name: "upgradedAdapterBoard",
                displayName: "AM64x Adapter Board",
            }],
            default: "initialAdapterBoard",
            getDisabledOptions: () => {
                return [{
                    name: "upgradedAdapterBoard",
                    reason: "The board is yet to be released"
                }]
            },
        },
        {
            name: "icssInstance",
            displayName: "ICSSG Instance",
            default: "ICSSG0",
            options: [{
                name: "ICSSG0",
            },
            {
                name: "ICSSG1",
            }],
            getDisabledOptions: () => {
                return [{
                    name: "ICSSG1",
                    reason: "The T&M SEM Adapter Board only supports ICSSG0"
                }]
            },
        },
        {
            name: "channelsInUse",
            displayName: "Channels In Use",
            description: "Number of adc channels to use for sampling from 1 to n",
            options: [
                { name: "1", },
                { name: "2", },
                { name: "3", },
                { name: "4", },
                { name: "5", },
                { name: "6", },
                { name: "7", },
                { name: "8", },
            ],
            default: "1",
        },
    ],
    validate: onValidate,
    sharedModuleInstances,
    moduleInstances,
    getInstanceConfig,
};

exports = adc_top_module;