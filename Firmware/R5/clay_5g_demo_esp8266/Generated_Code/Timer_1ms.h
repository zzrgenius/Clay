/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : Timer_1ms.h
**     Project     : clay_5g_demo_esp8266
**     Processor   : MK20DX256VLH7
**     Component   : TimerInt_LDD
**     Version     : Component 01.018, Driver 01.02, CPU db: 3.00.000
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-09-15, 15:16, # CodeGen: 9
**     Abstract    :
**          This TimerInt component implements a periodic interrupt.
**          When the component and its events are enabled, the "OnInterrupt"
**          event is called periodically with the period that you specify.
**          TimerInt supports also changing the period in runtime.
**          This TimerInt component provides a high level API for unified
**          hardware access to various timer devices using the TimerUnit
**          component.
**     Settings    :
**          Component name                                 : Timer_1ms
**          Periodic interrupt source                      : FTM0_MOD
**          Counter                                        : FTM0_CNT
**          Interrupt service/event                        : Enabled
**            Interrupt                                    : INT_FTM0
**            Interrupt priority                           : medium priority
**          Interrupt period                               : 1 ms
**          Initialization                                 : 
**            Enabled in init. code                        : yes
**            Auto initialization                          : no
**            Event mask                                   : 
**              OnInterrupt                                : Enabled
**          CPU clock/configuration selection              : 
**            Clock configuration 0                        : This component enabled
**            Clock configuration 1                        : This component disabled
**            Clock configuration 2                        : This component disabled
**            Clock configuration 3                        : This component disabled
**            Clock configuration 4                        : This component disabled
**            Clock configuration 5                        : This component disabled
**            Clock configuration 6                        : This component disabled
**            Clock configuration 7                        : This component disabled
**          Referenced components                          : 
**            Linked TimerUnit                             : TU1
**     Contents    :
**         Init - LDD_TDeviceData* Timer_1ms_Init(LDD_TUserData *UserDataPtr);
**
**     Copyright : 1997 - 2014 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file Timer_1ms.h
** @version 01.02
** @brief
**          This TimerInt component implements a periodic interrupt.
**          When the component and its events are enabled, the "OnInterrupt"
**          event is called periodically with the period that you specify.
**          TimerInt supports also changing the period in runtime.
**          This TimerInt component provides a high level API for unified
**          hardware access to various timer devices using the TimerUnit
**          component.
*/         
/*!
**  @addtogroup Timer_1ms_module Timer_1ms module documentation
**  @{
*/         

#ifndef __Timer_1ms_H
#define __Timer_1ms_H

/* MODULE Timer_1ms. */

/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* Include inherited beans */
#include "TU1.h"

#include "Cpu.h"

#ifdef __cplusplus
extern "C" {
#endif 


/*! Peripheral base address of a device allocated by the component. This constant can be used directly in PDD macros. */
#define Timer_1ms_PRPH_BASE_ADDRESS  0x40038000U
  
/* Methods configuration constants - generated for all enabled component's methods */
#define Timer_1ms_Init_METHOD_ENABLED  /*!< Init method of the component Timer_1ms is enabled (generated) */

/* Events configuration constants - generated for all enabled component's events */
#define Timer_1ms_OnInterrupt_EVENT_ENABLED /*!< OnInterrupt event of the component Timer_1ms is enabled (generated) */



/*
** ===================================================================
**     Method      :  Timer_1ms_Init (component TimerInt_LDD)
*/
/*!
**     @brief
**         Initializes the device. Allocates memory for the device data
**         structure, allocates interrupt vectors and sets interrupt
**         priority, sets pin routing, sets timing, etc. If the
**         property ["Enable in init. code"] is set to "yes" value then
**         the device is also enabled (see the description of the
**         [Enable] method). In this case the [Enable] method is not
**         necessary and needn't to be generated. This method can be
**         called only once. Before the second call of Init the [Deinit]
**         must be called first.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. This pointer will be
**                           passed as an event or callback parameter.
**     @return
**                         - Pointer to the dynamically allocated private
**                           structure or NULL if there was an error.
*/
/* ===================================================================*/
LDD_TDeviceData* Timer_1ms_Init(LDD_TUserData *UserDataPtr);

/*
** ===================================================================
**     Method      :  TU1_OnCounterRestart (component TimerInt_LDD)
**
**     Description :
**         The method services the event of the linked component TU1 and 
**         eventually invokes event Timer_1ms_OnInterrupt.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void TU1_OnCounterRestart(LDD_TUserData *UserDataPtr);

/* END Timer_1ms. */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif
/* ifndef __Timer_1ms_H */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
