# Booting Tools {#TOOLS_BOOT}

[TOC]

\note To see the exact sequence of steps in which applications and secondary bootloader (SBL) are converted from compiler generated .out files to
      boot images, see the makefile `makefile_ccs_bootimage_gen` that is included in every example and secondary bootloader (SBL) CCS project.

\note If you are using makefile based build, then see the file named `makefile` in the example folder.

## Introduction

This section describes the various tools that are used to create boot images for all the SDK applications

## Important files and folders

<table>
<tr>
    <th>Folder/Files
    <th>Description
</tr>
<tr><td colspan="2" bgcolor=#F0F0F0> ${SDK_INSTALL_PATH}/tools/boot/</td></tr>
<tr>
    <td>multicoreImageGen/
    <td>Tool to combine multiple RPRC into a single binary
</tr>
<tr>
    <td>out2rprc/
    <td>Tool to convert compiler generated ELF .out for a CPU to a compact and loadable binary representation, called RPRC.
</tr>
<tr>
    <td>sbl_prebuilt/
    <td>Pre-built secondary bootloader (SBL) images and flash configuration files for different supported EVMs, see also \ref TOOLS_FLASH
</tr>
<tr>
    <td>signing/
    <td>Security signing scripts need to create boot images that can be booted by ROM bootloader (RBL)
</tr>
<tr>
    <td>xipGen/
    <td>Tool to split a RPRC file generated from `out2rprc` into two files containing non-XIP and XIP sections.
</tr>
<tr>
    <td>uart_bootloader.py
    <td>Python script used to send the SBL and appimage binaries over UART using XMODEM protocol in UART boot mode
</tr>
<tr>
    <td>uart_uniflash.py
    <td>Python script used to flash SBL and applications to EVM flash using UART. See \ref TOOLS_FLASH for more details.
</tr>
</table>

## Out2RPRC

- This tool converts the application executable (.out) into custom TI RPRC (.rprc) image - an image loadable by the secondary bootloader (SBL).
- This tool strips out the initialized sections from the executable file (*.out) and places them in a compact format that the SBL can understand.
- The output RPRC file is typically much smaller than the original executable (*.out) file.
- The RPRC files are intermediate files in a format that is consumed by `MulticoreImageGen` tool that generates the final binary that is flashed (`*.appimage`)
- The RPRC file format contains header to various sections in the executable like section run address, size and
  a overall header which mentions the number of sections and the start offset to the first section.
- The RPRC magic word is `0x43525052` - which is ASCII equivalent for `RPRC`
- Shown below is the file header and section format for RPRC files.

    \imageStyle{tools_rprc_format.png,width:40%}
    \image html tools_rprc_format.png "RPRC File Format"

- This tool is provided as a minified JS script. To convert the application executable into RPRC image file, it can be used as
  \code
  cd ${SDK_INSTALL_PATH}/tools/boot/out2rprc
  ${NODE} elf2rprc.js {input application executable file (.out)}
  \endcode

## Multi-core Image Gen

- This tool converts the RPRC files created for each CPU into a single combined multicore application image that can be booted by the secondary bootloader (SBL)
- Shown below is the file format for multicore image files.

  \imageStyle{tools_multicore_format.png,width:60%}
  \image html tools_multicore_format.png "Multi-core Image File Format"

- The number of meta headers present is equal to the number of cores included.
- The meta header magic word is `0x5254534D` - which is ASCII equivalent for `MSTR`
- In Windows or Linux, use the following command to convert RPRC images into a multicore `.appimage` file
    \code
    cd ${SDK_INSTALL_PATH}/tools/boot/multicoreImageGen
    ${NODE} multicoreImageGen.js --devID {DEV_ID} --out {Output image file (.appimage)} {core 1 rprc file}@{core 1 id} [ {core n rprc file}@{core n id} ... ]
    \endcode

\cond SOC_AM64X || SOC_AM243X

- In case of @VAR_SOC_NAME, `DEV_ID` is `55`.
- The various core ID to be used are as below.

