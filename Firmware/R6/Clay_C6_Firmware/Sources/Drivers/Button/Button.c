////Includes //////////////////////////////////////////////////////
#include "BUTTON_IN.h"
#include "Button.h"
#include "Clock.h"

////Typedefs  /////////////////////////////////////////////////////
typedef enum
{
   BUTTON_EVENT_PRESS,
   BUTTON_EVENT_HOLD,
   BUTTON_EVENT_RELEASE,
   BUTTON_EVENT_TYPE_MAX
} Button_Event_Type;

typedef struct
{
   Button_Handler handler;
   Button_Event_Type type;
   bool call_handler;
   uint32_t hold_duration;
} Button_Callback_Struct;

////Globals   /////////////////////////////////////////////////////

////Local vars/////////////////////////////////////////////////////
static Button_Callback_Struct callbacks[BUTTON_CALLBACK_COUNT];
static bool button_state;
static uint32_t button_press_start_time;
static LDD_TDeviceData * button_in_data;

////Local Prototypes///////////////////////////////////////////////
int Find_Callback(Button_Handler handler);
bool Register_Callback(Button_Handler handler, Button_Event_Type type, uint32_t hold_duration);
void Unregister_Callback(Button_Handler handler);
bool Read_Button_State();

////Global implementations ////////////////////////////////////////
bool Button_Enable() {

   bool rval = FALSE;

   button_in_data = BUTTON_IN_Init(NULL);
   button_state = BUTTON_IN_GetPortValue(button_in_data);

   return rval;
}

void Button_Disable() {

   BUTTON_IN_Deinit(button_in_data);
}

bool Button_Get_Status() {

#ifdef C6R2
   button_state = (BUTTON_IN_GetPortValue(button_in_data) & BUTTON_IN_button_field_MASK) ? FALSE : TRUE;     //c6r2 -- button active low
#else
   button_state = (BUTTON_IN_GetPortValue(button_in_data) & BUTTON_IN_button_field_MASK) ? TRUE : FALSE;     //c6r3 -- button active high
#endif

   return button_state;
}

bool Button_Register_Press_Response(Button_Handler handler) {

   return Register_Callback(handler, BUTTON_EVENT_PRESS, 0);
}

void Button_Unregister_Press_Response(Button_Handler handler) {

   Unregister_Callback(handler);
}

bool Button_Register_Hold_Response(uint32_t duration_ms, Button_Handler handler) {

   return Register_Callback(handler, BUTTON_EVENT_HOLD, duration_ms);
}

void Button_Unregister_Hold_Response(Button_Handler handler) {

   Unregister_Callback(handler);
}

bool Button_Register_Release_Response(Button_Handler handler) {

   return Register_Callback(handler, BUTTON_EVENT_RELEASE, 0);
}

void Button_Unregister_Release_Response(Button_Handler handler) {

   Unregister_Callback(handler);
}

void Button_Event_Handler() {

   //get_status updates button_state.
   Button_Get_Status();

   if (button_state) {
      if (button_press_start_time == 0) {
         button_press_start_time = Millis();
      }
   } else {
      button_press_start_time = 0;
   }

   for (int i = 0; i < BUTTON_CALLBACK_COUNT; ++i) {
      if (callbacks[i].handler != NULL) {
         switch (callbacks[i].type) {

            case BUTTON_EVENT_PRESS: {     //TODO: update button_event_press to require that the button not be held.
               if (button_state) {
                  callbacks[i].call_handler = TRUE;
               }
               break;
            }

            case BUTTON_EVENT_HOLD: {

               if (button_state && (Millis() - button_press_start_time) > callbacks[i].hold_duration) {
                  callbacks[i].call_handler = TRUE;
               }

               break;
            }

            case BUTTON_EVENT_RELEASE: {

               if (!button_state) {
                  callbacks[i].call_handler = TRUE;
               }
               break;
            }

            case BUTTON_EVENT_TYPE_MAX:
            default: {
               break;
            }
         }
      }
   }
}

void Button_Periodic_Call() {

   //get_status updates button_state.
   Button_Get_Status();

   for (int i = 0; i < BUTTON_CALLBACK_COUNT; ++i) {
      if (callbacks[i].call_handler) {
         callbacks[i].handler();
         callbacks[i].call_handler = FALSE;
      } else if (button_state
                 && callbacks[i].type == BUTTON_EVENT_HOLD
                 && (Millis() - button_press_start_time) > callbacks[i].hold_duration) {
         callbacks[i].handler();
      }
   }
}

////Local implementations ////////////////////////////////////////
int Find_Callback(Button_Handler handler) {

   int rval = -1;

   for (int i = 0; i < BUTTON_CALLBACK_COUNT; ++i) {
      if (callbacks[i].handler == handler) {
         rval = i;
      }
   }

   return rval;
}

bool Register_Callback(Button_Handler handler, Button_Event_Type type, uint32_t hold_duration) {

   bool rval = FALSE;
   int open_index;

   if (Find_Callback(handler) > 0) {
      rval = TRUE;
   } else {
      open_index = Find_Callback(NULL);
      if (open_index > 0) {
         callbacks[open_index].handler = handler;
         callbacks[open_index].type = type;
         callbacks[open_index].hold_duration = hold_duration;
         callbacks[open_index].call_handler = FALSE;

         rval = TRUE;
      } else {
         rval = FALSE;
      }
   }

   return rval;
}

void Unregister_Callback(Button_Handler handler) {

   int open_index = Find_Callback(handler);

   if (open_index > 0) {
      callbacks[open_index].handler = NULL;
   }
}
