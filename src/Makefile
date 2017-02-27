# 
# blinkenspiel / Makefile
#
# builds and flashes blinkenspiel games onto the tiny2313 controller
#
#
#  Copyright (C) 2011 Manuel Jerger <nom@nomnom.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#


# change this to build and flash other programs:
PROGRAM = snake


DEVICE     = attiny2313
F_CPU      = 8000000

#PROGRAMMER = -c avrispmkII -P usb
PROGRAMMER = -c usbasp -P usb


OBJECTS    = $(PROGRAM).o 

# Fuses: internal oscillator 8 MHz, no prescale, BOD 1.8V, EEsave, spi enabled
FUSES      = -U lfuse:w:0xE4:m -U hfuse:w:0x9D:m 

# Tune the lines below only if you know what you are doing:

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
OBJDUMP = avr-objdump
COMPILE = avr-gcc -Wall -Wall -Wstrict-prototypes -Os  -mcall-prologues -fno-inline-small-functions -fno-reorder-blocks -fno-reorder-blocks-and-partition -fno-reorder-functions -fno-toplevel-reorder -fno-move-loop-invariants -DF_CPU=$(F_CPU) -mmcu=$(DEVICE)




# symbolic targets:
all:	$(PROGRAM).hex $(PROGRAM).lss    
	avr-size $(PROGRAM).hex


.c.o:
	$(COMPILE) -c $< -o $@


.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:$(PROGRAM).hex:i
	
eeprom: 
	$(AVRDUDE) -U eeprom:w:$(EEPROM).hex
	
reset_highscore:
	$(AVRDUDE) -U eeprom:w:0x00:m

fuse:
	$(AVRDUDE) $(FUSES)

clean:
	rm -f $(PROGRAM).hex $(PROGRAM).elf $(PROGRAM).lss $(PROGRAM).o

# file targets:
$(PROGRAM).elf: $(OBJECTS)
	$(COMPILE) -o $(PROGRAM).elf $(OBJECTS)

$(PROGRAM).hex: $(PROGRAM).elf
	rm -f $(PROGRAM).hex
	avr-objcopy -j .text -j .data -O ihex $(PROGRAM).elf $(PROGRAM).hex
	

# Targets for code debugging and analysis:
disasm:	$(PROGRAM).elf
	avr-objdump -d $(PROGRAM).elf

%.lss: %.elf
	@echo
	$(OBJDUMP) -h -S $< > $@

cpp:
	$(COMPILE) -E $(PROGRAM).c
