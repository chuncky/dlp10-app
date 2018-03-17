/* linux/arch/arm/mach-nuc970/include/mach/nuc970-etimer.h
 *
 * Copyright (c) 2014 Nuvoton technology corporation
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _SOFTTIMER_H_
#define _SOFTTIMER_H_


typedef struct{
	int id;
	volatile int enable;
	volatile int count;
	volatile int timeout;

}etimer_def;

extern etimer_def stimer0,stimer1,stimer2,stimer3,stimer4,stimer5;
void time_init(void);

void timer_start(etimer_def *etimer);
void timer_stop(etimer_def *etimer);
void timerprocess(void);

#define SET_LED_CMD system("echo 1 > /sys/class/gpio/gpio101/value")
#define CLEAR_LED_CMD system("echo 0 > /sys/class/gpio/gpio101/value")

#endif
