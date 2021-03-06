/*
 * Buzzer.c
 *
 *  Created on: Mar 15, 2016
 *      Author: thebh_000
 */

////Includes //////////////////////////////////////////////////////
#include "Clock.h"
#include "Buzzer.h"
// TODO: update PWM driver
//#include "BUZZER_PWM.h"

////Typedefs  /////////////////////////////////////////////////////

////Globals   /////////////////////////////////////////////////////

////Local vars/////////////////////////////////////////////////////
// TODO: update PWM driver
//static LDD_TDeviceData * buzzer_pwm_data;
static uint32_t notes[] = { 2093,            //c
                            2217,            //c#/db
                            2349,            //d
                            2489,            //d#/eb
                            2637,            //e
                            2794,            //f
                            2960,            //f#/gb
                            3136,            //g
                            3322,            //g#/ab
                            3520,            //a
                            3729,            //a#/bb
                            3951,            //b
                            0 };

static uint32_t buzzer_stop_time;

////Local Prototypes///////////////////////////////////////////////
uint16_t Scale_Percent(uint8_t percent);

////Global implementations ////////////////////////////////////////

bool Buzzer_Enable() {

   bool rval = false;

   // TODO: update PWM driver
//   LDD_TError err;

//   buzzer_pwm_data = BUZZER_PWM_Init(NULL);
//
//   err = BUZZER_PWM_SetFrequencyHz(buzzer_pwm_data, 0);
//   BUZZER_PWM_SetRatio16(buzzer_pwm_data, Scale_Percent(0));

   return rval;
}

bool Buzzer_Disable() {

   // TODO: update PWM driver
//   BUZZER_PWM_Deinit(buzzer_pwm_data);

   return true;
}

void Buzzer_Play_Note(NOTE_INDEX note, uint32_t duration_ms) {
   Buzzer_Play_Frequency(notes[note], duration_ms);
}

void Buzzer_Play_Frequency(uint32_t frequency, uint32_t duration_ms) {

   // TODO: update PWM driver.
//   if (frequency > 0) {
//      BUZZER_PWM_SetFrequencyHz(buzzer_pwm_data, frequency);
//      BUZZER_PWM_SetRatio16(buzzer_pwm_data, Scale_Percent(75));
//   } else {
//      BUZZER_PWM_SetRatio16(buzzer_pwm_data, 0);
//   }

   if (duration_ms > 0) {
      buzzer_stop_time = Millis() + duration_ms;
   } else {
      buzzer_stop_time = 0;
   }
}

void Buzzer_Stop_Check() {
   if (buzzer_stop_time > 0 && Millis() > buzzer_stop_time) {
      Buzzer_Play_Frequency(0, 0);
   }
}

////Local implementations ////////////////////////////////////////
uint16_t Scale_Percent(uint8_t percent) {
   return (uint16_t)(((double) percent / 100.0) * 0xFFFF);
}
