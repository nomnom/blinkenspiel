/*  
 * blinkenspiel / display.h
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

void doScanStep(void);
void shutdownDisplay(void);
void clear(uint8_t val);

#ifndef __DISPLAY_H__
# define __DISPLAY_H__
#endif

#ifndef __KEYBOARD_H__
# error "keyboard.h missing "
#endif

// common anode is default
# if !defined ( _TA07_ ) && !defined ( _TC07_ )
#  define _TA07_ 
# endif


# if defined ( _FLIPPED_ )
#  if defined ( _ROTATED_ )    

#   define PORT_ROW1 PORTD
#   define PORT_ROW2 PORTD
#   define PORT_ROW3 PORTB
#   define PORT_ROW4 PORTB
#   define PORT_ROW5 PORTB
#   define ROW1      (1 << 5)
#   define ROW2      (1 << 3)
#   define ROW3      (1 << 2)
#   define ROW4      (1 << 5)
#   define ROW5      (1 << 4)
#   define PORT_COL1 PORTB
#   define PORT_COL2 PORTB
#   define PORT_COL3 PORTD
#   define PORT_COL4 PORTB
#   define PORT_COL5 PORTD
#   define PORT_COL6 PORTA
#   define PORT_COL7 PORTA
#   define COL1      (1 << 0)
#   define COL2      (1 << 1)
#   define COL3      (1 << 4)
#   define COL4      (1 << 3)
#   define COL5      (1 << 2)
#   define COL6      (1 << 0)
#   define COL7      (1 << 1)
#   define A_OUTPUTS 0b00000011 // XX XX XX XX XX XX R7 R6
#   define B_OUTPUTS 0b00111111 // XX XX C4 C5 R4 C3 R2 R1
#   define D_OUTPUTS 0b00111100 // XX XX C1 R3 C2 R5 XX XX
#   define A_INIT    0b00000011
#   define B_INIT    0b00001011 // R->HIGH, C->LOW
#   define D_INIT    0b00010100

#  else

#   define PORT_ROW1 PORTB
#   define PORT_ROW2 PORTB
#   define PORT_ROW3 PORTD
#   define PORT_ROW4 PORTD
#   define PORT_ROW5 PORTD
#   define ROW1      (1 << 5)
#   define ROW2      (1 << 3)
#   define ROW3      (1 << 2)
#   define ROW4      (1 << 5)
#   define ROW5      (1 << 4)
#   define PORT_COL1 PORTA
#   define PORT_COL2 PORTA
#   define PORT_COL3 PORTB
#   define PORT_COL4 PORTD
#   define PORT_COL5 PORTB
#   define PORT_COL6 PORTB
#   define PORT_COL7 PORTB
#   define COL1      (1 << 1)
#   define COL2      (1 << 0)
#   define COL3      (1 << 4)
#   define COL4      (1 << 3)
#   define COL5      (1 << 2)
#   define COL6      (1 << 1)
#   define COL7      (1 << 0)
#   define A_OUTPUTS 0b00000011 // XX XX XX XX XX XX R1 R2  
#   define B_OUTPUTS 0b00111111 // XX XX C1 R3 C2 R5 R6 R7
#   define D_OUTPUTS 0b00111100 // XX XX C4 C5 R4 C3 XX XX
#   define A_INIT    0b00000011
#   define B_INIT    0b00010111 // R->HIGH, C->LOW
#   define D_INIT    0b00001000

#  endif

# else
#  if defined ( _ROTATED_ ) 
   
#   define PORT_ROW1 PORTB
#   define PORT_ROW2 PORTB
#   define PORT_ROW3 PORTD
#   define PORT_ROW4 PORTA
#   define PORT_ROW5 PORTA
#   define ROW1      (1 << 0)
#   define ROW2      (1 << 2)
#   define ROW3      (1 << 3)
#   define ROW4      (1 << 1)
#   define ROW5      (1 << 0)
#   define PORT_COL1 PORTD
#   define PORT_COL2 PORTD
#   define PORT_COL3 PORTB
#   define PORT_COL4 PORTD
#   define PORT_COL5 PORTB
#   define PORT_COL6 PORTB
#   define PORT_COL7 PORTB
#   define COL1      (1 << 5)
#   define COL2      (1 << 4)
#   define COL3      (1 << 1)
#   define COL4      (1 << 2)
#   define COL5      (1 << 3)
#   define COL6      (1 << 4)
#   define COL7      (1 << 5)
#   define A_OUTPUTS 0b00000011 // XX XX XX XX XX XX C4 C5  
#   define B_OUTPUTS 0b00111111 // XX XX R7 R6 R5 C2 R3 C1
#   define D_OUTPUTS 0b00111100 // XX XX R1 R2 C3 R4 XX XX
#   define A_INIT    0b00000000 
#   define B_INIT    0b00111010 // R->HIGH, C->LOW
#   define D_INIT    0b00110100

#  else     // DEFAULT

#   define PORT_ROW1 PORTA
#   define PORT_ROW2 PORTD
#   define PORT_ROW3 PORTB
#   define PORT_ROW4 PORTB
#   define PORT_ROW5 PORTB
#   define ROW1      (1 << 1)
#   define ROW2      (1 << 2)
#   define ROW3      (1 << 3)
#   define ROW4      (1 << 0)
#   define ROW5      (1 << 1)
#   define PORT_COL1 PORTB
#   define PORT_COL2 PORTB
#   define PORT_COL3 PORTA
#   define PORT_COL4 PORTB
#   define PORT_COL5 PORTD
#   define PORT_COL6 PORTD
#   define PORT_COL7 PORTD
#   define COL1      (1 << 5)
#   define COL2      (1 << 4)
#   define COL3      (1 << 0)
#   define COL4      (1 << 2)
#   define COL5      (1 << 3)
#   define COL6      (1 << 4)
#   define COL7      (1 << 5)
#   define A_OUTPUTS 0b00000011 // XX XX XX XX XX XX C1 R3  
#   define B_OUTPUTS 0b00111111 // XX XX R1 R2 C3 R4 C5 C4
#   define D_OUTPUTS 0b00111100 // XX XX R7 R6 R5 C2 XX XX
#   define A_INIT    0b00000001
#   define B_INIT    0b00110100 // R->HIGH, C->LOW
#   define D_INIT    0b00111000

#  endif
# endif

// define outputs 
# define initDisplay { DDRA |= A_OUTPUTS; DDRB |= B_OUTPUTS; DDRD |= D_OUTPUTS; }

