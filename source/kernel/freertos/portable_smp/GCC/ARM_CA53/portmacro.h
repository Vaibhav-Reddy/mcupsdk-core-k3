/*
 * FreeRTOS Kernel V10.4.3
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 * 1 tab == 4 spaces!
 */

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
	extern "C" {
#endif

#include <kernel/dpl/HwiP.h>
#include <kernel/nortos/dpl/a53/HwiP_armv8_gic.h>
#include <kernel/nortos/dpl/a53/common_armv8.h>

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the given hardware
 * and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	size_t
#define portBASE_TYPE	long

typedef portSTACK_TYPE StackType_t;
typedef portBASE_TYPE BaseType_t;
typedef uint64_t UBaseType_t;

typedef uint32_t TickType_t;
#define portMAX_DELAY ( ( TickType_t ) 0xffffffff )

/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
not need to be guarded with a critical section. */
#define portTICK_TYPE_IS_ATOMIC 1

/* Hardware specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT			16
#define portPOINTER_SIZE_TYPE 		uint64_t
#define portCRITICAL_NESTING_IN_TCB 1

/* Task utilities. */

extern uint64_t ullPortYieldRequired[];
/* Called at the end of an ISR that can cause a context switch. */
#define portEND_SWITCHING_ISR( xSwitchRequired )            \
{												            \
												            \
	if( xSwitchRequired != pdFALSE )			            \
	{											            \
		ullPortYieldRequired[portGET_CORE_ID()] = pdTRUE;   \
	}											            \
}

#define portYIELD_FROM_ISR( x ) portEND_SWITCHING_ISR( x )
#define portYIELD() __asm volatile ( "SVC 0" ::: "memory" )

/*-----------------------------------------------------------
 * Critical section control
 *----------------------------------------------------------*/

extern void vTaskEnterCritical( void );
extern void vTaskExitCritical( void );

#define portDISABLE_INTERRUPTS()                HwiP_disable()
#define portENABLE_INTERRUPTS()		            HwiP_enable()
#define portENTER_CRITICAL()		            vTaskEnterCritical();
#define portEXIT_CRITICAL()			            vTaskExitCritical();
extern uint32_t ulSetInterruptMaskFromISR( void ) ;
extern void vClearInterruptMaskFromISR( uint32_t ulMask ) ;
#define portSET_INTERRUPT_MASK_FROM_ISR()         ulSetInterruptMaskFromISR()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR( x )    vClearInterruptMaskFromISR(x);
/* Task function macros as described on the FreeRTOS.org WEB site.  These are
not required for this port but included in case common demo code that uses these
macros is used. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters )	void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters )	void vFunction( void *pvParameters )

/* Any task that uses the floating point unit MUST call vPortTaskUsesFPU()
before any floating point instructions are executed. */
void vPortTaskUsesFPU( void );
#define portTASK_USES_FLOATING_POINT() vPortTaskUsesFPU()

/* Architecture specific optimisations. */
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
	#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#endif

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1

	/* Store/clear the ready priorities in a bit map. */
	#define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )
	#define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )

	#define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) uxTopPriority = ( 31 - __builtin_clz( uxReadyPriorities ) )

#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

#define portNOP() __asm volatile( "NOP" )
#define portINLINE __inline
#define portMEMORY_BARRIER() __asm volatile( "" ::: "memory" )

/* port for SMP */
#define portGET_CORE_ID()                   Armv8_getCoreId()
#define portRESTORE_INTERRUPTS( ulState )   HwiP_restore ( ulState )
#define portCHECK_IF_IN_ISR()               HwiP_inISR()
#define portASSERT_IF_IN_ISR()              configASSERT( portCHECK_IF_IN_ISR() == 0 )
#define portSET_INTERRUPT_MASK()            HwiP_disable()
#define portCLEAR_INTERRUPT_MASK( ulState)  HwiP_restore ( ulState )
extern UBaseType_t vTaskEnterCriticalFromISR( void );
extern void vTaskExitCriticalFromISR( UBaseType_t uxSavedInterruptStatus );
#define portENTER_CRITICAL_FROM_ISR()       vTaskEnterCriticalFromISR()
#define portEXIT_CRITICAL_FROM_ISR( x )     vTaskExitCriticalFromISR( x )

/*-----------------------------------------------------------
 * Critical section locks
 *----------------------------------------------------------*/

#define ISR_LOCK   (0u)
#define TASK_LOCK  (1u)

#define portRTOS_LOCK_COUNT 2
#if defined(SMP_QUADCORE_FREERTOS)
#define portMAX_CORE_COUNT 4
#else
#define portMAX_CORE_COUNT 2
#endif


uint64_t Get_64(volatile uint64_t* x);
void Set_64(volatile uint64_t* x, uint64_t value);

/* Which core owns the lock */
volatile uint64_t ucOwnedByCore[ portMAX_CORE_COUNT ];
/* Lock count a core owns */
volatile uint64_t ucRecursionCountByLock[ portRTOS_LOCK_COUNT ];

/* Index 0 is used for ISR lock and Index 1 is used for task lock */
uint32_t GateWord[ portRTOS_LOCK_COUNT ];

int32_t GateSmp_tryLock(uint32_t* gateWord);
void GateSmp_unlock(uint32_t* gateWord);

static inline void vPortRecursiveLock(uint32_t ulLockNum, BaseType_t uxAcquire)
{
    uint32_t ulCoreNum = portGET_CORE_ID();
    uint32_t ulLockBit = 1u << ulLockNum;

    /* Lock acquire */
    if (uxAcquire)
    {

        /* Check if spinlock is available */
        /* If spinlock is not available check if the core owns the lock */
        /* If the core owns the lock wait increment the lock count by the core */
        /* If core does not own the lock wait for the spinlock */
        if( GateSmp_tryLock( &GateWord[ulLockNum] ) != 0)
        {
            /* Check if the core owns the spinlock */
            if( Get_64(&ucOwnedByCore[ulCoreNum]) & ulLockBit )
            {
                configASSERT( Get_64(&ucRecursionCountByLock[ulLockNum]) != 255u);
                Set_64(&ucRecursionCountByLock[ulLockNum], (Get_64(&ucRecursionCountByLock[ulLockNum])+1));
                return;
            }

            /* Preload the gate word into the cache */
            uint32_t dummy = GateWord[ulLockNum];
            dummy++;

            /* Wait for spinlock */
            while( GateSmp_tryLock(&GateWord[ulLockNum]) != 0);
        }

         /* Add barrier to ensure lock is taken before we proceed */
        __asm__ __volatile__ (
            "dmb sy"
            ::: "memory"
        );

        /* Assert the lock count is 0 when the spinlock is free and is acquired */
        configASSERT(Get_64(&ucRecursionCountByLock[ulLockNum]) == 0);

        /* Set lock count as 1 */
        Set_64(&ucRecursionCountByLock[ulLockNum], 1);
        /* Set ucOwnedByCore */
        Set_64(&ucOwnedByCore[ulCoreNum], (Get_64(&ucOwnedByCore[ulCoreNum]) | ulLockBit));
    }
    /* Lock release */
    else
    {
        /* Assert the lock is not free already */
        configASSERT( (Get_64(&ucOwnedByCore[ulCoreNum]) & ulLockBit) != 0 );
        configASSERT( Get_64(&ucRecursionCountByLock[ulLockNum]) != 0 );

        /* Reduce ucRecursionCountByLock by 1 */
        Set_64(&ucRecursionCountByLock[ulLockNum], (Get_64(&ucRecursionCountByLock[ulLockNum]) - 1) );

        if( !Get_64(&ucRecursionCountByLock[ulLockNum]) )
        {
            Set_64(&ucOwnedByCore[ulCoreNum], (Get_64(&ucOwnedByCore[ulCoreNum]) & ~ulLockBit));
            GateSmp_unlock(&GateWord[ulLockNum]);
            /* Add barrier to ensure lock is taken before we proceed */
            __asm__ __volatile__ (
                "dmb sy"
                ::: "memory"
            );
        }
    }
}

#define portRELEASE_ISR_LOCK()  vPortRecursiveLock(ISR_LOCK, pdFALSE)
#define portGET_ISR_LOCK()  vPortRecursiveLock(ISR_LOCK, pdTRUE)

#define portRELEASE_TASK_LOCK()  vPortRecursiveLock(TASK_LOCK, pdFALSE)
#define portGET_TASK_LOCK()  vPortRecursiveLock(TASK_LOCK, pdTRUE)


extern int32_t Signal_coreIntr( CSL_gic500_gicrRegs *pGic500GicrRegs, uint32_t coreId, uint32_t intrNum );

/* Interrupt number to interrupt a core for task yield */
#define YIELD_CORE_INTERRUPT_NO     (0U)

#define portYIELD_CORE( xCoreID )   do {                        \
                                        Signal_coreIntr((CSL_gic500_gicrRegs *) ( HWIP_GIC_BASE_ADDR + CSL_GIC500_GICR_CORE_CONTROL_CTLR(0U)), xCoreID, YIELD_CORE_INTERRUPT_NO); \
                                    }while(0);

#ifdef __cplusplus
	} /* extern C */
#endif

#endif /* PORTMACRO_H */

