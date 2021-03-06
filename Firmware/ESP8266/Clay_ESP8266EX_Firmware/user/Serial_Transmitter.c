/*
 * Serial_Transmitter.cpp
 *
 *  Created on: Feb 22, 2016
 *      Author: thebh
 *
 *      This class consumes Clay_Message types from a buffer and transmits them over serial to a microcontroller.
 */

/*
 * States:
 *      Disable -- No transmission occurs.
 *          -Go to configure when enabled
 *      Configure -- set up serial port, interrupts, etc
 *          -Go to Idle upon completion
 *      Idle
 *          -No messages in queue that need to go to micro.
 *          -When a message appears in queue, go to Message_available
 *          -Disable can send us back to Disabled state
 *      Message_Available
 *          -Set interrupt output line low, wait for uC to acknowledge.
 *          -When uC interrupt comes in, go to Transmitting
 *          -Disable command will reset interrupt output and send us to Disable.
 *      Transmitting
 *          -Send one message.
 *          -Reset interrupt output and return to Idle upon completion.
 * */

////Includes //////////////////////////////////////////////////////
#include "esp_common.h"
#include "GPIO.h"
#include "UART.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "UDP_Transmitter.h"
#include "Serial_Receiver.h"
#include "Serial_Transmitter.h"

#include "../include/AddressSerialization.h"
#include "../include/System_Monitor.h"
#include "Clay_Config.h"
#include "Message_Queue.h"
#include "ESP_Utilities.h"
#include "Queues.h"

////Macros ////////////////////////////////////////////////////////
#define MESSAGE_TRIGGER_LEVEL			10
#define TEXT_NONE	"none"

////Typedefs  /////////////////////////////////////////////////////
typedef enum
{
	Disable,
	Configure,
	Idle,
	Message_Available,
	Wait_For_Transmit_Ok,
	Transmitting,
	Transmitting_Done,
	UDP_STATE_MAX
} Serial_Transmitter_States;
////Globals   /////////////////////////////////////////////////////

////Local vars/////////////////////////////////////////////////////
static Serial_Transmitter_States state;

static int serial_tx_length;
static uint8 * serial_tx_buffer;
static Message * temp_msg_ptr;

static uint32 time_temp;
static bool promoted;

////Local Prototypes///////////////////////////////////////////////
static bool Check_Needs_Promotion();

////Global implementations ////////////////////////////////////////
bool ICACHE_RODATA_ATTR Serial_Transmitter_Init()
{
	bool rval = true;
	promoted = false;

	taskENTER_CRITICAL();
	serial_tx_buffer = zalloc(SERIAL_TX_BUFFER_SIZE_BYTES);
	taskEXIT_CRITICAL();

	state = Idle;

	xTaskHandle serial_tx_handle;

	xTaskCreate(Serial_Transmitter_Task, "uarttx1", 256, NULL, DEFAULT_PRIORITY,
			&serial_tx_handle);

	System_Register_Task(TASK_TYPE_SERIAL_TX, serial_tx_handle,
			Check_Needs_Promotion);

	return rval;
}

void ICACHE_RODATA_ATTR Serial_Transmitter_Task()
{
	for (;;)
	{
		switch (state)
		{
		case Disable:
		{
			if (wifi_station_get_connect_status() == STATION_GOT_IP)
			{
				state = Idle;
			}
			break;
		}

		case Configure:
		{
			break;
		}

		case Idle:
		{
			taskENTER_CRITICAL();
			if (Has_Messages(&incoming_message_queue))
			{
				state = Message_Available;
			}
			taskEXIT_CRITICAL();

			break;
		}

		case Message_Available:
		{
			taskENTER_CRITICAL();
			temp_msg_ptr = Dequeue_Message(&incoming_message_queue);
			memset(serial_tx_buffer, 0, SERIAL_TX_BUFFER_SIZE_BYTES);
			taskEXIT_CRITICAL();

			if (temp_msg_ptr != NULL)
			{
				--incoming_message_count;
				taskENTER_CRITICAL();
				serial_tx_length = Serialize_Message_With_Message_Header(
						temp_msg_ptr, serial_tx_buffer,
						SERIAL_TX_BUFFER_SIZE_BYTES - 1);
				taskEXIT_CRITICAL();

				Delete_Message(temp_msg_ptr);

				if (serial_tx_length > 0)
				{

					time_temp = system_get_time();

#if(CLAY_INTERRUPT_OUT_PIN == 16)
					gpio16_output_set(0);
#else
					GPIO_OUTPUT(BIT(CLAY_INTERRUPT_OUT_PIN), 0);
#endif

					//wait 1ms
					while ((system_get_time() - time_temp) < 1000)
					{
						taskYIELD();
					}

#if(CLAY_INTERRUPT_OUT_PIN == 16)
					gpio16_output_set(1);
#else
					GPIO_OUTPUT(BIT(CLAY_INTERRUPT_OUT_PIN), 1);
#endif

					state = Wait_For_Transmit_Ok;
				}
			}

			break;
		}

		case Wait_For_Transmit_Ok:
		{
#ifdef PRINT_HIGH_WATER
			taskENTER_CRITICAL();
			printf("stx send\r\n");
			taskEXIT_CRITICAL();

//			UART_WaitTxFifoEmpty(UART0);

			DEBUG_Print_High_Water();
#endif

			taskENTER_CRITICAL();
			printf("  ");
			UART_WaitTxFifoEmpty(UART0);
			uart_tx_array(UART0, serial_tx_buffer, serial_tx_length);
			UART_WaitTxFifoEmpty(UART0);
			printf("          ");
			UART_WaitTxFifoEmpty(UART0);
			taskEXIT_CRITICAL();
			state = Transmitting;

			break;
		}

		case Transmitting:
		{
			if (UART_CheckTxFifoEmpty(UART0))
			{
				state = Transmitting_Done;
			}
			break;
		}

		case Transmitting_Done:
		{
			state = Idle;
			break;
		}

		case UDP_STATE_MAX:
		default:
		{
			break;
		}
		}

		taskYIELD();
	}
}

void ICACHE_RODATA_ATTR Send_Message_To_Master(char * message,
		Message_Type type)
{
	Message * temp_msg;
	char type_string[CLAY_MESSAGE_TYPE_STRING_MAX_LENGTH];

	taskENTER_CRITICAL();
	temp_msg = Create_Message();
	Set_Message_Type(temp_msg, message_type_strings[type]);
	Set_Message_Source(temp_msg, TEXT_NONE);
	Set_Message_Destination(temp_msg, TEXT_NONE);
	Set_Message_Content_Type(temp_msg, content_type_strings[CONTENT_TYPE_TEXT]);
	Set_Message_Content(temp_msg, message, strlen(message));
	taskEXIT_CRITICAL();

	taskENTER_CRITICAL();
	incoming_message_count = Queue_Message(&incoming_message_queue, temp_msg);
	taskEXIT_CRITICAL();
}

////Local implementations /////////////////////////////////////////
static bool ICACHE_RODATA_ATTR Check_Needs_Promotion()
{
	taskENTER_CRITICAL();
	bool rval = (incoming_message_count > 5);
	taskEXIT_CRITICAL();

	promoted = rval;

	return rval;
}
