# MCASP Loopback {#EXAMPLES_DRIVERS_MCASP_LOOPBACK}

[TOC]

# Introduction

This example demonstrates the McASP in loopback mode operation.

- Mcasp is configured in TDM mode
- Transfers data on single channel

# Supported Combinations {#EXAMPLES_DRIVERS_MCASP_LOOPBACK_COMBOS}

\cond SOC_AM273X

 Parameter      | Value
 ---------------|-----------
 CPU + OS       | r5fss0-0 freertos
 ^              | r5fss0-0 nortos
 ^              | c66ss0 nortos
 Toolchain      | ti-arm-clang, ti-c6000
 Board          | @VAR_BOARD_NAME_LOWER
 Example folder | examples/drivers/mcasp/mcasp_loopback

\endcond

\cond SOC_AM62AX

 Parameter      | Value
 ---------------|-----------
 CPU + OS       | c75ss0-0 freertos
 ^              | r5fss0-0 nortos
 ^              | a53ss0-0 freertos
 Toolchain      | ti-c7000
 ^              | ti-arm-clang
 Board          | @VAR_BOARD_NAME_LOWER
 Example folder | examples/drivers/mcasp/mcasp_loopback

\endcond

\cond SOC_AM62X

 Parameter      | Value
 ---------------|-----------
 CPU + OS       | a53ss0-0 freertos             |
 Toolchain      | arm.gnu.aarch64-none
 Board          | @VAR_BOARD_NAME_LOWER, @VAR_SK_LP_BOARD_NAME_LOWER, @VAR_SIP_SK_BOARD_NAME_LOWER
 Example folder | examples/drivers/mcasp/mcasp_loopback

\endcond

# Steps to Run the Example

- **When using CCS projects to build**, import the CCS project for the required combination
  and build it using the CCS project menu (see \ref CCS_PROJECTS_PAGE).
- **When using makefiles to build**, note the required combination and build using
  make command (see \ref MAKEFILE_BUILD_PAGE)
- Launch a CCS debug session and run the executable, see \ref CCS_LAUNCH_PAGE

# See Also

\ref DRIVERS_MCASP_PAGE

# Sample Output

Shown below is a sample output when the application is run,
\if SOC_AM62AX
\code
[MCASP] Loopback example started. Testing 2560 bytes ...
All bytes match!!
All tests have passed!!
\endcode
\elseif SOC_AM62X
\code
[MCASP] Loopback example started. Testing 2560 bytes ...
All bytes match!!
All tests have passed!!
\endcode
\else
\code
[MCASP] Loopback example started. Testing 100 bytes ...
All 100 bytes match!!
\endcode
\endif

