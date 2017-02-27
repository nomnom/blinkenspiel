/*
 *  blinkenspiel / keyboard.c
 * 
 *  4 button "keyboard":
 *  
 *               attiny2313
 *   UP         pin 2  / PD0        KEY1
 *   DOWN       pin 3  / PD1        KEY2
 *   LEFT       pin 11 / PD6        KEY3
 *   RIGHT      pin 18 / PB6        KEY4
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



#ifndef __KEYBOARD_H__
# include "keyboard.h"
#endif


static volatile uint8_t key1_state = 0;
static volatile uint8_t key2_state = 0;
static volatile uint8_t key3_state = 0;
static volatile uint8_t key4_state = 0;



// update keys (frequency determins debounce delay)
void pollKeys(void) {

#if (defined KEY1_DOWN_ACTION || defined KEY1_UP_ACTION)
    if (PIN_KEY1 & KEY1) {
        if (key1_state == 2) {         // key1 released
            key1_state = 3;
            #if (defined KEY1_UP_ACTION)
            KEY1_UP_ACTION;
            #endif
        } else {                       // still released after one poll cycle
            key1_state = 0;
        }
    } else {
         if (key1_state == 0) {
            key1_state = 1; 
            #if (defined KEY1_DOWN_ACTION)
            KEY1_DOWN_ACTION;
            #endif
        } else {  // still pressed after one poll cycle
             key1_state = 2; 
        }
    }
#endif

#if (defined KEY2_DOWN_ACTION || defined KEY2_UP_ACTION)        
    if (PIN_KEY2 & KEY2) {
        if (key2_state == 2) {         // key2 released
            key2_state = 3;
            #if (defined KEY2_UP_ACTION)
            KEY2_UP_ACTION;
            #endif
        } else {                       // still released after one poll cycle
            key2_state = 0;
        }
    } else {
         if (key2_state == 0) {
            key2_state = 1; 
            #if (defined KEY2_DOWN_ACTION)
            KEY2_DOWN_ACTION;
            #endif
        } else {  // still pressed after one poll cycle
             key2_state = 2; 
        }
    }
#endif

#if (defined KEY3_DOWN_ACTION || defined KEY3_UP_ACTION)
    if (PIN_KEY3 & KEY3) {
        if (key3_state == 2) {         // key3 released
            key3_state = 3;
            #if (defined KEY3_UP_ACTION)
            KEY3_UP_ACTION;
            #endif
        } else {                       // still released after one poll cycle
            key3_state = 0;
        }
    } else {
         if (key3_state == 0) {
            key3_state = 1; 
            #if (defined KEY3_DOWN_ACTION)
            KEY3_DOWN_ACTION;
            #endif
        } else {  // still pressed after one poll cycle
             key3_state = 2; 
        }
    }
#endif

#if (defined KEY4_DOWN_ACTION || defined KEY4_UP_ACTION)
    if (PIN_KEY4 & KEY4) {
        if (key4_state == 2) {         // key4 released
            key4_state = 3;
            #if (defined KEY4_UP_ACTION)
            KEY4_UP_ACTION;
            #endif
        } else {                       // still released after one poll cycle
            key4_state = 0;
        }
    } else {
         if (key4_state == 0) {
            key4_state = 1; 
            #if (defined KEY4_DOWN_ACTION)
            KEY4_DOWN_ACTION;    
            #endif
        } else {  // still pressed after one poll cycle
             key4_state = 2; 
        }
    }
#endif

#if (defined _USE_AUTOSHUTDOWN_)
if (anykey) { stayAwake; }
#endif

}
