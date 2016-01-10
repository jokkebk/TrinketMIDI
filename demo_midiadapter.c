/*
This file is part of TrinketMIDI. See README.md for licencing details.

TrinketMIDI is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TrinketMIDI is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TrinketMIDI.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <avr/power.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "usbconfig.h"
#include "usbdrv/usbdrv.h"

#include "vusbmidi.h"   // V-USB MIDI definitions & routines
#include "trinketusb.h" // Trinket oscillator calibration & USB init
#include "usbmidi.h"    // usbmidi*() functions

#ifndef PRO_TRINKET
#error "MIDI adapter demo only works with hardware USART in Pro Trinket!"
#endif

void USARTInit(unsigned int ubrr_value) {
	//Set Baud rate
	UBRR0H = (unsigned char)(ubrr_value >> 8);  
	UBRR0L = (unsigned char)(ubrr_value & 255);
    // Make sure double speed is off! Maybe Trinket bootloader sets this?
    UCSR0A &= ~(1 << U2X0);
	//Enable the receiver
	UCSR0B = (1 << RXEN0); // | (1 << TXEN0);
	// Frame Format: asynchronous 8-N-1
	UCSR0C = (0 << USBS0) | (3 << UCSZ00);
}

int main(void) {
    enum { NOTEON, NOTEOFF, CONTROL, OTHER } state = OTHER;
    uchar msg, data[2], dp=0;

	USARTInit(F_CPU/16/31250); // Exact: 12e6/16/31250 = 24

    wdt_enable(WDTO_1S);

    trinketUsbBegin();

    PORTB |= _BV(PB2); // Pullup on button

    while(1) {
        wdt_reset();
        usbPoll();

        while(UCSR0A & (1<<RXC0)) { // process received data
            msg = UDR0; // makes room for more data in FIFO

            // There are several limitations to the code below:
            // 1. System real-time messages ignored
            // 2. Only note on, off and control change handled
            // 3. Channel number ignored
            
            if(msg & 0x80) { // MIDI status byte
                if(msg == 0xF8 || msg == 0xFE) {
                    // Ignore Timing Clock and Active Sensing
                } else {
                    switch(msg >> 4) { // Remove channel
                    case 0xB:
                        state = CONTROL;
                        break;
                    case 0x8:
                        state = NOTEOFF;
                        break;
                    case 0x9:
                        state = NOTEON;
                        break;
                    default:
                        state = OTHER;
                        break;
                    }
                    dp = 0;
                }
            } else { // MIDI data byte
                //msg = (msg-'0')<<4;
                data[dp++] = msg;

                if(dp == 2) {
                    switch(state) {
                        case NOTEON:
                            usbmidiNoteOn(data[0], data[1]);
                            break;
                        case NOTEOFF:
                            usbmidiNoteOff(data[0], data[1]);
                            break;
                        case CONTROL:
                            usbmidiControlChange(data[0], data[1]);
                            break;
                        case OTHER: // ignore
                            break;
                    }
                    dp = 0; // ready for running status
                }
            }
        }

        usbmidiSend();
    }

    return 0;
}
