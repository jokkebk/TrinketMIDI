/*
This is the part of the TrinketKeyboard code that sets Trinket clock
rate to 16 MHz and Adafruit-adapted oscillator calibration routine
to calibrate the (scaled) clock to 8.25 MHz. Calibration comments were
removed but otherwise no changes made. You can find the full
TrinketKeyboard code from:

https://github.com/adafruit/Adafruit-Trinket-USB

Copyright (c) 2013 Adafruit Industries
All rights reserved.

TrinketKeyboard is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

TrinketKeyboard is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with TrinketKeyboard. If not, see
<http://www.gnu.org/licenses/>.
*/

#ifndef __TRINKETUSB_H
#define __TRINKETUSB_H

#ifdef __cplusplus
extern "C" {
#endif

void usbBegin();

#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny25__)
void calibrateOscillator(void);
#endif

#ifdef __cplusplus
}
#endif

#endif // __TRINKETUSB_H
