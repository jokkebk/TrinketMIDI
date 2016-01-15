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

#ifdef PRO_TRINKET
#warning "This example does not actually work, UART drops bytes with V-USB"
#else
#error "MIDI adapter demo only works with hardware USART in Pro Trinket!"
#endif

// Minimal ring buffer implementation
typedef struct { uchar rr, rw, ri[32]; } Ring;
#define RAVAIL(r) (r.rr!=r.rw)
#define RPUT(r,v) { r.ri[r.rw++] = (v); if(r.rw >= sizeof(r.ri)) r.rw = 0; }
#define RGET(r,v) { v = r.ri[r.rr++]; if(r.rr >= sizeof(r.ri)) r.rr = 0; }
volatile Ring serial = {0, 0}; // initialize read, write pointers

void USARTInit(unsigned int ubrr_value) {
    cli();
	//Set Baud rate
	UBRR0H = (unsigned char)(ubrr_value >> 8);  
	UBRR0L = (unsigned char)(ubrr_value & 255);
    // Make sure double speed is off! Maybe Trinket bootloader sets this?
    UCSR0A &= ~(1 << U2X0);
	//Enable the receiver
	UCSR0B = (1 << RXEN0) | _BV(TXEN0) | (1 << RXCIE0); // enable RX complete interrupt
	// Frame Format: asynchronous 8-N-1
	UCSR0C = (0 << USBS0) | (3 << UCSZ00);
    sei();
}

ISR(USART_RX_vect) { // USART receive complete interrupt
    //if(UCSR0A & _BV(DOR0))
        PINB |= _BV(PB5); // toggle LED
    uchar data = UDR0;
    if(data == 0xF8 || data == 0xFE) return; // skip timestamp
    RPUT(serial, data); // clear RX FIFO
}

int main(void) {
    enum { NOTEON, NOTEOFF, CONTROL, OTHER } state = OTHER;
    uchar msg, data[2], dp=0;

	USARTInit(F_CPU/16/31250); // Exact: 12e6/16/31250 = 24

    wdt_enable(WDTO_1S);

    trinketUsbBegin();

    DDRB |= _BV(PB5); // LED as output

    while(1) {
        while(RAVAIL(serial)) { // process received MIDI data
            RGET(serial, msg); // next byte from ring buffer
            /*if(msg & 0x80)
                usbmidiNoteOn(msg & 0x7F, 0x10);
            else
                usbmidiNoteOff(msg, 0x00);
            continue;
            */

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
                        state = OTHER;
                        break;
                    }
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
                        case OTHER: // ignore
                            break;
                    }
                    dp = 0; // ready for running status
                }
            }
        }

        wdt_reset();
        usbPoll();
        usbmidiSend();
    }

    return 0;
}