CORE        | CORE ID
------------|--------
r5fss0-0    | 4
r5fss0-1    | 5
r5fss1-0    | 6
r5fss1-1    | 7
m4fss0-0    | 14

\endcond

\cond SOC_AM273X

- In case of @VAR_SOC_NAME, `DEV_ID` is `55`.
- The various core ID to be used are as below.

CORE        | CORE ID
------------|--------
r5fss0-0    | 0
r5fss0-1    | 1
c66xdsp_0   | 2

\endcond

\cond SOC_AWR294X

- In case of @VAR_SOC_NAME, `DEV_ID` is `55`.
- The various core ID to be used are as below.

CORE        | CORE ID
------------|--------
r5fss0-0    | 0
r5fss0-1    | 1
c66xdsp_0   | 2
r4          | 3

\endcond

\cond SOC_AM62X

- In case of @VAR_SOC_NAME, `DEV_ID` is `55`.
- The various core ID to be used are as below.

CORE          | CORE ID
--------------|--------
r5fss0-0      | 4
m4fss0-0      | 5
hsm-m4fss0-0  | 6

\endcond

\cond SOC_AM62AX

- In case of @VAR_SOC_NAME, `DEV_ID` is `55`.
- The various core ID to be used are as below.

CORE          | CORE ID
--------------|--------
r5fss0-0      | 4
mcu-r5fss0-0  | 5
hsm-m4fss0-0  | 6
c75ss0-0      | 7

\endcond

\cond SOC_AM62PX

- In case of @VAR_SOC_NAME, `DEV_ID` is `55`.
- The various core ID to be used are as below.

CORE          | CORE ID
--------------|--------
r5fss0-0      | 4
mcu-r5fss0-0  | 5
hsm-m4fss0-0  | 6

\endcond


## Signing Scripts {#TOOLS_BOOT_SIGNING}

- To run these scripts, one needs `openssl` installed as mentioned here, \ref INSTALL_OPENSSL
- Signing scripts are a collection of scripts needed to sign ROM images (image booted by ROM - mostly the SBL) and application images (image booted by the SBL)
- The RBL requires the boot image (mostly SBL), to be signed always, even if we are not using secure boot.
- We follow a combined boot method for ROM images. Here the ROM Bootloader (RBL) boots the SBL, SYSFW and BOARDCFG together. The boot image would be a binary concatenation of x509 Certificate, SBL, SYSFW, BOARDCFG (and the SYSFW inner certificate in case of HS device) binary blobs. We use a python script to generate this final boot image. This script has a dependency on `openssl` as mentioned before, so make sure you've installed it. To generate a combined boot image, one can do as below:

- For GP devices
  \code
  cd ${SDK_INSTALL_PATH}/tools/boot/signing
  ${PYTHON} rom_image_gen.py --swrv 1 --sbl-bin  <path-to-sbl-binary> --sysfw-bin <path-to-sysfw-binary> --boardcfg-blob <path-to-boardcfg-binary-blob> --boardcfg-sbldata-blob <path-to-boardcfg-sbldata-blob> --sbl-loadaddr ${SBL_RUN_ADDRESS} --sysfw-loadaddr ${SYSFW_LOAD_ADDR} --bcfg-loadaddr ${BOARDCFG_LOAD_ADDR} --bcfg-sbldata-loadaddr ${BOARDCFG_SBLDATA_LOAD_ADDR} --key ${BOOTIMAGE_CERT_KEY} --rom-image <path-to-output-image> --enable-sbldata yes
  \endcode
  
