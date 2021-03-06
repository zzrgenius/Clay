/* ###################################################################
 **     Filename    : main.c
 **     Project     : clay_g5_bootloader
 **     Processor   : MK20DX256VLL7
 **     Version     : Driver 01.01
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2015-12-19, 11:12, # CodeGen: 0
 **     Abstract    :
 **         Main module.
 **         This module contains user's application code.
 **     Settings    :
 **     Contents    :
 **         No public methods
 **
 ** ###################################################################*/
/*!
 ** @file main.c
 ** @version 01.01
 ** @brief
 **         Main module.
 **         This module contains user's application code.
 */
/*!
 **  @addtogroup main_module main module documentation
 **  @{
 */
/* MODULE main */

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "FLASH1.h"
#include "tick_1ms_timer.h"
#include "TU1.h"
#include "LED1.h"
#include "LED2.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "system_tick.h"
#include "program_flash.h"

#define DERP_LENGTH 1000

static uint8_t derp[DERP_LENGTH] = { 0 };

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
    /* Write your local variable definition here */

    /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
    PE_low_level_init();
    /*** End of Processor Expert internal initialization.                    ***/

    /* Write your code here */
    /* For example: for(;;) { } */

    bool erase = 0;
    bool write = 0;
    bool get_crc = 0;
    uint8_t last_rval = 0;
    uint16_t last_checksum;
    bool led_state = FALSE;

    for (;;)
    {
        if (erase)
        {
            erase = 0;
//            last_rval = erase_program_flash_page(APP_START_ADDR);
            last_rval = erase_program_flash();
        }

        if (write)
        {
            write = 0;
            last_rval = write_program_block(APP_START_ADDR, derp, DERP_LENGTH);
        }
        
        if(get_crc)
        {
            get_crc = 0;
            last_checksum = compute_checksum();
        }
        
        if(tick_250msec)
        {
            tick_250msec = FALSE;
            LED1_PutVal(LED1_DeviceData, led_state);
            LED2_PutVal(LED2_DeviceData, !led_state);
            led_state = !led_state;
        }
    }

    /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
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
