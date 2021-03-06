/*
 * Clock.h
 *
 * Created on: Sep 10, 2015
 * Authors: Ben Hefner, Michael Gubbels
 */

#ifndef CLOCK_H_
#define CLOCK_H_
// includes //////////////////
#include "PE_Types.h"

// defines ///////////////////

// structs ///////////////////

// global vars ///////////////
extern uint32_t power_on_time_msec;
extern bool tick_1msec;
extern bool tick_250msec;
extern bool tick_500msec;

// TODO: Enable_Clock ()

extern void Start_Clock ();

extern void Tick ();

extern void Wait (uint32_t milliseconds);

extern uint32_t Millis ();

// extern void Stop_Clock ();

// TODO: Disable_Clock ()

#endif /* CLOCK_H_ */