- For HS devices, we have to pass the HS SYSFW binaries and also the SYSFW inner certificate to the signing script.
  \code
  cd ${SDK_INSTALL_PATH}/tools/boot/signing
  ${PYTHON} rom_image_gen.py --swrv 1 --sbl-bin <path-to-sbl-binary> --sysfw-bin <path-to-sysfw-binary> --sysfw-inner-cert <path-to-sysfw-inner-cert-binary> --boardcfg-blob <path-to-boardcfg-binary-blob> --boardcfg-sbldata-blob <path-to-boardcfg-sbldata-blob> --sbl-loadaddr ${SBL_RUN_ADDRESS} --sysfw-loadaddr ${SYSFW_LOAD_ADDR} --bcfg-loadaddr ${BOARDCFG_LOAD_ADDR} --bcfg-sbldata-loadaddr ${BOARDCFG_SBLDATA_LOAD_ADDR} --key ${BOOTIMAGE_CERT_KEY} --rom-image <path-to-output-image> --enable-sbldata yes
  \endcode

- For SBL images or examples which is loaded by SBL, we use a different signing script. This is solely because of the x509 certificate template differences between ROM and SYSFW. In GP devices appimages are not signed. The signing happens only in HS devices. The script usage is:
  \code
  cd ${SDK_INSTALL_PATH}/tools/boot/signing
  $(PYTHON) appimage_x509_cert_gen.py --bin <path-to-the-binary> --authtype 0 --loadaddr 84000000 --key <signing-key-derived-from-devconfig> --output <output-image-name> --keyversion 1.5
  \endcode

- In the case of encryption, two extra options are also passed to the script like so:
  \code
  cd ${SDK_INSTALL_PATH}/tools/boot/signing
  $(PYTHON) appimage_x509_cert_gen.py --bin <path-to-the-binary> --authtype 0 --loadaddr 84000000 --key <signing-key-derived-from-devconfig> --enc y --enckey <encryption-key-derived-from-devconfig> --output <output-image-name> --keyversion 1.5
  \endcode

- These scripts are invoked in makefiles, and the image generation happens automatically along with the example build. So mostly these scripts need not be manually run.

 - Here,
  - `SBL_RUN_ADDRESS` is `0x43C00000`
  - In the case of GP device, `BOOTIMAGE_CERT_KEY` is `app_degenerateKey.pem`
  - In the case of HS device, `BOOTIMAGE_CERT_KEY` is custMpk_@VAR_SOC_NAME_LOWER .pem. 

\cond SOC_AM62X || SOC_AM62AX
For more details about this see \ref SECURE_BOOT
\endcond

These scripts are invoked in makefiles, and the image generation happens
automatically along with the example build. So mostly these scripts need
not be manually run.
If the user build-system is different from TI's makefile system, it needs to
be ensured that the same is followed as part of the post build steps.
The devconfig has ENC_SBL_ENABLED=yes and that is why for HS-SE devices, the SBL
image is encrypted by default.

\cond SOC_AM64X || SOC_AM243X
## XIP Image Generator Tool

- This tool, splits a input RPRC application file, into two RPRC files,
   - First RPRC file, containing non-XIP sections and the application entry point
   - Second RPRC file, containing XIP sections. Here, adjacent sections are merged into one section

- The final number of sections in both RPRC files taken together can be less
  than the sections in the input file due to section merging in the XIP RPRC file.

  \imageStyle{tools_xip_gen.png,width:60%}
  \image html tools_xip_gen.png "XIP Image Generation Tool flow"


- The non-XIP RPRC file should be flashed and booted via SBL as usual

- The XIP RPRC file should be flashed via the SDK flash writer, using the command `--flash-xip`,
  the flash writer in this case will flash sections at the flash address mentioned in the RPRC section header.

