/*  
 * blinkenspiel / power.h
 * 
 * sleep / wakeup controller for blinkenspiel
 *  
 *  - can automatically go to sleep, if no key is pressed in certain time
 *  - wakes up every x seconds, checks input and goes to sleep if low
 * 
 *  Copyright (C) 2011 Manuel Jerger <nom@nomnom.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

void goSleep(void);


#ifndef __POWER_H__
# define __POWER_H__
#endif

#include <avr/interrupt.h>
#include <avr/wdt.h> 
#include <avr/sleep.h> 

// disable watchdog
#define initPower { \
    MCUSR &= ~(1<<WDRF); \
    WDTCSR = (1<<WDCE)|(1<<WDE); \
    WDTCSR = (1<<WDIF)|(0<<WDIE)|(1<<WDCE)|(0<<WDE)|(0<<WDP3)|(1<<WDP2)|(1<<WDP1)|(0<<WDP0); }\ 

#ifdef _USE_AUTOSHUTDOWN_
 static volatile uint8_t awake_ctr=0;
 static volatile uint16_t awake_div=0;
 #define SHUTDOWN_DELAY 60              // shutdown after 60 sec
 #define pollPower { if (awake_div++ > 4000) { awake_div=0; if (awake_ctr++ > SHUTDOWN_DELAY) { awake_ctr=0; goSleep(); } } }
 #define stayAwake { awake_ctr=0; awake_div=0;}
#endif
