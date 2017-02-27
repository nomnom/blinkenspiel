/*
 *  blinkenspiel / keyboard.h 
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

void pollKeys(void);


#ifndef __KEYBOARD_H__
# define __KEYBOARD_H__
#endif


# define PIN_KEY1   PIND
# define KEY1       (1<<PIND0)

# define PIN_KEY2   PIND
# define KEY2       (1<<PIND1)

# define PIN_KEY3   PIND
# define KEY3       (1<<PIND6)

# define PIN_KEY4   PINB
# define KEY4       (1<<PINB6)

# define B_INPUTS   0b01000000
# define D_INPUTS   0b01000011

 



// init inputs for the keys                               // pullups on (active low buttons) :
# define initKeys { DDRB &= ~B_INPUTS; DDRD &= ~D_INPUTS;   PORTB |= B_INPUTS;  PORTD |= D_INPUTS; }

// anykey: use e.g.:        while (!anykey) {}
#define anykey (key1_state || key2_state || key3_state || key4_state)