- To see the detailed options supported by the tool, run this tool with the option `--help`.
  Example, output in Windows is shown below,

  \code
  > cd {SDK_INSTALL_PATH}/tools/boot/xipGen
  > xipGen.exe

    XIP Image Creation Tool  - (c) Texas Instruments 2021, created on Apr 19 2021

    Usage: xipGen [options]

    Description,
    This tool, splits a input RPRC application file, into two RPRC files,
    - First RPRC file, containing non-XIP sections. The application entry point is assumed
        to be in non-XIP region.
    - Second RPRC file, containing XIP sections. The RPRC entry point is set to 0 and
        adjacent XIP sections are merged

    Note, the final number of sections in both RPRC files taken together can be less
    than the sections in the input file due to section merging in the XIP RPRC file.

    The non-XIP RPRC file should be flashed and booted via SBL as usual

    The XIP RPRC file should be flashed via the SDK flash writer, the flash writer will
    flash sections at the flash address mentioned in the RPRC sections

    Options,
    --input, -i : input RPRC file,
    --output, -o : output RPRC file of non-XIP sections,
    --output-xip, -x : output RPRC file of XIP sections,
    --flash-start-addr, -f : XIP flash address space start, specified in hex. If not specified 0x60000000 is used
    --flash-size, -s : XIP flash address space size in units of mega bytes, specified as integer. If not specified 256 MB is used
    --verbose, -v : Verbose prints are enabled during the tool execution
    --help, -h : Shows this help
  \endcode
\endcond


## UART Bootloader Python Script {#UART_BOOTLOADER_PYTHON_SCRIPT}

- This script is used in UART boot mode for sending the SBL and appimage binaries to the EVM via UART using XMODEM protocol
- Make sure that python3 and its dependent modules are installed in the host machine as mentioned in \ref INSTALL_PYTHON3
- Booting via UART is slow, but is useful if application loading via CCS or OSPI boot is not an option
- Make sure the UART port used for terminal is identified as mentioned in \ref CCS_UART_TERMINAL
- Make sure you have the EVM power cable and UART cable connected as shown in \ref EVM_CABLES
- To boot applications using this script, **POWER OFF the EVM**
- Switch to \ref BOOTMODE_UART.
- **POWER ON the EVM**
- To confirm that the board is in UART boot mode, open the UART terminal and confirm that you see the character 'C' getting printed on the console every 2-3 seconds.
- Now close the terminal. This is important as the script won't be able to function properly if the UART terminal is open.
- Update the appimage path on sbl_prebuilt/{board}/default_sbl_uart_hs_fs.cfg file
\cond !SOC_AM62PX
\note For HS-SE device, use default_sbl_uart_hs.cfg as the cfg file.
\endcond
- Open a command prompt and run the below command to send the SBL and application binary to the EVM
  - on Linux
  \code
  cd ${SDK_INSTALL_PATH}/tools/boot
  python uart_bootloader.py -p /dev/ttyUSB0 --cfg=sbl_prebuilt/{board}/default_sbl_uart_hs_fs.cfg
  \endcode

  - on Windows
  \code
  cd ${SDK_INSTALL_PATH}/tools/boot
  python uart_bootloader.py -p -p COM<x> --cfg=sbl_prebuilt/{board}/default_sbl_uart_hs_fs.cfg
  \endcode

- When you execute this, the script first sends the uart bootloader, and then the multicore appimage
- After the multicore appimage is successfully parsed, the uart bootloader sends an acknowledgment to the script
and waits for 5 seconds before running the application binary
- Upon receiving the ack, the script will exit successfully
- Connect to the UART terminal within 5 seconds to see logs from the application
- Below are the logs of the script after all the files have been sent
  \code
  Sending the UART bootloader sbl_prebuilt/{board}/sbl_uart.release.tiimage ...
  Sent bootloader sbl_prebuilt/{board}/sbl_uart.release.tiimage of size 243975 bytes in 23.94s.

  Sending the application ../../examples/drivers/udma/udma_memcpy_polling/{board}/r5fss0-0_nortos/ti-arm-clang/udma_memcpy_polling.release.appimage ...
  Sent application ../../examples/drivers/udma/udma_memcpy_polling/{board}/r5fss0-0_nortos/ti-arm-clang/udma_memcpy_polling.release.appimage of size 99580 bytes in 11.74s.
  [STATUS] Application load SUCCESS !!!
  Connect to UART in 5 seconds to see logs from UART !!!
  \endcode

