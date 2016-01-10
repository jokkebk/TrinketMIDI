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

#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usbconfig.h"
#include "usbdrv/usbdrv.h"

void trinketUsbBegin() {
	cli();

	// run at full speed, because Trinket defaults to 8MHz for low voltage compatibility reasons
	clock_prescale_set(clock_div_1);

	// fake a disconnect to force the computer to re-enumerate
	PORTB &= ~(_BV(USB_CFG_DMINUS_BIT) | _BV(USB_CFG_DPLUS_BIT));
	usbDeviceDisconnect();
	_delay_ms(250);
	usbDeviceConnect();

	// start the USB driver
	usbInit();
	sei();
}

#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny25__)
// section copied from EasyLogger and adapted by Adafruit, comments removed by Joonas
void calibrateOscillator(void) {
    uchar       step = 128;
    uchar       trialValue = 0, optimumValue;
    int         x, optimumDev, targetValue = (unsigned)(1499 * (double)F_CPU / 10.5e6 + 0.5);

    do { /* do a binary search: */
        OSCCAL = trialValue + step;
        x = usbMeasureFrameLength();    /* proportional to current real frequency */
        if(x < targetValue)             /* frequency still too low */
            trialValue += step;
        step >>= 1;
    } while(step > 0);
    /* We have a precision of +/- 1 for optimum OSCCAL here */
    /* now do a neighborhood search for optimum value */
    optimumValue = trialValue;
    optimumDev = x; /* this is certainly far away from optimum */
    for(OSCCAL = trialValue - 1; OSCCAL <= trialValue + 1; OSCCAL++){
        x = usbMeasureFrameLength() - targetValue;
        if(x < 0) x = -x; // abs()
        if(x < optimumDev){
            optimumDev = x;
            optimumValue = OSCCAL;
        }
    }
    OSCCAL = optimumValue;
}
#endif


