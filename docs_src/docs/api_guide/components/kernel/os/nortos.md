# NO RTOS {#KERNEL_NORTOS_PAGE}

[TOC]

\cond !SOC_AM62X && !SOC_AM62AX
\note A53 will not be available on all SOCs. All references to A53 should be ignored on such SOCs.
\endcond

## Introduction

NORTOS is a SW module that implements APIs which allow the upper layers of SW to run in no-RTOS mode.
i.e without any RTOS. The NO RTOS APIs are implemented underneath the \ref KERNEL_DPL_PAGE APIs. This allows
the device drivers to run either in no-RTOS mode or with a RTOS.

## Features Supported

Common across all CPUs,
- Clock APIs to initialize a system tick ISR and allow SW to create multiple SW timers using a single underlying HW timer.
\if SOC_AM62AX
- Address translate APIs to translate system address to local address, needed for MCU_R5F
\else
- Address translate APIs to translate system address to local address, needed for M4F
\endif

- Heap APIs to create arbitrary heaps at user defined memory locations
- Semaphore APIs to model a semaphore in no-RTOS environment
- HW Timer APIs to setup user defined HW timers beyond the system tick timer.
- Logging APIs to log to different consoles like UART, CCS, shared memory, CPU local memory, including logging zones to enable/disable logging.

\if SOC_AM62AX
DM_R5F features,
- CPU start up code
- Cache APIs to enable, disable, invalidate, write back caches
- Memory protection unit (MPU) APIs to enable, disable multiple regions in the MPU
- Interrupt controller APIs to register ISRs, enable/disable interrupts
- ISR handlers and exception handlers
- Performance counter APIs
- DM_R5F ISRs,
  - IRQ mode,
    - nested interrupts supported
    - FPU save/restore supported
\else
R5F features,
- CPU start up code
- Cache APIs to enable, disable, invalidate, write back caches
- Memory protection unit (MPU) APIs to enable, disable multiple regions in the MPU
- Interrupt controller APIs to register ISRs, enable/disable interrupts
- ISR handlers and exception handlers
- Performance counter APIs
- R5F ISRs,
  - IRQ mode,
    - nested interrupts supported
    - FPU save/restore supported
\endif

\if SOC_AM62AX
MCU_R5F features,
- CPU start up code
- Memory protection unit (MPU) APIs to enable, disable multiple regions in the MPU
- Interrupt controller APIs to register ISRs, enable/disable interrupts
- ISR handlers and exception handlers
- Performance counter APIs
- SysTick timer APIs
- MCU_R5F ISRs
  - Nested interrupts
\else
M4F features,
- CPU start up code
- Memory protection unit (MPU) APIs to enable, disable multiple regions in the MPU
- Interrupt controller APIs to register ISRs, enable/disable interrupts
- ISR handlers and exception handlers
- Performance counter APIs
- SysTick timer APIs
- M4F ISRs
  - Nested interrupts
\endif

\cond SOC_AM62X || SOC_AM62AX
A53 features,
- Single Core A53
- CPU start up code
- Memory management unit (MMU) APIs to enable, disable multiple regions in the MMU
- Interrupt controller APIs to register ISRs, enable/disable interrupts
- ISR handlers and exception handlers
- Performance counter APIs
- A53 ISRs
  - IRQ mode
    - nested interrupt supported
    - FPU save/restore supported
\endcond

## SysConfig Features

@VAR_SYSCFG_USAGE_NOTE

SysConfig can be used to configure below modules with NORTOS
- Clock module to setup system tick timer including the tick duration
- Debug Log module to select the console to use for logging as well as enable/disable logging zones
\if SOC_AM62AX
- RAT to setup  address translation regions, needed for MCU_R5F and DM_R5F
- MPU ARMv7 to setup different MPU regions for DM_R5F and MCU_R5F CPUs
\else
- RAT to setup  address translation regions, needed for M4F
- MPU ARMv7 to setup different MPU regions for R5F and M4F CPUs
\endif
\cond SOC_AM62X || SOC_AM62AX
- MMU ARMV8 to setup different MMU regions for A53 CPU
\endcond
- Timer to setup HW timer available on the SOC, including enabling timer interrupt and ISR registration


