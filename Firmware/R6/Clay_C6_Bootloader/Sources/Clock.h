/*
 * Clock.h
 *
 * Created on: Sep 10, 2015
 * Authors: Ben Hefner, Michael Gubbels
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include "PE_Types.h"

extern uint32_t power_on_time_msec;
extern bool tick_1msec;
extern bool tick_5msec;
extern bool tick_250msec;
extern bool tick_500msec;

extern uint8_t Enable_Clock();

extern uint8_t Start_Clock();

extern void Tick();

extern void Wait(uint32_t milliseconds);

extern uint32_t Millis();

extern void Monitor_Periodic_Events();

// extern void Stop_Clock ();

// TODO: Disable_Clock ()

#endif /* CLOCK_H_ */