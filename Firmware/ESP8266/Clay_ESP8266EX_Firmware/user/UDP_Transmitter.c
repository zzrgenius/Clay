/*
 * UDP_Transmitter.cpp
 *
 *  Created on: Feb 22, 2016
 *      Author: thebh
 *
 *      This class consumes (Clay_Message)s from a queue and transmits them over UDP
 */

/*
 * States:
 *      Disabled -- no transmissions occur
 *      Configure -- Create a UDP socket and prepare it for listening.
 *          -upon completion, we go to Idle
 *      Idle -- no messages available in buffer
 *          -messages become available, we go to Buffer_Message
 *          -Disable message sends us back to Disabled
 *      Buffer_Message -- message was available in queue
 *          -Message consumed out of queue into transmit buffer. Go to Send_Message. Leave message in queue for now.
 *          -Disable message sends us back to Disabled.
 *      Send_Message (combine with Buffer_Message?)
 *          -Send the message waiting in the buffer. Return to idle upon successful completion
 *      Retry?
 * */

////Includes //////////////////////////////////////////////////////
#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "uart.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "UDP_Transmitter.h"

#include "../include/System_Monitor.h"
#include "Clay_Config.h"

#include "Wifi_Message_Serialization.h"
#include "Message_Queue.h"
#include "Message.h"
#include "Queues.h"

////Macros  ///////////////////////////////////////////////////////
#define MESSAGE_TRIGGER_LEVEL			10

////Typedefs  /////////////////////////////////////////////////////
typedef enum
{
	Disable, Configure, Idle, Buffer_Message, Send_Message, UDP_STATE_MAX
} UDP_Transmitter_States;
////Globals   /////////////////////////////////////////////////////

////Local vars/////////////////////////////////////////////////////
static UDP_Transmitter_States State;

static int ret;

static uint8_t *udp_tx_buffer;
static uint16_t udp_tx_count;

static bool promoted;

static struct sockaddr_in DestinationAddr;
static struct sockaddr_in server_addr;

static int32 transmit_sock;
static int32 testCounter;

static Message_Type tempIgnoredMessageType;

static Message * m;
static Message * temp_msg_ptr;

static bool task_running = false;

////Local Prototypes///////////////////////////////////////////////
static bool Connect();
static bool Transmit();
static bool Message_Available();
static bool Check_Needs_Promotion();

////Global implementations ////////////////////////////////////////
bool ICACHE_RODATA_ATTR UDP_Transmitter_Init()
{
	bool rval = true;

	if (!task_running)
	{
		promoted = false;

		taskENTER_CRITICAL();
		Free_Message_Queue(&outgoing_udp_message_queue);
		udp_tx_buffer = zalloc(UDP_TX_BUFFER_SIZE_BYTES);
		taskEXIT_CRITICAL();

		State = Disable;

		xTaskHandle UDP_transmit_handle;

		xTaskCreate(UDP_Transmitter_Task, "udptx1", 512, NULL, DEFAULT_PRIORITY,
				&UDP_transmit_handle);

		System_Register_Task(TASK_TYPE_UDP_TX, UDP_transmit_handle,
				Check_Needs_Promotion);

		testCounter = 0;

		task_running = true;
	}
	else
	{
		rval = false;
	}

	return rval;
}

void ICACHE_RODATA_ATTR UDP_Transmitter_Deinit()
{
	if (task_running)
	{
		lwip_close(transmit_sock);
		transmit_sock = -1;

		free(udp_tx_buffer);

		task_running = false;
		System_Stop_Task(TASK_TYPE_UDP_TX);
	}
}

void ICACHE_RODATA_ATTR UDP_Transmitter_Task()
{
	for (;;)
	{
		switch (State)
		{
		case Disable:
		{
			if (wifi_station_get_connect_status() == STATION_GOT_IP)
			{
				State = Configure;
			}
			break;
		}

		case Configure:
		{
			if (Connect())
			{
				State = Idle;
			}
			break;
		}

		case Idle:
		{
			if (Message_Available())
			{
				State = Buffer_Message;
			}
			break;
		}

		case Buffer_Message:
		{
			taskENTER_CRITICAL();
			memset(&DestinationAddr, 0, sizeof(DestinationAddr));
			taskEXIT_CRITICAL();

			taskENTER_CRITICAL();
			temp_msg_ptr = Dequeue_Message(&outgoing_udp_message_queue);
			taskEXIT_CRITICAL();

			if (temp_msg_ptr != NULL)
			{
				--outgoing_udp_message_count;

				taskYIELD();

				taskENTER_CRITICAL();
				udp_tx_count = Serialize_Message_Content(temp_msg_ptr,
						udp_tx_buffer, UDP_TX_BUFFER_SIZE_BYTES);
				taskEXIT_CRITICAL();

				taskYIELD();

				taskENTER_CRITICAL();
				Deserialize_Address(temp_msg_ptr->destination, &DestinationAddr,
						&tempIgnoredMessageType);
				taskEXIT_CRITICAL();

				Delete_Message(temp_msg_ptr);

				taskYIELD();
			}

			State = Send_Message;
			break;
		}

		case Send_Message:
		{
			Transmit();
			State = Idle;
			break;
		}

		case UDP_STATE_MAX:
		default:
		{
			break;
		}
		}

		if (!Message_Available())
		{
			taskYIELD();
		}
	}
}

////Local implementations /////////////////////////////////////////
static bool ICACHE_RODATA_ATTR ICACHE_RODATA_ATTR Connect()
{
	bool rval = false;

	taskENTER_CRITICAL();
	memset(&server_addr, 0, sizeof(server_addr));
	taskEXIT_CRITICAL();

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(UDP_TX_PORT);
	server_addr.sin_len = sizeof(server_addr);

	///create the socket
	do
	{
		transmit_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (transmit_sock == -1)
		{
			vTaskDelay(1000 / portTICK_RATE_MS);
		}
	} while (transmit_sock == -1);

	//bind the socket
	do
	{
		ret = bind(transmit_sock, (struct sockaddr * ) &server_addr,
				sizeof(server_addr));
		if (ret != 0)
		{
			vTaskDelay(1000 / portTICK_RATE_MS);
		}
	} while (ret != 0);
	rval = true;

	return rval;
}

static bool ICACHE_RODATA_ATTR Transmit()
{
	bool rval = false;
	rval = udp_tx_count
			== sendto(transmit_sock, (uint8* ) udp_tx_buffer, udp_tx_count, 0,
					(struct sockaddr * ) &DestinationAddr,
					sizeof(DestinationAddr));

	return rval;
}

static bool ICACHE_RODATA_ATTR Message_Available()
{
	bool rval = false;

	taskENTER_CRITICAL();
	m = Peek_Message(&outgoing_udp_message_queue);
	taskEXIT_CRITICAL();

	if (m != NULL)
	{
		rval = true;
	}

	return rval;
}

static bool ICACHE_RODATA_ATTR Check_Needs_Promotion()
{
	taskENTER_CRITICAL();
	bool rval = outgoing_udp_message_count;
	taskEXIT_CRITICAL();

	return rval;
}