## Features Not Supported

- Task APIs are not supported in NORTOS mode. Task APIs necessarily need a RTOS and cannot be used in no-RTOS mode
\if SOC_AM62AX
- DM_R5F ISRs,
  - FIQ mode,
    - nested interrupts not supported.
    - FPU save/restore not supported.
- MCU_R5F ISRs,
  - FPU save/restore not supported.
\else
- R5F ISRs,
  - FIQ mode,
    - nested interrupts not supported
    - FPU save/restore not supported.
- M4F ISRs,
  - FPU save/restore not supported.
\endif

- A53 ISRs,
  - FIQ mode ISRs not supported.
- A53 multi-core SMP mode is not supported.


## Important files and directory structure

<table>
<tr>
    <th>Folder/Files
    <th>Description
</tr>
<tr><td colspan="2" bgcolor=#F0F0F0> ${SDK_INSTALL_PATH}/source/kernel/</td></tr>
<tr>
    <td>dpl/
    <td>APIs to access NORTOS features</td>
</tr>
<tr><td colspan="2" bgcolor=#F0F0F0> ${SDK_INSTALL_PATH}/source/kernel/nortos</td></tr>
<tr>
    <td>lib/
    <td>NORTOS library to link against. Linking to the library in this path enables the application to operate in no-RTOS mode</td>
</tr>
<tr>
    <td>dpl/common/
    <td>NORTOS APIs that are common across all CPUs
</tr>
\cond !SOC_AM62AX
<tr>
    <td>dpl/m4/
    <td>NORTOS APIs that are specific to M4F CPUs
</tr>
\endcond
\cond SOC_AM62X
<tr>
    <td>dpl/r5/
    <td>NORTOS APIs that are specific to R5F CPUs
</tr>
\endcond
\cond SOC_AM62X || SOC_AM62AX
<tr>
    <td>dpl/a53/
    <td>NORTOS APIs that are specific to A53 CPUs
</tr>
\endcond
</table>

## Additional References {#NORTOS_ADDITIONAL_REFERENCES}

\note Precise web links are not provided since these can change, search for the document title in google to download the documents

Please also refer to below documents from ARM Ltd. to understand more about R5F, M4F, A53 CPUs including
features like MPU, MMU, cache and interrupts.

<table>
<tr>
    <th>Document Title
    <th>Description
</tr>
\cond !SOC_AM62X
<tr>
    <td>ARM v7R Architecture Reference Manual
    <td>Information about ARM architecture that is implemented by R5F. Should be used in conjunction with R5F TRM to understand R5F architecture details.
</tr>
\endcond
\cond !SOC_AM62AX
<tr>
    <td>ARM v7M Architecture Reference Manual
    <td>Information about ARM architecture that is implemented by M4F. Should be used in conjunction with M4F TRM to understand M4F architecture details.
</tr>
\endcond
\cond !SOC_AM62X && !SOC_AM62AX
<tr>
    <td>ARM v8A Architecture Reference Manual
    <td>Information about ARM architecture that is implemented by A53. Should be used in conjunction with A53 TRM to understand A53 architecture details.
</tr>
\endcond
\cond !SOC_AM62X
<tr>
    <td>ARM Cortex R5F Technical Reference Manual
    <td>Information about R5F CPU architecture.
</tr>
\endcond
\cond !SOC_AM62AX
<tr>
    <td>ARM Cortex M4F Technical Reference Manual
    <td>Information about M4F CPU architecture.
</tr>
\endcond
\cond !SOC_AM62X && !SOC_AM62AX
<tr>
    <td>ARM Cortex A53 Technical Reference Manual
    <td>Information about A53 CPU architecture.
</tr>
\endcond
</table>

## See also

\ref KERNEL_DPL_PAGE