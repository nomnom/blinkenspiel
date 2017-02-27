/*
 *  blinkenspiel / snake.c    
 * 
 *  shackspace snake v2011
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

// INCLUDES
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdint.h>



// PROTOTYPES
void setItem (void);
void delay_ms(uint16_t delay);


uint16_t framewait = 300;

enum STATES {INIT, GAME, END, WIN };
static volatile uint8_t state = INIT;

// counter for item flashing
#define ICTRMAX 7
#define ICTRDUTY 3
volatile uint16_t ictr = 0;

// SNAKE VARIABLES
// one value 0..35 for every position indicates the "oldness" of a pixel
volatile uint8_t snake[35];
volatile uint8_t length;
volatile uint8_t growing;


volatile int8_t posy;  // 0 .. 4
volatile int8_t posx;  // 0 .. 6

volatile int8_t diry;  // -1/0/+1 : delta x movement per step
volatile int8_t dirx;  // -1/0/+1 : delta y movement per step

volatile int8_t iposy;     // 0 .. 4
volatile int8_t iposx;     // 0 .. 6

volatile uint8_t key_set;



// key actions
// prevent multiple button inputs and dir changes in opposite direction (=crash)
#define KEY1_DOWN_ACTION
#define KEY2_DOWN_ACTION
#define KEY3_DOWN_ACTION
#define KEY4_DOWN_ACTION
#include "keyboard.h"

#define _TA07_
#define _FLIPPED_
#define _USE_INVERT_
#include "display.h"


#define _USE_AUTOSHUTDOWN_
#define WAKEUP_ACTION
#include "power.h"


#undef KEY1_DOWN_ACTION
#define KEY1_DOWN_ACTION { if (!key_set && diry != 1) { key_set=1; diry=-1; dirx= 0; } }   //up
#undef KEY2_DOWN_ACTION
#define KEY2_DOWN_ACTION { if (!key_set && diry !=-1) { key_set=1; diry= 1; dirx= 0; } }   //down 
#undef KEY3_DOWN_ACTION
#define KEY3_DOWN_ACTION { if (!key_set && dirx !=-1) { key_set=1; diry= 0; dirx= 1; } }   //left 
#undef KEY4_DOWN_ACTION
#define KEY4_DOWN_ACTION { if (!key_set && dirx != 1) { key_set=1; diry= 0; dirx=-1; } }   //right

#undef WAKEUP_ACTION
#define WAKEUP_ACTION { /* re init game ?*/ }

#include "keyboard.c"
#include "display.c"
#include "power.c"
#include "highscore.c"

static volatile uint16_t ctr_delay = 0;  // 16 bit counter for delay function
static volatile uint8_t ctr_20 = 0;      // 10 hz divider


/*
 * ISR TIMER0_OVF_vect
 * runs with 8MHz / 250 / 8 = 4000 Hz  (0.25ms)
 */
ISR(TIMER0_OVF_vect) {  
    TCNT0 = 0xFF-250;   // reset timer 0 counter

    // display scanning
    doScanStep();
    
    // for autosleep
    pollPower;
    
    // poll key states w/ 20 hz
    if (ctr_20++ > 200) { 
        ctr_20 = 0;
        if (!key_set) {
            pollKeys();
        }
        
        if (state == GAME) {
            
            // draw snake
            clear(0);
            uint8_t x,y;
            for (x=0; x<7; x++) {
                for (y=0; y<5; y++) {
                    if (snake[y*7+x] > 0) {
                        buff[y] |= (1 << x);
                    }
                }
            }
    
            // flash item
            if (ictr++ > ICTRMAX) { ictr = 0; }
            if (ictr > ICTRDUTY) {
                buff[iposy] |= (1 << iposx);
            } else {
                buff[iposy] &= ~(1 << iposx);
            }
        }
    }
    
    ctr_delay++;
}



 
/*
 * delay_ms
 * Uses the counter that is incremented by the ISR.
 */
void delay_ms(uint16_t delay) {
    uint16_t t = delay << 2;
    ctr_delay = 0;
    while (ctr_delay < t) {}
}



void setItem (void) {
    // postconditional loop until item is placed at a free spot
    int8_t nipos=0;
    
    do {
        do {
            nipos = TCNT1 & 63;       // lowest 6 bits -> 0 .. 63
        } while (nipos > 34);
        //if (nipos > 34) { nipos -= 35; }  // .. fitted to  0 .. 35
    } while ( snake[nipos] != 0 );          // check if position is free
    
    TCNT1 = 23;
    
    
    // calc xy position
    iposy = 0;
    while (nipos > 6) { nipos -= 7; iposy++; }
    iposx = nipos;

}



int main(void) {
    
    cli();
    
    TCCR0B |= (1 << CS01);      // timer 0 prescaler 8
    TCCR1B |= (1 << CS00);      // timer 1 prescaler 1
    TCCR1A |= (1 << WGM10);     // timer 1 8 bit pwm mode
    
    ACSR |= (1 << ACD);         // Analog Comparator Disabled

    initDisplay;
    initKeys;
    initPower;
    
    shutdownDisplay();          // display off
    
    // hold up+right at startup for hardcore mode
    //if ( (PIN_KEY1 & KEY1) && (PIN_KEY2 & KEY2) ){  framewait = 200; }
 
    TIMSK |= (1 << TOIE0);      // enable timer 0 interrupt
 
    // enable interrupts
    sei();
    
    // clear screen
    clear(0);
    
     // main loop: this is where stuff happens between ISR0 Interrupts
    while (1) {
        
        // start position / direction
        posx = 2;
        posy = 2;
        dirx = 1;
        diry = 0;
        uint8_t i;
        for (i=0;i<35;i++) { snake[i]=0; }   //reset field
        snake[16] = 2;
        snake[15] = 1;
        length = 2;
        growing = 0;
        key_set = 0;
        setItem();
        state = GAME;

        delay_ms(0xff);
        while (!anykey)  {};
        
        // gameloop
        do {
            delay_ms(framewait);
            
            // dostep:
            
            // "olden" snake pixels (if snake isn't growing right now)
            if (!growing) {
                uint8_t i;
                for (i=0; i<35; i++) {
                    if (snake[i] > 0) { snake[i]--; }
                }
            }
            growing = 0;
            
            // warp around, new position:
            int8_t nposx = (posx + dirx); 
            int8_t nposy = (posy + diry);
            if (nposx < 0) { nposx = 6; } else if (nposx > 6) { nposx = 0; }  // % 7
            if (nposy < 0) { nposy = 4; } else if (nposy > 4) { nposy = 0; }  // % 5

            // snake hit
            if (snake[nposy*7+nposx] != 0) {
                state = END;
                
            // boundary hit
            //} else if ( (nposx != posx + dirx) || (nposy != posy + diry) ) {
              //  state = END;
            
            // field is empty, go ahead snake!
            } else  {
                
                // place new sneakhead
                snake[nposy*7+nposx] = length;
                
                if (length < 35) {
                    //check if snake got an item, grow snake an replace item
                    if (nposy == iposy && nposx == iposx) {
                        length++;
                        growing = 1;
                        setItem();
                    }    
                } else {
                   state = WIN;
                }
                posx = nposx; posy = nposy;
            }
            
            key_set=0;
            
        } while (state == GAME);
        
        // check if highscore, display length if new score
        if (checkScore(length)) { displayHighscore; }

        //  flash display
        for (i=0; i<30; i++) {
            display_on ^= 1;
            delay_ms(100);
        }
        while (!anykey)  {};
    }
 
}