\cond SOC_AM64X || SOC_AM62X || SOC_AM62AX || SOC_AM62PX
## Linux Appimage Generator Tool {#LINUX_APPIMAGE_GEN_TOOL}

\cond !SOC_AM62PX
\note Change DEVICE_TYPE to HS in ${SDK_INSTALL_PATH}/devconfig/devconfig.mak and then generate Linux Appimage for HS-SE device.
\endcond

- This tool generates a Linux Appimage by taking the Linux binaries (ATF, OPTEE, SPL) as input and generates a Linux appimage containing the input Linux binaries.
\cond SOC_AM62X || SOC_AM62AX || SOC_AM62PX
- In the falcon mode, A53 SPL is replaced by the Linux kernel so that Linux can boot up directly. For more information on obtaining Kernel and DTB, refer to Processor-SDK-Linux.
\endcond
- The input file location can be mentioned in the `config.mak` file located at {SDK_INSTALL_PATH}/tools/boot/linuxAppimageGen/board/@VAR_BOARD_NAME_LOWER
- **PSDK_LINUX_PATH** mentions the path of Processor-SDK-Linux installer.
\cond SOC_AM62X
- **PSDK_LINUX_PREBUILT_IMAGES** mentions the path of prebuilt images necessary for Linux appimage.
    - PSDK_LINUX_PREBUILT_IMAGES?=\$(PSDK_LINUX_PATH)/board-support/prebuilt-images/am62xx-evm
    - In case of AM62X-SK-LP board,
        - PSDK_LINUX_PREBUILT_IMAGES?=\$(PSDK_LINUX_PATH)/board-support/prebuilt-images/am62xx-lp-evm
    - In case of AM62X-SIP-SK board,
        - PSDK_LINUX_PREBUILT_IMAGES?=\$(PSDK_LINUX_PATH)/board-support/prebuilt-images/am62xxsip-evm
\endcond
\cond SOC_AM62AX
- **PSDK_LINUX_PREBUILT_IMAGES** mentions the path of prebuilt images necessary for Linux appimage.
    - PSDK_LINUX_PREBUILT_IMAGES?=\$(PSDK_LINUX_PATH)/board-support/prebuilt-images
\endcond
\cond SOC_AM62PX
- **PSDK_LINUX_PREBUILT_IMAGES** mentions the path of prebuilt images necessary for Linux appimage.
    - PSDK_LINUX_PREBUILT_IMAGES?=\$(PSDK_LINUX_PATH)/board-support/prebuilt-images/am62pxx-evm-display-cluster
    - By default for linux appimage, the config takes input from display cluster prebuilt images that implements changes required for early splash and display sharing with SBL boot flow.
    - In order to use default linux boot with SBL flow, use
        - PSDK_LINUX_PREBUILT_IMAGES?=\$(PSDK_LINUX_PATH)/board-support/prebuilt-images/am62pxx-evm.
\endcond

\cond SOC_AM64X
- The input file names for ATF, OPTEE and SPL can also be mentioned in the `config.mak` file.
    - `#Input linux binaries`\n
       `ATF_BIN_NAME=bl31.bin`\n
       `OPTEE_BIN_NAME=bl32.bin`\n
       `SPL_BIN_NAME=u-boot-spl.bin-am64xx-evm`\n
- The load address for ATF, OPTEE and SPL need to be mentioned in the `config.mak` file.
    - `#Linux image load address`\n
      `ATF_LOAD_ADDR=0x0701a0000`\n
      `OPTEE_LOAD_ADDR=0x9e800000`\n
      `SPL_LOAD_ADDR=0x80080000`\n
\endcond
\cond SOC_AM62X
- The input file names for ATF, OPTEE and SPL can also be mentioned in the `config.mak` file.
    - `#Input linux binaries`\n
       `ATF_BIN_NAME=bl31.bin`\n
       `OPTEE_BIN_NAME=bl32.bin`\n
       `SPL_BIN_NAME=u-boot-spl.bin-am62xx-evm`\n
    - In the falcon mode\n
       `ATF_BIN_NAME=bl31.bin`\n
       `OPTEE_BIN_NAME=bl32.bin`\n
       `KERN_BIN_NAME=Image`\n
       `FDT_BIN_NAME=k3-am625-sk.dtb`\n
