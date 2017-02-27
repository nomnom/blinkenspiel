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
#define framewait 300


// PROTOTYPES
void delay_ms(uint16_t delay);

// COUNTERS / TIMER DIVIDERS
static volatile uint16_t ctr_delay = 0;  // 16 bit counter for delay function
static volatile uint8_t ctr_20 = 0;      // 10 hz divider


// GLOBAL VARIABLES
enum STATES {INIT, GAME, END, WIN };
volatile uint8_t state = INIT;

// DATA
const uint8_t title[] PROGMEM = { /* put data here */  };


// BUTTON PROCESSING

// define key down actions here
#define KEY1_DOWN_ACTION   // up
#define KEY2_DOWN_ACTION   // down
#define KEY3_DOWN_ACTION   // left
#define KEY4_DOWN_ACTION   // right

// define key up actions here
#define KEY1_UP_ACTION   // up
#define KEY2_UP_ACTION   // down
#define KEY3_UP_ACTION   // left
#define KEY4_UP_ACTION   // right
#include "keyboard.h"

// DISPLAY
#define _TA07_          // common anode
#define _FLIPPED_       // display from top, chip from bottom
#define _USE_INVERT_    // use invert (enable with invert=1);
#define _USE_SCRAMBLE_  // enable scramble (refresh freq. is halfed)
#include "display.h"

// POWER CONTROL
#define _USE_AUTOSHUTDOWN_  // automatically enters sleep state if no key is pressed in a certain interval
#define WAKEUP_ACTION       // action to be executed when waking up; specify below
#include "power.h"


// fill in macros here

#undef KEY1_DOWN_ACTION
#define KEY1_DOWN_ACTION {  }   //up
#undef KEY2_DOWN_ACTION
#define KEY2_DOWN_ACTION {  }   //down 
#undef KEY3_DOWN_ACTION
#define KEY3_DOWN_ACTION {  }   //left 
#undef KEY4_DOWN_ACTION
#define KEY4_DOWN_ACTION {  }   //right

#undef KEY1_UP_ACTION
#define KEY1_UP_ACTION {  }   //up
#undef KEY2_UP_ACTION
#define KEY2_UP_ACTION {  }   //down 
#undef KEY3_UP_ACTION
#define KEY3_UP_ACTION {  }   //left 
#undef KEY4_UP_ACTION
#define KEY4_UP_ACTION {  }   //right

#undef WAKEUP_ACTION
#define WAKEUP_ACTION { /* re init game ?*/ }

#include "keyboard.c"
#include "display.c"
#include "power.c"
#include "highscore.c"      // highscore module


/*
 * Interrupt Service Routine TIMER 0
 * runs with (8MHz / 250 / 8) = 4kHz  (0.25ms interval)
 */
ISR(TIMER0_OVF_vect) {
    TCNT0 = 0xFF-250;   // reset timer 0 counter

  
    // display scanning
    doScanStep();
    
    // for autosleep
    pollPower;
    
    ctr_delay++;
    
    //20 hz
    ctr_20++;
    if (ctr_20 > 200) { 
        ctr_20=0; 
        
        // poll key states
        pollKeys();
        
        /* TODO other stuff with 20 Hz here */
    }
    
     /* TODO other timer divideres for periodic events*/
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


// initialize game state to begin a game
void init (void) {
    
    /* TODO reset game state here */
    
    state = GAME;   // game starts now
}


// update backbuffer here (or update only selected pixels if u like)
void draw (void) {
   
   /* set pixel on backbuffer:    buff[y] |=  (1 << x);
    * reset pixel on backbuffer:  buff[y] &= ~(1 << x);
    */
   
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
    
     // main loop: this is where stuff happens between ISR0 Interrupts
    while (1) {
        
        init();
        draw();

        // loop of one game instance
        while (state == GAME) {
            
           // TODO do one step of game logic
           /* STEP */
           draw();      // update screen
           delay_ms(framewait);
        }
        
        if (state == END) {
            // TODO game lost
        } else {
            // TODO game won
        }
        
    }
 
}
