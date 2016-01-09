/*
TrinketMIDI USB MIDI implementation for Adafruit Trinket.
Based on Adafrtuit Industries TrinketKeyboard example and
Martin Homuth-Rosemann's V-USB MIDI device code.

Copyright (c) 2016 Joonas Pihlajamaa (GPL v3)
Copyright (c) 2013 Adafruit Industries (LGPL v3)
Copyright (c) 2008 by Martin Homuth-Rosemann (GPL v2)
All rights reserved.

TrinketMIDI is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

TrinketMIDI is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU (Lesser) General Public
License along with this project. If not, see
<http://www.gnu.org/licenses/>.
*/

#include <avr/power.h>
#include <util/delay.h>

#include "usbconfig.h"
#include "usbdrv/usbdrv.h"

#include "usbmidi.h"
#include "trinketusb.h"

// Very minimal ring buffer implementation
uchar ring[8], rr=0, rw=0;
#define REMPTY() (rr==rw)
#define RPUT(v) { ring[rw] = (v); rw = rw ? rw-1 : sizeof(ring)-1; }
#define RGET(v) { v = ring[rr]; rr = rr ? rr-1 : sizeof(ring)-1; }

int main(void) {
	uchar midiMsg[8], pos, keydown=0, key;
    unsigned debounce=0;

	//wdt_enable(WDTO_1S);
	usbBegin();

	PORTB |= _BV(PB0); // Pullup on button

    while(1) {
        //wdt_reset();
        usbPoll();

        if(debounce) debounce--;
        else if(!(PINB & _BV(PB0)) != keydown) { // button state change
            keydown = !keydown; // keydown to reflect current state
            RPUT(keydown); // add to ring buffer
            debounce = 1000;
        }

        if (usbInterruptIsReady()) { // 4. USB MIDI Event Packets
            for(pos=0; pos<8 && !REMPTY(); pos+=4) {
                RGET(key);
                if (key) { // Key press
                    midiMsg[pos+0] = 0x09;
                    midiMsg[pos+1] = 0x90;
                    midiMsg[pos+2] = 0x40; // key
                    midiMsg[pos+3] = 0x40; // max velocity 7f
                } else { // Key release
                    midiMsg[pos+0] = 0x08;
                    midiMsg[pos+1] = 0x80;
                    midiMsg[pos+2] = 0x40; // key
                    midiMsg[pos+3] = 0x00; // zero velocity
                }
            }

            if(pos) usbSetInterrupt(midiMsg, pos);
        }
    }

	return 0;
}
