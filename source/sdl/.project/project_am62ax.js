let path = require('path');

let device = "am62ax";

const files_r5f = {
    common: [
        "sdl_dpl.c",
        "sdl_mcrc.c",
        "sdl_ip_mcrc.c",
        "sdl_mcrc_soc.c",
        "sdl_ip_esm.c",
        "sdl_esm.c",
        "sdl_esm_core.c",
        "sdl_esm_priv.c",
        "sdl_ip_vtm.c",
        "sdl_vtm_pvt_sensor.c",
        "sdl_vtm.c",
        "sdl_soc_vtm.c",
        "sdl_dcc.c",
        "sdl_soc_dcc.c",
        "sdl_rti.c",
        "sdl_ip_rti.c",
        "sdl_soc_rti.c",
        "sdl_pok.c",
        "sdl_ip_pok.c",
        "sdl_soc_pok.c",
        "sdl_ip_pok_defs.c",
        "sdl_ecc.c",
        "sdl_ip_ecc.c",
        "sdl_ecc_r5.c",
        "sdl_interrupt.c",
        "sdl_interrupt_handlers.c",
        "sdl_interrupt_register.c",
        "sdl_exception.c",
        "sdl_ip_tog.c",
        "sdl_tog.c",
        "sdl_soc_tog.c",
        "sdl_ip_mtog.c",
        "sdl_mtog.c",
        "sdl_soc_mtog.c",
        "sdl_ip_pbist.c ",
        "sdl_soc_pbist.c ",
        "sdl_pbist.c ",
        "sdl_ip_lbist.c",
        "sdl_soc_lbist.c",
        "sdl_lbist.c",
        "sdl_ip_rom_checksum.c",
        "sdl_rom_checksum.c",
    ],
};

const filedirs = {
    common: [
        "dpl",
        "mcrc",
        "mcrc/v0",
        "mcrc/v0/soc/am62ax",
    	"esm",
        "esm/v0",
        "esm/v0/v0_0",
        "esm/soc",
        "esm/soc/am62ax",
        "vtm",
        "vtm/v0",
        "vtm/v0/soc/am62ax",
        "dcc",
        "dcc/v0",
        "dcc/v0/soc",
        "dcc/v0/soc/am62ax",
        "rti",
        "rti/v0",
        "rti/v0/soc",
        "rti/v0/soc/am62ax",
        "pok",
        "pok/v1",
        "pok/v1/soc",
        "pok/v1/soc/am62ax",
        "ecc",
        "ecc/V0",
        "ecc/soc/am62ax",
        "r5",
        "r5/v0",
        "stog/v0",
        "stog/v0/soc",
        "stog/v0/soc/am62ax",
        "mtog",
        "mtog/v0",
        "mtog/soc/am62ax",
        "pbist",
        "pbist/v0",
        "pbist/v0/soc",
        "pbist/v0/soc/am62ax",
        "lbist",
        "lbist/v0",
        "lbist/soc",
        "lbist/soc/am62ax",
        "rom_checksum",
    ],
};

const asmfiles_r5f = {
    common: [
        "sdl_ecc_utils.S",
        "sdl_r5_utils.S",
    ],
};

const r5_macro = {
    common: [
        "R5F_CORE",
    ],
};

const cflags_r5f = {
    common: [
        "-Wno-extra",
    ],
};

const buildOptionCombos = [
	{ device: device, cpu: "r5f", cgt: "ti-arm-clang"},
];

function getComponentProperty() {
    let property = {};

    property.dirPath = path.resolve(__dirname, "..");
    property.type = "library";
    property.name = "sdl";
    property.isInternal = false;
    property.buildOptionCombos = buildOptionCombos;

    return property;
}

function getComponentBuildProperty(buildOption) {
    let build_property = {};

    build_property.filedirs = filedirs;
    if(buildOption.cpu.match(/r5f*/)) {
        build_property.files = files_r5f;
        build_property.defines = r5_macro;
        build_property.cflags = cflags_r5f;
        build_property.asmfiles = asmfiles_r5f;
    }

    return build_property;
}

module.exports = {
    getComponentProperty,
    getComponentBuildProperty,
};
