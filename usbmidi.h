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

#ifndef __USBMIDI_H
#define __USBMIDI_H

#include <avr/pgmspace.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MIDI_BUF_LEN 128 // Must be a multiple of four bytes

/* Check is USB interrupt is ready and then send any bytes from buffer */
void usbmidiSend();

/* Add a note on event to midi buffer */
void usbmidiNoteOn(uchar note, uchar velo);

/* Add a note off event to midi buffer, usually velo=0 */
void usbmidiNoteOff(uchar note, uchar velo);

/* Add a control change event to midi buffer */
void usbmidiControlChange(uchar num, uchar data);

#ifdef __cplusplus
}
#endif

#endif /* __USBMIDI_H */
