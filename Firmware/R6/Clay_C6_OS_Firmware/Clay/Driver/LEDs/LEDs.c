#include "LEDs.h"

int8_t LED_Enable() {

   // TODO: update LED GPIO
//	(void) LED1_Init (NULL);
//	(void) LED2_Init (NULL);
//
//	LED1_SetDir (NULL, true); // Set as output.
//	LED2_SetDir (NULL, true); // Set as output.

//	LED2_PutVal(LED2_DeviceData, !(hb_led_count & 0x01));

   return true;
}

void Set_LED_State(uint8_t number, uint8_t state) {     // i.e., set discrete state of the channel (on or off)

//	uint8_t stateValue = 0;

   // TODO: Set the specified channel to be an output if it is not configured as such (and save state if it was an input so it can resume that behavior later).

   // Prepare to set direction as input or output.
   if (state == ON_CHANNEL) {
//		stateValue = true; // Set as output.
   } else if (state == OFF_CHANNEL) {
//		stateValue = false; // Set as input.
   }

   // TODO: update LED GPIO
   // Actually set the channel based on preparations above.
   if (number == LED1) {
//		LED1_PutVal (NULL, stateValue);
   } else if (number == LED2) {
//		LED2_PutVal (NULL, stateValue);
   }
}

uint8_t Get_LED_State(uint8_t number) {     // i.e., get discrete input state

   // TODO: update LED gpios
   // Actually set the channel based on preparations above.
   if (number == LED1) {
//		return (uint8_t) LED1_GetVal (NULL);
   } else if (number == LED2) {
//		return (uint8_t) LED2_GetVal (NULL);
   }

   return 0;
}

int8_t Perform_Status_LED_Effect() {

   Set_LED_State(LED1, ON_CHANNEL);
   Set_LED_State(LED2, OFF_CHANNEL);
   Wait(50);
   Set_LED_State(LED1, OFF_CHANNEL);
   Set_LED_State(LED2, ON_CHANNEL);
   Wait(50);
   Set_LED_State(LED1, ON_CHANNEL);
   Set_LED_State(LED2, OFF_CHANNEL);
   Wait(50);
   Set_LED_State(LED1, OFF_CHANNEL);
   Set_LED_State(LED2, ON_CHANNEL);
   Wait(50);
   Set_LED_State(LED1, OFF_CHANNEL);
   Set_LED_State(LED2, OFF_CHANNEL);

   return true;
}
