# WinAVR cross-compiler toolchain is used here
CC = avr-gcc
OBJCOPY = avr-objcopy
DUDE = avrdude

# Update the lines below to match your configuration

# Below the F_CPU and part for Trinket
#CFLAGS = -Wall -Os -Iusbdrv -I. -mmcu=attiny85 -DF_CPU=16500000
#DUDEFLAGS = -p trinket -c usbtiny -v

# Below the F_CPU and part for 12 MHz Pro Trinket
CFLAGS = -Wall -O2 -Iusbdrv -I. -mmcu=atmega328p -DF_CPU=12000000 -DPRO_TRINKET
DUDEFLAGS = -p atmega328p -c usbtiny -v

# Object files for the firmware (usbdrv/oddebug.o not strictly needed I think)
OBJECTS = trinketusb.o vusbmidi.c usbmidi.c usbdrv/usbdrv.o usbdrv/usbdrvasm.o 
DEMOS = demo_key.hex demo_midiadapter.hex

# By default, build the firmware and command-line client, but do not flash
all: demo_key.hex

adapter: demo_midiadapter.hex

# By default, flash the key demo. See the rule below to flash other things.
flash: demo_key.flash

# "make demo_key.flash" will first make demo_key.hex and then flash it. Nice!
%.flash: %.hex
	$(DUDE) $(DUDEFLAGS) -U flash:w:$*.hex

# Housekeeping if you want it
clean:
	$(RM) *.o *.hex *.elf usbdrv/*.o

# From .elf file to .hex
%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

# All elf files will need $(OBJECTS)
%.elf: %.o $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@
	#$(CC) $(CFLAGS) $(OBJECTS) -o $@

# Without this dependence, .o files will not be recompiled if you change 
# the config! I spent a few hours debugging because of this...
$(OBJECTS) $(DEMOS): usbconfig.h

# From C source to .o object file
%.o: %.c	
	$(CC) $(CFLAGS) -c $< -o $@

# From assembler source to .o object file
%.o: %.S
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@
