/*
 * Clock.c
 *
 *  Created on: Sep 10, 2015
 *      Author: thebh_000
 */

#include "Clock.h"

// defines ///////////////////

// structs ///////////////////

// global vars ///////////////
uint32_t power_on_time_msec;
uint8_t  tick_1msec;
uint8_t  tick_250msec;
uint8_t  tick_500msec;

// function prototypes ///////

void Enable_Clock () {
	// Initialize the timer component
	(void) Timer_1ms_Init (NULL);
}

//reset all of the flags to false and the power on time to 0.
void Start_Clock () {
	
    power_on_time_msec = 0;
    tick_1msec = FALSE;
    tick_250msec = FALSE;
    tick_500msec = FALSE;
}

//call this every 1msec to increment the power on time and set the flags.
void Tick () {
	
    ++power_on_time_msec;
    tick_1msec = TRUE;

    if (!(power_on_time_msec % 250))
    {
        tick_250msec = TRUE;
    }

    if (!(power_on_time_msec % 500))
    {
        tick_500msec = TRUE;
    }
}

///will block for n milliseconds
void Wait (uint32_t milliseconds) {
	
    int start = power_on_time_msec;
    while (power_on_time_msec - start < milliseconds) {
    	;
    }
}

uint32_t Millis () {
	return power_on_time_msec;
}

Color_RGB
    colors[] =
            {
                    { LED_MODE_OFF, LED_MODE_OFF, LED_MODE_OFF },        //off
                    { LED_MODE_MED, LED_MODE_MED, LED_MODE_OFF },        //rg
                    { LED_MODE_OFF, LED_MODE_MED, LED_MODE_MED },        //gb
                    { LED_MODE_MED, LED_MODE_OFF, LED_MODE_MED }        //rb
            };

void Monitor_Periodic_Events () {
	
	mpu_values v = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	
	// LEDs
	if (tick_1msec) {
		tick_1msec = FALSE;
		
		// TODO: Perform any periodic actions (1 ms).
		
        if (!(power_on_time_msec % 30))
        {
            get_mpu_readings(&v);

            if (v.x_accel < 50)
            {
                Set_LED_Output (RGB_12, colors + 1);        //-x
                Set_LED_Output (RGB_4, colors + 0);        //+x
            }
            else if (v.x_accel > 50)
            {
            	Set_LED_Output (RGB_12, colors + 0);        //-x
            	Set_LED_Output (RGB_4, colors + 1);        //+x
            }

            if (v.y_accel < 50)
            {
            	Set_LED_Output (RGB_1, colors + 1);        //-y
            	Set_LED_Output (RGB_9, colors + 0);        //+y
            }
            else if (v.y_accel > 50)
            {
            	Set_LED_Output (RGB_1, colors + 0);        //-y
            	Set_LED_Output (RGB_9, colors + 1);        //+y
            }

            if (v.z_accel < 15010)        //-z
            {
            	Set_LED_Output (RGB_10, colors + 1);
            	Set_LED_Output (RGB_7, colors + 1);
            	Set_LED_Output (RGB_6, colors + 1);
            	Set_LED_Output (RGB_3, colors + 1);
            }
            else if (v.z_accel > 15030)        //+z
            {
            	Set_LED_Output (RGB_10, colors + 3);
            	Set_LED_Output (RGB_7, colors + 3);
            	Set_LED_Output (RGB_6, colors + 3);
            	Set_LED_Output (RGB_3, colors + 3);
            }

        }
	}
	
	if (tick_250msec) {
		tick_250msec = FALSE;
		
		// TODO: Perform any periodic actions (1 ms).
	}
	
	if (tick_500msec) {
		tick_500msec = FALSE;
		
		// TODO: Perform any periodic actions (1 ms).

		/*
		//toggle LEDs
		LED1_PutVal(LED1_DeviceData, !led_state);
		LED2_PutVal(LED2_DeviceData, led_state);
		led_state = !led_state;

		Color_RGB * derp = colors + color_index;

		Set_LED_Output((RGB_LED) led_index, derp);

		if (++led_index % RGB_INVALID == 0)
		{
			led_index = 0;
			color_index = (color_index + 1) % 3;
		}
		*/
	}
}
