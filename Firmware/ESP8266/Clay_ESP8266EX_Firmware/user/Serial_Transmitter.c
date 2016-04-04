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
#include "Clay_Config.h"
#include "Message_Queue.h"
#include "ESP_Utilities.h"

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

static uint8 * serial_tx_buffer;
static uint32 serial_tx_count;
static Message * temp_message;

static uint32 time_temp;
static xTaskHandle serial_tx_task;

////Local Prototypes///////////////////////////////////////////////

////Global implementations ////////////////////////////////////////
bool ICACHE_RODATA_ATTR Serial_Transmitter_Init()
{
	bool rval = true;

	taskENTER_CRITICAL();
	serial_tx_buffer = zalloc(SERIAL_TX_BUFFER_SIZE_BYTES);
	Initialize_Message_Queue(&incoming_message_queue);
	taskEXIT_CRITICAL();

	state = Idle;

	xTaskCreate(Serial_Transmitter_Task, "uarttx1", 128, NULL, 2,
			serial_tx_task);

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
			temp_message = Peek_Message(&incoming_message_queue);
			taskEXIT_CRITICAL();

			if (temp_message != NULL)
			{
				state = Message_Available;
				UART_ResetTxFifo(UART0);
			}
			break;
		}

		case Message_Available:
		{
			taskENTER_CRITICAL();
			temp_message = Dequeue_Message(&incoming_message_queue);
			taskEXIT_CRITICAL();

			serial_tx_count = strlen(temp_message->content);

			taskENTER_CRITICAL();
			sprintf(serial_tx_buffer, "%s%s%s%s%s%s%s%s", temp_message->content,
					message_delimiter, temp_message->message_type,
					type_delimiter, temp_message->source, address_delimiter,
					temp_message->destination, address_terminator);
			taskEXIT_CRITICAL();

			//dequeue alloc's a message.
			free(temp_message);
			temp_message = NULL;

			time_temp = system_get_time();

#if(CLAY_INTERRUPT_OUT_PIN == 16)
			gpio16_output_set(0);
#else
			GPIO_OUTPUT(BIT(CLAY_INTERRUPT_OUT_PIN), 0);
#endif

			//wait 1ms
			while ((system_get_time() - time_temp) > 1000)
			{
				taskYIELD();
			}

#if(CLAY_INTERRUPT_OUT_PIN == 16)
			gpio16_output_set(1);
#else
			GPIO_OUTPUT(BIT(CLAY_INTERRUPT_OUT_PIN), 1);
#endif

			state = Wait_For_Transmit_Ok;

			break;
		}

		case Wait_For_Transmit_Ok:
		{
#ifdef PRINT_HIGH_WATER
			taskENTER_CRITICAL();
			printf("stx send\r\n");
			taskEXIT_CRITICAL();
			portENTER_CRITICAL();
			UART_WaitTxFifoEmpty(UART0);
			portEXIT_CRITICAL();

			DEBUG_Print_High_Water();
#endif

			taskENTER_CRITICAL();
			printf(serial_tx_buffer);
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

		vTaskDelay(5 / portTICK_RATE_MS);
//		taskYIELD();
	}
}

void Send_Message_To_Master(char * message, Message_Type type)
{
	Message m;
	char type_string[CLAY_MESSAGE_TYPE_STRING_MAX_LENGTH];

//	DEBUG_Print("send message");
//	DEBUG_Print(message);

	taskENTER_CRITICAL();
	Get_Message_Type_Str(type, type_string);
	taskEXIT_CRITICAL();

	taskENTER_CRITICAL();
	Initialize_Message(&m, type_string, ":", ":", message);
	taskEXIT_CRITICAL();

	taskENTER_CRITICAL();
	Queue_Message(&incoming_message_queue, &m);
	taskEXIT_CRITICAL();

//	DEBUG_Print("message enqueued");
}

////Local implementations /////////////////////////////////////////
