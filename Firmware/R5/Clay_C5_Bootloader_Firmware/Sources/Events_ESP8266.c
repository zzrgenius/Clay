/* ###################################################################
**     Filename    : Events_ESP8266.c
**     Project     : Clay_5G_Firmware
**     Processor   : MK20DX256VLH7
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-10-02, 10:29, # CodeGen: 34
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         ESP8266_Serial_OnBlockReceived - void ESP8266_Serial_OnBlockReceived(LDD_TUserData *UserDataPtr);
**         ESP8266_Serial_OnBlockSent     - void ESP8266_Serial_OnBlockSent(LDD_TUserData *UserDataPtr);
**
** ###################################################################*/
/*!
** @file Events_ESP8266.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_ESP8266_module Events_ESP8266 module documentation
**  @{
*/         
/* MODULE Events_ESP8266 */

#include "Cpu.h"
#include "Events.h"
#include "Events_ESP8266.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */
#include "Application.h"
#include "Drivers/ESP8266.h"
#include "Clock.h"

/*
** ===================================================================
**     Event       :  ESP8266_Serial_OnBlockReceived (module Events_ESP8266)
**
**     Component   :  ESP8266_Serial [Serial_LDD]
*/
/*!
**     @brief
**         This event is called when the requested number of data is
**         moved to the input buffer.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. This pointer is passed
**                           as the parameter of Init method.
*/
/* ===================================================================*/
void ESP8266_Serial_OnBlockReceived(LDD_TUserData *UserDataPtr)
{
	ESP8266_UART_Device *ptr = (ESP8266_UART_Device*) UserDataPtr;

	(void) ptr->rxPutFct (ptr->rxChar);
	(void) ESP8266_Serial_ReceiveBlock (ptr->handle, (LDD_TData *) &ptr->rxChar, sizeof (ptr->rxChar));
	
//	if (Ring_Buffer_NofElements () > 500) {
//		printf ("Ring_Buffer_NofElements: %d\r\n", Ring_Buffer_NofElements ());
//	}
}

/*
** ===================================================================
**     Event       :  ESP8266_Serial_OnBlockSent (module Events_ESP8266)
**
**     Component   :  ESP8266_Serial [Serial_LDD]
*/
/*!
**     @brief
**         This event is called after the last character from the
**         output buffer is moved to the transmitter. 
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. This pointer is passed
**                           as the parameter of Init method.
*/
/* ===================================================================*/
void ESP8266_Serial_OnBlockSent(LDD_TUserData *UserDataPtr)
{
	ESP8266_UART_Device *ptr = (ESP8266_UART_Device*) UserDataPtr;
	ptr->isSent = TRUE;
}

/* END Events_ESP8266 */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

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
