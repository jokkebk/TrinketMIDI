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

#include "usbconfig.h"
#include "usbdrv/usbdrv.h"

#include "usbmidi.h"

uchar midiRing[MIDI_BUF_LEN], rr=0, rw=0;

void usbmidiNoteOn(uchar note, uchar velo) {
    midiRing[rw++] = 0x09;
    midiRing[rw++] = 0x90;
    midiRing[rw++] = note;
    midiRing[rw++] = velo;
    if(rw >= MIDI_BUF_LEN) rw = 0;
}

void usbmidiNoteOff(uchar note, uchar velo) {
    midiRing[rw++] = 0x08;
    midiRing[rw++] = 0x80;
    midiRing[rw++] = note;
    midiRing[rw++] = velo;
    if(rw >= MIDI_BUF_LEN) rw = 0;
}

void usbmidiControlChange(uchar num, uchar data) {
    midiRing[rw++] = 0x0B;
    midiRing[rw++] = 0xB0;
    midiRing[rw++] = num;
    midiRing[rw++] = data;
    if(rw >= MIDI_BUF_LEN) rw = 0;
}

void usbmidiSend() {
    if(rr == rw || !usbInterruptIsReady()) return; // no data or not ready
    uchar len = (rr+8 <= rw) ? 8 : 4; // send two if possible
    usbSetInterrupt(&midiRing[rr], len);
    rr += len;
    if(rr >= MIDI_BUF_LEN) rr = 0;
}
