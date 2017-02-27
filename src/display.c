/*  
 * blinkenspiel / display.c
 * 
 * On/Off controller for a 5x7 LED matrix
 * 
 * Defines how the 5x7 led dot matrix display is connected
 *  - 4 possible orientations (from top/from bottom + 0/180deg rotated)
 *  - common anode (TA07) or common cathode (TC07) displays are supported
 *  - (display pins , attiny pins)  =  (4-9 , 12-17)
 * 
 * 
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


#ifndef __DISPLAY_H__
# include "display.h"
#endif
 
static volatile uint8_t buff[5];        // screen memory, 5 rows / 7 cols (8th bit unused)      
static volatile uint8_t row = 0;        // active row number

#if defined (_USE_INVERT_)
static volatile uint8_t inverted = 0;   // if pixels should be inverted
#endif

static volatile uint8_t display_on = 1; // display on/off

// resets ports so no LED is lighting up (takes key inputs into account)
void shutdownDisplay(void) {
#if defined (_TA07_)  // common anode
# if defined (__KEYBOARD_H__)
    PORTA = A_INIT;
    PORTB = B_INIT | B_INPUTS;
    PORTD = D_INIT | D_INPUTS;
# else
    PORTA = A_INIT;
    PORTB = B_INIT;
    PORTD = D_INIT;
# endif
#elif defined (_TC07_)  // common cathode
# if defined (__KEYBOARD_H__)
    PORTA = A_OUTPUTS^A_INIT;
    PORTB = (B_OUTPUTS^B_INIT) | B_INPUTS;
    PORTD = (D_OUTPUTS^D_INIT) | D_INPUTS;
# else
    PORTA = A_OUTPUTS^A_INIT;
    PORTB = B_OUTPUTS^B_INIT;
    PORTD = D_OUTPUTS^D_INIT;
# endif
#endif
}

// set all rows to same value
void clear(uint8_t val) {
	buff[0]=val;
	buff[1]=val;
	buff[2]=val;
	buff[3]=val;
    buff[4]=val;
}

/*
 * row scanning (5 rows a 7 bit)
 * scans one row per call
 */
void doScanStep(void) {
    
    uint8_t val;
 
    shutdownDisplay();

    if (!display_on) return;

    // next row
    if (row++ > 4) { row = 0; }
    val = buff[row];


#if defined (_USE_INVERT_)
    if (inverted) { val = ~val; }
#endif

    // set row values according to active row
#if defined (_TA07_)  // common anode
    if ( val & 0x40 ) { PORT_COL1 &= ~COL1; }
    if ( val & 0x20 ) { PORT_COL2 &= ~COL2; }
    if ( val & 0x10 ) { PORT_COL3 &= ~COL3; }
    if ( val & 0x08 ) { PORT_COL4 &= ~COL4; }
    if ( val & 0x04 ) { PORT_COL5 &= ~COL5; }
    if ( val & 0x02 ) { PORT_COL6 &= ~COL6; }
    if ( val & 0x01 ) { PORT_COL7 &= ~COL7; }
    
    switch (row) {
     case 4 : PORT_ROW1 |= ROW1; break; 
     case 3 : PORT_ROW2 |= ROW2; break; 
     case 2 : PORT_ROW3 |= ROW3; break; 
     case 1 : PORT_ROW4 |= ROW4; break; 
     case 0 : PORT_ROW5 |= ROW5; break; 
     default: break;
    }

#elif defined (_TC07_)  // common cathode
    if ( val & 0x40 ) { PORT_COL1 |= COL1; }
    if ( val & 0x20 ) { PORT_COL2 |= COL2; }
    if ( val & 0x10 ) { PORT_COL3 |= COL3; }
    if ( val & 0x08 ) { PORT_COL4 |= COL4; } 
    if ( val & 0x04 ) { PORT_COL5 |= COL5; }
    if ( val & 0x02 ) { PORT_COL6 |= COL6; }
    if ( val & 0x01 ) { PORT_COL7 |= COL7; }
    
    switch (row) {
     case 4 : PORT_ROW1 &= ~ROW1; }
     case 3 : PORT_ROW2 &= ~ROW2; }
     case 2 : PORT_ROW3 &= ~ROW3; }
     case 1 : PORT_ROW4 &= ~ROW4; }
     case 0 : PORT_ROW5 &= ~ROW5; }
     default: break;
    }
#endif

}