\endcond
\cond SOC_AM62AX
- The input file names for ATF, OPTEE and SPL can also be mentioned in the `config.mak` file.
    - `#Input linux binaries`\n
       `ATF_BIN_NAME=bl31.bin`\n
       `OPTEE_BIN_NAME=bl32.bin`\n
       `SPL_BIN_NAME=u-boot-spl.bin-am62axx-evm`\n
    - In the falcon mode\n
       `ATF_BIN_NAME=bl31.bin`\n
       `OPTEE_BIN_NAME=bl32.bin`\n
       `KERN_BIN_NAME=Image`\n
       `FDT_BIN_NAME=k3-am62a7-sk.dtb`\n
\endcond
\cond SOC_AM62PX
- The input file names for ATF, OPTEE and SPL can also be mentioned in the `config.mak` file.
    - `#Input linux binaries`\n
       `ATF_BIN_NAME=bl31.bin`\n
       `OPTEE_BIN_NAME=bl32.bin`\n
       `SPL_BIN_NAME=u-boot-spl.bin-am62pxx-evm`\n
    - In the falcon mode\n
       `ATF_BIN_NAME=bl31.bin`\n
       `OPTEE_BIN_NAME=bl32.bin`\n
       `KERN_BIN_NAME=Image`\n
       `FDT_BIN_NAME=k3-am62p5-sk.dtb`\n
\endcond
\cond SOC_AM62X || SOC_AM62AX || SOC_AM62PX
- The load address for ATF, OPTEE and SPL need to be mentioned in the `config.mak` file.
    - `#Linux image load address`\n
      `ATF_LOAD_ADDR=0x9e780000`\n
      `OPTEE_LOAD_ADDR=0x9e800000`\n
      `SPL_LOAD_ADDR=0x80080000`\n
    - In the falcon mode\n
      `ATF_LOAD_ADDR=0x0701a0000`\n
      `OPTEE_LOAD_ADDR=0x9e800000`\n
      `KERN_LOAD_ADDR=0x80080000`\n
      `FDT_LOAD_ADDR=0x82000000`\n
\endcond
- The output appimage name can be mentioned in the `config.mak` file.
    - `#Output appimage name`\n
      `LINUX_BOOTIMAGE_NAME=linux.appimage`\n
- Run the makefile at {SDK_INSTALL_PATH}/tools/boot/linuxAppimageGen to generate the Linux appimage
    - For Windows

            cd ${SDK_INSTALL_PATH}/tools/boot/linuxAppimageGen
            gmake -s BOARD=@VAR_BOARD_NAME_LOWER all

    - For Linux

            cd ${SDK_INSTALL_PATH}/tools/boot/linuxAppimageGen
            make -s BOARD=@VAR_BOARD_NAME_LOWER all

\cond SOC_AM62X || SOC_AM62AX || SOC_AM62PX
    - For Falcon

            cd ${SDK_INSTALL_PATH}/tools/boot/linuxAppimageGen
            make -s BOARD=@VAR_BOARD_NAME_LOWER all FALCON_MODE=1
\endcond
\cond SOC_AM62X
    - In case of AM62X-SK-LP board, use `am62x-sk-lp` as the BOARD in make command
    - In case of AM62X-SIP-SK board, use `am62x-sip-sk` as the BOARD in make command
\endcond

- The Linux appimage wil be generated at {SDK_INSTALL_PATH}/tools/boot/linuxAppimageGen/board/@VAR_BOARD_NAME_LOWER after running the makefile
\endcond


\cond SOC_AM62X || SOC_AM62AX || SOC_AM62PX
## HSM Appimage Generator Tool {#HSM_APPIMAGE_GEN_TOOL}

