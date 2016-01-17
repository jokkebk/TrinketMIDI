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
#include <avr/interrupt.h>
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
    cli();
	//Set Baud rate
	UBRR0H = (unsigned char)(ubrr_value >> 8);  
	UBRR0L = (unsigned char)(ubrr_value & 255);
    // Make sure double speed is off! Maybe Trinket bootloader sets this?
    UCSR0A &= ~(1 << U2X0);
	// Frame Format: asynchronous 8-N-1
	UCSR0C = (0 << USBS0) | (3 << UCSZ00);
	//Enable the receiver
#ifdef DEBUG
	UCSR0B = _BV(RXEN0) | _BV(TXEN0); // echo RX on TX
#else
	UCSR0B = _BV(RXEN0);
#endif
    sei();
}

int main(void) {
    enum { NOTEON, NOTEOFF, CONTROL, OTHER } state = OTHER;
    uchar msg, data[2], dp=0;

    // Run MIDI at slightly higher clock rate, because ATmega
    // seemed to drop bytes with exact 31250 baud rate
	USARTInit(F_CPU/16/32150); // Exact: 12e6/16/31250 = 24

    wdt_enable(WDTO_1S);

    trinketUsbBegin();

#ifdef DEBUG
    DDRB |= _BV(PB5); // LED as output
#endif

    while(1) {
        wdt_reset();
        usbPoll();
        usbmidiSend();
        if(!(UCSR0A & (1<<RXC0))) continue; // no data waiting

        // There are several limitations to the code below:
        // 1. System real-time messages ignored
        // 2. Only note on, off and control change handled
        // 3. Channel number ignored
        msg = UDR0;
#ifdef DEBUG
        UDR0 = msg; // echo on TX for debugging
        PINB |= _BV(PB5); // Toggle LED
#endif

        if(msg & 0x80) { // MIDI status byte
            switch(msg >> 4) { // Remove channel
            case 0xB:
                state = CONTROL;
                dp = 0;
                break;
            case 0x8:
                state = NOTEOFF;
                dp = 0;
                break;
            case 0x9:
                state = NOTEON;
                dp = 0;
                break;
            default:
                // Just forget everything else for now, including
                // 0xF8 (time) and 0xFE (active sensing)
                //state = OTHER;
                break;
            }
        } else { // MIDI data byte
            data[dp++] = msg;

            if(dp >= 2) {
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
                    default: // ignore
                        break;
                }
                dp = 0; // ready for running status
            }
        }
    }

    return 0;
}
