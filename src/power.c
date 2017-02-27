/*  
 * blinkenspiel / power.c
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
 
#ifndef __POWER_H__
# include "power.h"
#endif
 
 
void goSleep(void) {
    
    uint8_t timsk_val = TIMSK;  // backup timer values
    
    // prepare sleep
    display_on = 0;
    shutdownDisplay();
    TIMSK &= ~(1 << TOIE0);     // disable timer interrupts
    TIMSK &= ~(1 << TOIE1);    // disable timer interrupts
    
    // set sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    
    do {
        
        wdt_reset();            // Reset Wathdog Timer to ensure sleep for one whole Watchdog Timer period
        
        // The ATTiny2313-way of enabling WDT for sleep wakeup:
        cli();
        WDTCSR |= (1<<WDIE);    // Enable Watchdog Interrupt Mode
        sleep_enable();
        sei();
        
        // go sleep
        sleep_cpu();

        // wake up here
        
        // disable sleep
        sleep_disable();
        
        pollKeys();
    } while (!anykey);
    
    display_on = 1;
    TIMSK = timsk_val;      // reenable timers
    WDTCSR &= ~(1<<WDIE);   // disable watchdog
    
    #if (defined WAKEUP_ACTION)
    WAKEUP_ACTION;
    #endif
}

// empty wdt ISR
ISR(WDT_OVERFLOW_vect) {}


