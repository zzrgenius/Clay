/* ###################################################################
 **     Filename    : Events.c
 **     Project     : Clay_C6_Bootloader
 **     Processor   : MK64FN1M0VLL12
 **     Component   : Events
 **     Version     : Driver 01.00
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2016-04-24, 22:28, # CodeGen: 0
 **     Abstract    :
 **         This is user's event module.
 **         Put your event handler code here.
 **     Contents    :
 **         Cpu_OnNMI - void Cpu_OnNMI(void);
 **
 ** ###################################################################*/
/*!
 ** @file Events.c
 ** @version 01.00
 ** @brief
 **         This is user's event module.
 **         Put your event handler code here.
 */
/*!
 **  @addtogroup Events_module Events module documentation
 **  @{
 */
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"
#include "Init_Config.h"
#include "PDD_Includes.h"
#include "WiFi.h"
#include "Clock.h"
#include "Program_Flash.h"
#include "Button.h"

#include "Multibyte_Ring_Buffer.h"

#ifdef __cplusplus
extern "C" {
#endif 

#define WIFI_INTERRUPT_RX_BUF_SIZE              5

uint8_t wifi_serial_interrupt_rx_buf[WIFI_INTERRUPT_RX_BUF_SIZE];
uint32_t wifi_rx_interrupt_count = WIFI_INTERRUPT_RX_BUF_SIZE;

/* User includes (#include below this line is not maintained by Processor Expert) */

/*
 ** ===================================================================
 **     Event       :  Cpu_OnNMI (module Events)
 **
 **     Component   :  Cpu [MK64FN1M0LQ12]
 */
/*!
 **     @brief
 **         This event is called when the Non maskable interrupt had
 **         occurred. This event is automatically enabled when the [NMI
 **         interrupt] property is set to 'Enabled'.
 */
/* ===================================================================*/
void Cpu_OnNMI(void) {
   /* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  Timer_1ms_OnInterrupt (module Events)
 **
 **     Component   :  Timer_1ms [TimerInt_LDD]
 */
/*!
 **     @brief
 **         Called if periodic event occur. Component and OnInterrupt
 **         event must be enabled. See [SetEventMask] and [GetEventMask]
 **         methods. This event is available only if a [Interrupt
 **         service/event] is enabled.
 **     @param
 **         UserDataPtr     - Pointer to the user or
 **                           RTOS specific data. The pointer passed as
 **                           the parameter of Init method.
 */
/* ===================================================================*/
void Timer_1ms_OnInterrupt(LDD_TUserData *UserDataPtr) {
   /* Write your code here ... */
   Tick();
}

/*
 ** ===================================================================
 **     Event       :  WIFI_XPD_DCDC_INTERRUPT_OnPortEvent (module Events)
 **
 **     Component   :  WIFI_XPD_DCDC_INTERRUPT [GPIO_LDD]
 */
/*!
 **     @brief
 **         Called if defined event on any pin of the port occured.
 **         OnPortEvent event and GPIO interrupt must be enabled. See
 **         SetEventMask() and GetEventMask() methods. This event is
 **         enabled if [Interrupt service/event] is Enabled and disabled
 **         if [Interrupt service/event] is Disabled.
 **     @param
 **         UserDataPtr     - Pointer to RTOS device
 **                           data structure pointer.
 */
/* ===================================================================*/
void WIFI_XPD_DCDC_INTERRUPT_OnPortEvent(LDD_TUserData *UserDataPtr) {
   /* Write your code here ... */
   WifiInterruptReceived = TRUE;
}

/*
 ** ===================================================================
 **     Event       :  ESP8266_Serial_OnBlockReceived (module Events)
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
void ESP8266_Serial_OnBlockReceived(LDD_TUserData *UserDataPtr) {
   /* Write your code here ... */

   //receive block sets up the count and the location to store the data
   //   this interrupt is fired when the count has been met.
   ESP8266_UART_Device *ptr = (ESP8266_UART_Device*) UserDataPtr;

   Multibyte_Ring_Buffer_Enqueue(&wifi_multibyte_ring, wifi_serial_interrupt_rx_buf, wifi_rx_interrupt_count);
   (void) ESP8266_Serial_ReceiveBlock(ptr->handle, wifi_serial_interrupt_rx_buf, wifi_rx_interrupt_count);
}

/*
 ** ===================================================================
 **     Event       :  ESP8266_Serial_OnBlockSent (module Events)
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
void ESP8266_Serial_OnBlockSent(LDD_TUserData *UserDataPtr) {
   /* Write your code here ... */
   ESP8266_UART_Device *ptr = (ESP8266_UART_Device*) UserDataPtr;
   ptr->isSent = TRUE;
}

