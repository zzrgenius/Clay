/*
 * RGB_LED.c
 *
 *  Created on: Jan 26, 2016
 *      Author: thebh
 */

////includes
#include "RGB_LED.h"
#include "PE_Types.h"
#include "I2C2.h"
#include "LED_SDB.h"
#include "Clock.h"

////defines
#define SHUTDOWN_REG_ADDR				0x00
#define UPDATE_REGISTER					0x25
#define GLOBAL_CONTROL_ADDR				0x4A
#define RESET_REG_ADDR   				0x4F

#define LED_CONTROL_OUT_MASK			0x01
#define LED_CONTROL_SL_MASK				0x06

#define I2C_SLAVE_ADDR					0x3C

////typedefs
typedef struct
{
	RGB_LED_Index index;
	uint8_t R_Control_Addr;
	uint8_t G_Control_Addr;
	uint8_t B_Control_Addr;
	uint8_t R_PWM_Addr;
	uint8_t G_PWM_Addr;
	uint8_t B_PWM_Addr;
} RGB_LED_Channel;

////global vars

////local vars
static RGB_LED_Channel LED_Channels[RGB_MAX] =
{
{ RGB1, 0x26, 0x27, 0x28, 0x01, 0x02, 0x03 },
{ RGB2, 0x29, 0x2A, 0x2B, 0x04, 0x05, 0x06 },
{ RGB3, 0x2C, 0x2D, 0x2E, 0x07, 0x08, 0x09 },

{ RGB4, 0x2F, 0x30, 0x31, 0x0A, 0x0B, 0x0C },
{ RGB5, 0x32, 0x33, 0x34, 0x0D, 0x0E, 0x0F },
{ RGB6, 0x35, 0x36, 0x37, 0x10, 0x11, 0x12 },

{ RGB7, 0x38, 0x39, 0x3A, 0x13, 0x14, 0x15 },
{ RGB8, 0x3B, 0x3C, 0x3D, 0x16, 0x17, 0x18 },
{ RGB9, 0x3E, 0x3F, 0x40, 0x19, 0x1A, 0x1B },

{ RGB10, 0x41, 0x42, 0x43, 0x1C, 0x1D, 0x1E },
{ RGB11, 0x44, 0x45, 0x46, 0x1F, 0x20, 0x21 },
{ RGB12, 0x47, 0x48, 0x49, 0x22, 0x23, 0x24 } };

////local function prototypes

////global function implementations

void RGB_LED_Enable()
{
	LED_SDB_PutVal(NULL, 1);

	uint8_t temp[] =
	{ 0x00, 0x01, //shutdown register needs to be 1 to operate.
			0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, //4.33mA
			0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70,

			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,

			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,

			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,

			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK,
			LED_CONTROL_OUT_MASK | LED_CONTROL_SL_MASK

	};

	I2C2_SelectSlaveDevice(I2C2_DeviceData, LDD_I2C_ADDRTYPE_7BITS, I2C_SLAVE_ADDR);
	Wait(1);
	I2C2_MasterSendBlock(I2C2_DeviceData, temp, 74, LDD_I2C_SEND_STOP);
	Wait(1);
}

void RGB_LED_Start()
{
}

void RGB_LED_Stop()
{
}

void RGB_LED_Pause()
{
}

void RGB_LED_SetColor(RGB_LED_Index LED, RGB_Color Color)
{
}

////local function implementations