\cond !SOC_AM62PX
\note Change DEVICE_TYPE to HS in ${SDK_INSTALL_PATH}/devconfig/devconfig.mak and then generate Linux Appimage for HS-SE device.
\endcond
\attention GCC AARCH64 compiler installation is required for HSM appimage generation. Refer \ref GCC_AARCH64_DOWNLOAD

- This tool generates a HSM Appimage by taking the HSM binaries (.bin file) as input and generates an appimage containing the input HSM binary.
- The input file location can be mentioned in the `config.mak` file located at {SDK_INSTALL_PATH}/tools/boot/HSMAppimageGen/board/@VAR_BOARD_NAME_LOWER
- The input file name for HSM bin file can be mentioned in the `config.mak` file.
    - `#Input binary name`\n
       `HSM_BINARY_NAME = HSM_min_sample.bin`\n
- The output appimage name can be mentioned in the `config.mak` file.
    - `#Output appimage name`\n
      `HSM_APPIMAGE_NAME=hsm.appimage`\n
- Run the makefile at {SDK_INSTALL_PATH}/tools/boot/HSMAppimageGen to generate the HSM appimage
    - For Windows

            cd ${SDK_INSTALL_PATH}/tools/boot/HSMAppimageGen
            gmake -s BOARD=@VAR_BOARD_NAME_LOWER all

    - For Linux

            cd ${SDK_INSTALL_PATH}/tools/boot/HSMAppimageGen
            make -s BOARD=@VAR_BOARD_NAME_LOWER all

\cond SOC_AM62X
    - Incase of AM62X-SK-LP baord use `am62x-sk-lp` as the BOARD in make command
    - Incase of AM62X-SIP-SK board, use `am62x-sip-sk` as the BOARD in make command
\endcond
- The HSM appimage wil be generated at {SDK_INSTALL_PATH}/tools/boot/HSMAppimageGen/board/@VAR_BOARD_NAME_LOWER after running the makefile
\endcond

\cond SOC_AM64X || SOC_AM243X

## SoC ID parser Python Script

- Boot ROM reports SoC ID on UART console of the device when UART boot mode is selected. It reports on both GP and HS devices and it provides insights into device configuration which would be helpful for debugs.

- uart_boot_socid.py is a python based parser to convert the hexadecimal numbers reported by ROM to human readable text, below are the steps involved to use this parser. This will be helpful in debugging the device boot issue. This will also help to see important information about device like device type, prime/non-prime, key count, key revision, MPK hash etc.

- Steps to use the parser:

1) Copy the soc id reported in UART console to socid.txt (socid.txt)

2) execute

\code
$python uart_boot_socid.py socid.txt
\endcode

- Example Output:

SoC ID Header Info: \n  \n

NumBlocks : [2] \n   \n

SoC ID Public ROM Info: \n \n

SubBlockId : 1 \n
SubBlockSize : 26 \n
DeviceName : am64x  \n
DeviceType : HSSE \n
DMSC ROM Version : [0, 1, 1, 1] \n
R5 ROM Version : [0, 1, 1, 1] \n \n

SoC ID Secure ROM Info: \n \n

Sec SubBlockId : 2 \n
Sec SubBlockSize : 166 \n
Sec Prime : 0 \n
Sec Key Revision : 1 \n
Sec Key Count : 1 \n
Sec TI MPK Hash : aa1f8e3095042e5c71ac40ede5b4e8c85fa87e03305ae0ea4f47933e89f4164aeb5a12ae13778f49de0622c1a578e6e747981d8c44a130f89a336a887a7955ee \n
Sec Cust MPK Hash : 1f6002b07cd9b0b7c47d9ca8d1aae57b8e8784a12f636b2b760d7d98a18f189760dfd0f23e2b0cb10ec7edc7c6edac3d9bdfefe0eddc3fff7fe9ad875195527d \n
Sec Unique ID : fd6e232b89dfc6ea125c2fa09f25f95034e08a54797490c32bf47c64bf4c8f21 \n

\endcond