/*
 ** ===================================================================
 **     Event       :  BUTTON_IN_OnPortEvent (module Events)
 **
 **     Component   :  BUTTON_IN [GPIO_LDD]
 */
/*!
 **     @brief
 **         Called if defined event on any pin of the port occured.
 **         OnPortEvent event and GPIO interrupt must be enabled. See
 **         SetEventMask() and GetEventMask() methods. This event is
 **         enabled if [Interrupt service/event] is Enabled and disabled
 **         if [Interrupt service/event] is Disabled.
 **     @param
 **         UserDataPtr     - Pointer to RTOS device
 **                           data structure pointer.
 */
/* ===================================================================*/
void BUTTON_IN_OnPortEvent(LDD_TUserData *UserDataPtr) {
   /* Write your code here ... */
   Button_Event_Handler();
}

/*
 ** ===================================================================
 **     Event       :  FLASH1_OnOperationComplete (module Events)
 **
 **     Component   :  FLASH1 [FLASH_LDD]
 */
/*!
 **     @brief
 **         Called at the end of the whole write / erase operation. if
 **         the event is enabled. See SetEventMask() and GetEventMask()
 **         methods.
 **     @param
 **         UserDataPtr     - Pointer to the user or
 **                           RTOS specific data. This pointer is passed
 **                           as the parameter of Init method.
 */
/* ===================================================================*/
void FLASH1_OnOperationComplete(LDD_TUserData *UserDataPtr) {
   /* Write your code here ... */
   flash_operation_completed = TRUE;
}

/*
 ** ===================================================================
 **     Event       :  Cpu_OnHardFault (module Events)
 **
 **     Component   :  Cpu [MK64FN1M0LL12]
 */
/*!
 **     @brief
 **         This event is called when the Hard Fault exception had
 **         occurred. This event is automatically enabled when the [Hard
 **         Fault] property is set to 'Enabled'.
 */
/* ===================================================================*/
void Cpu_OnHardFault(void) {
   /* Write your code here ... */
   PE_DEBUGHALT();
}

/*
 ** ===================================================================
 **     Event       :  ESP8266_Serial_OnError (module Events)
 **
 **     Component   :  ESP8266_Serial [Serial_LDD]
 */
/*!
 **     @brief
 **         This event is called when a channel error (not the error
 **         returned by a given method) occurs. The errors can be read
 **         using [GetError] method.
 **         The event is available only when the [Interrupt
 **         service/event] property is enabled.
 **     @param
 **         UserDataPtr     - Pointer to the user or
 **                           RTOS specific data. This pointer is passed
 **                           as the parameter of Init method.
 */
/* ===================================================================*/
void ESP8266_Serial_OnError(LDD_TUserData *UserDataPtr) {
   /* Write your code here ... */
   LDD_SERIAL_TError serial_error;
   LDD_TError get_error_result = ESP8266_Serial_GetError(&wifi_serial_device_data, &serial_error);

//                              LDD_SERIAL_RX_OVERRUN - Receiver overrun.
//                              LDD_SERIAL_PARITY_ERROR - Parity error
//                              (only if HW supports parity feature).
//                              LDD_SERIAL_FRAMING_ERROR - Framing error.
//                              LDD_SERIAL_NOISE_ERROR - Noise error.
}

/* END Events */

#ifdef __cplusplus
} /* extern "C" */
#endif 

/*!
 ** @}
 */
/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 10.5 [05.21]
 **     for the Freescale Kinetis series of microcontrollers.
 **
 ** ###################################################################
 */
