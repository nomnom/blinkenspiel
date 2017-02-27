/*
 *  blinkenspiel / game.c    
 * 
 *  A basic skeleton for writing blinkenspiel games
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

// CONSTANTS
#define framewait 100


// PROTOTYPES
void delay_ms(uint16_t delay);
void drawCreate(void);
uint8_t getl (uint8_t b[], int8_t x, int8_t y);


// COUNTERS / TIMER DIVIDERS
static volatile uint16_t ctr_delay = 0;  // 16 bit counter for delay function
static volatile uint8_t ctr_20 = 0;      // 20 hz divider



// GLOBAL VARIABLES
enum STATES {INIT, CREATE, SIMULATE, END };
volatile uint8_t state = INIT;

static volatile uint8_t  curx, cury;         // current cursor position
static volatile uint8_t  univ[8];


#define maxX 7
#define maxY 7




// BUTTON PROCESSING

// define key down actions here
#define KEY1_DOWN_ACTION   // up
#define KEY2_DOWN_ACTION   // down
#define KEY3_DOWN_ACTION   // left
#define KEY4_DOWN_ACTION   // right

#include "keyboard.h"

// DISPLAY
#define _TA07_          // common anode
#define _FLIPPED_       // display from top, chip from bottom
//#define _USE_INVERT_    // use invert (enable with invert=1);
//#define _USE_SCRAMBLE_  // enable scramble (refresh freq. is halfed)
#include "display3.h"

// POWER CONTROL
#define _USE_AUTOSHUTDOWN_  // automatically enters sleep state if no key is pressed in a certain interval
//#define WAKEUP_ACTION       // action to be executed when waking up; specify below
#include "power.h"



static volatile uint8_t keystate=0;
#define MAX_DELAY_SET 5
#define MAX_DELAY_START 30

static volatile uint8_t ctr_delay_set=0;
static volatile uint8_t ctr_delay_start=0;

/*
#undef KEY1_DOWN_ACTION
#define KEY1_DOWN_ACTION { } //up
#undef KEY2_DOWN_ACTION
#define KEY2_DOWN_ACTION { } //down
#undef KEY3_DOWN_ACTION
#define KEY3_DOWN_ACTION { } //left 
#undef KEY4_DOWN_ACTION
#define KEY4_DOWN_ACTION { } //right
*/

#undef KEY1_UP_ACTION
#define KEY1_UP_ACTION {if (!ctr_delay_set) {if (cury > 0) { cury--; } } }//up
#undef KEY2_UP_ACTION
#define KEY2_UP_ACTION {if (!ctr_delay_set) {if (cury < 4) { cury++; } } }//down 
#undef KEY3_UP_ACTION
#define KEY3_UP_ACTION {if (!ctr_delay_set) {if (curx < 6) { curx++; } } }//left 
#undef KEY4_UP_ACTION
#define KEY4_UP_ACTION {if (!ctr_delay_set) {if (curx > 0) { curx--; } } }//right



#undef WAKEUP_ACTION
#define WAKEUP_ACTION { /* init game ?*/ }

#include "keyboard.c"
#include "display3.c"
#include "power.c"
#include "highscore.c"      // highscore module


/*
 * Interrupt Service Routine TIMER 0
 * runs with (8MHz / 256 / 8) = 3906 Hz  (0.256ms interval)
 */
ISR(TIMER0_OVF_vect) {  

  
    doScanStep();
    
    // for autosleep
    pollPower;
    
    ctr_delay++;
    
    //20 hz
    ctr_20++;
    if (ctr_20> 195) { 
        ctr_20=0; 
        
        // poll key states
        pollKeys();
        
        // if Up+Down pressed
        if (key1_state && key2_state) {
            if (key3_state && key4_state) {
                
                // if all buttons pressed
                if (ctr_delay_start++ > MAX_DELAY_START) {
                    ctr_delay_start = 0;
                    state = SIMULATE;

                }
                
            } else {
                ctr_delay_start = 0;
                
                // up+down pressed
                if (ctr_delay_set++ > MAX_DELAY_SET) {
                    ctr_delay_set = 0;
                    // toggle pixel in universe
                    univ[cury] ^= (1 << curx);
                }
                
            }
        }
        if (!anykey) {
            ctr_delay_set = 0; 
            ctr_delay_start = 0;
        }
    }
}


