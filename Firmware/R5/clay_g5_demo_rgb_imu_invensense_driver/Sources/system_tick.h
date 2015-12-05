/*
 * system_tick.h
 *
 *  Created on: Sep 10, 2015
 *      Author: thebh_000
 */

#ifndef SYSTEM_TICK_H_
#define SYSTEM_TICK_H_
// includes //////////////////
#include "PE_Types.h"

// defines ///////////////////

// structs ///////////////////

// global vars ///////////////
extern uint32 power_on_time_msec;
extern bool tick_1msec;
extern bool tick_250msec;
extern bool tick_500msec;
extern uint8_t data_ready;

// function prototypes ///////
extern void init_tick();

extern void tick();

extern void delay_n_msec(uint32 n);

#endif /* SYSTEM_TICK_H_ */
