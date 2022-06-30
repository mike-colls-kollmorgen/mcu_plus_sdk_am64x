
let common = system.getScript("/common");
let soc = system.getScript(`/networking/soc/networking_${common.getSocName()}`);

exports = {
    displayName: "TI Networking",
    templates: [
        {
            name: "/networking/common/enet_config.c.xdt",
            outputPath: "ti_enet_config.c",
            alwaysRun: true,
        },
        {
            name: "/networking/common/enet_config.h.xdt",
            outputPath: "ti_enet_config.h",
            alwaysRun: true,
        },
        {
            name: "/networking/common/enet_open.c.xdt",
            outputPath: "ti_enet_open_close.c",
            alwaysRun: true,
        },
        {
            name: "/networking/common/enet_open.h.xdt",
            outputPath: "ti_enet_open_close.h",
            alwaysRun: true,
        },
    ],
    topModules: soc.getTopModules(),
};