/*
 * delay_ms
 * Uses the counter that is incremented by the ISR.
 */
void delay_ms(uint16_t delay) {
    uint16_t t = delay << 2;   // * 3.906
    ctr_delay = 0;
    while (ctr_delay < t) {}
}


// wraps coordinates around edges
uint8_t getl (uint8_t b[], int8_t xp, int8_t yp) {
    if (xp<0) xp=maxX;
    if (yp<0) yp=maxY;
    if (xp>maxX) xp=0;
    if (yp>maxY) yp=0;
    return (b[yp] & (1<<xp));
}




// the main game logic may be implemented using delay_ms() function
// -> interrupted only by display scanning & key polling in ISR0!
int main(void) {

    // setup timers
    TCCR0B |= (1 << CS01);      // timer 0 prescaler 8
    TCCR1B |= (1 << CS00);      // timer 1 prescaler 1
    TCCR1A |= (1 << WGM10);     // timer 1 8 bit pwm mode
    
    // misc
    ACSR |= (1 << ACD);     // Analog Comparator Disabled

    initDisplay;
    initKeys;
    initPower;
    
    TIMSK |= (1 << TOIE0);  // enable timer 0 interrupt
    
    shutdownDisplay();      // display off
    
    sei();                  // enable interrupts = enable ISR
    
    // clear screen
    clear(0x00);
    
    delay_ms(100);
    
    uint8_t i;
    
    
    uint16_t lifecycles;
    int8_t x, y;
    uint8_t n=0; 
    uint8_t tmpuniv[8];
    
     // main loop: this is where stuff happens between ISR0 Interrupts
    while (1) {

        // create universe
        state = CREATE;
        curx = 3; cury = 2;
        for (i=0; i<5; i++) {
            univ[i] = 0;//TCNT1;
            tmpuniv[i] = 0;
        }

        // wait for start signal
        while (state == CREATE) {
            for (i=0; i<5; i++) {
                buffb[i] = univ[i];
                buff[i] = 0;
            }
            buff[cury] |= (1 << curx);
            delay_ms(framewait);
        }
        
        for (i=0; i<8; i++) { tmpuniv[i] = univ[i]; }
        
        clear(0);
        
        uint8_t alive = 1;
        while (state == SIMULATE && alive) { 
        
            alive = 0;
            for (x=0; x<maxX; x++) {
                for (y=0; y<maxY; y++) {
                
                    n=0;
                    if (getl(tmpuniv, x-1, y-1)) n++;
                    if (getl(tmpuniv, x, y-1)) n++;
                    if (getl(tmpuniv, x+1, y-1)) n++;
                    if (getl(tmpuniv, x-1, y)) n++;
                    if (getl(tmpuniv, x+1, y)) n++;
                    if (getl(tmpuniv, x-1, y+1)) n++;
                    if (getl(tmpuniv, x, y+1)) n++;
                    if (getl(tmpuniv, x+1, y+1)) n++;
                    
                    if (tmpuniv[y] & (1 << x)) {
                        if ((n < 2) || (n > 3)) {   // check 1/2, do nothing otherwise (3.)
                            univ[y] &= ~(1<<x);     // DIE!
                            alive = 1;
                        }
                    } else {
                        if (n == 3) {               // check 4.
                            univ[y] |= (1<<x);      // become alive
                            alive = 1;
                        }
                    }
                }
            }
                

            // copy to display / tmp buffer
            for (i=0; i<5; i++) { buff[i] = univ[i]; }
            
            for (i=0; i<8; i++) { tmpuniv[i] = univ[i]; }
            
            // check if alive TODO: multi cycle detection
            
            lifecycles++;
        
            delay_ms(framewait);
        }
    }
 
}
