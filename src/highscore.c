/*
 *  blinkenspiel / highscore.c    
 * 
 *  unified highscore module for all games
 *   - one byte max
 *   - stored in first eeprom byte
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


#include <avr/eeprom.h>

#ifndef __DISPLAY_H__
# error "display.h not included"
#endif

// check score, save and return true if higher than current
uint8_t checkScore (uint8_t score) {
    if (score > eeprom_read_byte(0)) {
        eeprom_write_byte(0, score);
        return 1;
    }
    return 0;
}

#define displayHighscore \
    uint8_t i; \
    uint8_t tens = 0; \
    while (length >= 10) { \
     tens += 5; \
     length -= 10; \
    } \
    for (i=0; i<5; i++) { \
        buff[i] = pgm_read_byte(numbers + i + length*5); \
        if (tens) { buff[i] |= pgm_read_byte(numbers + i + tens) << 4; } \
    }\
    


// numbers data
const uint8_t numbers [] PROGMEM = {
    //0
    0b00000010 ,
    0b00000101 ,
    0b00000111 ,
    0b00000101 ,
    0b00000010 ,
    //1
    0b00000010,
    0b00000110,
    0b00000010,
    0b00000010,
    0b00000111,
    //2
    0b00000110,
    0b00000001,
    0b00000010,
    0b00000100,
    0b00000111,
    //3
    0b00000110,
    0b00000001,
    0b00000010,
    0b00000001,
    0b00000110,
    //4
    0b00000100,
    0b00000100,
    0b00000111,
    0b00000010,
    0b00000010,
    //5
    0b00000111,
    0b00000100,
    0b00000110,
    0b00000001,
    0b00000110,
    //6
    0b00000011,
    0b00000100,
    0b00000111,
    0b00000101,
    0b00000111,
    //7
    0b00000111,
    0b00000001,
    0b00000011,
    0b00000001,
    0b00000001,
    //8
    0b00000111,
    0b00000101,
    0b00000111,
    0b00000101,
    0b00000111,
    //9
    0b00000111,
    0b00000101,
    0b00000111,
    0b00000001,
    0b00000111
};

