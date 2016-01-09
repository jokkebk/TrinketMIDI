/* This is a simplified version of the V-USB MIDI implementation by
 * Martin Homuth-Rosemann. Basically only MIDI descriptors from
 * USB MIDI specification and the stripped-down implementations of V-USB
 * functions remain (I removed the sendEmptyFrame -code because it did
 * not do anything). See License_VUSBMIDI.txt for copyright details.
 *
 * Project: V-USB MIDI device on Low-Speed USB
 * Author: Martin Homuth-Rosemann
 * Creation Date: 2008-03-11
 * Copyright: (c) 2008 by Martin Homuth-Rosemann.
 * License: GPL.
 *
 */

#ifndef __VUSBMIDI_H
#define __VUSBMIDI_H

#include <avr/pgmspace.h>

#ifdef __cplusplus
extern "C" {
#endif

/* USB device descriptor */
extern PROGMEM const char deviceDescrMIDI[18];

/* USB configuration descriptor */
extern PROGMEM const char configDescrMIDI[101];

/* VERY barebones V-USB implementation of these functions */
uchar usbFunctionDescriptor(usbRequest_t * rq);
uchar usbFunctionSetup(uchar data[8]);
uchar usbFunctionRead(uchar * data, uchar len);
uchar usbFunctionWrite(uchar * data, uchar len);
void usbFunctionWriteOut(uchar * data, uchar len);

#ifdef __cplusplus
}
#endif

#endif // __VUSBMIDI_H
