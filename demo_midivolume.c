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

/*
 * This demo shows implementation of a MIDI Volume Pedal and sending
 * of Program Change Messages that can be adjusted by two push buttons.
 * The potentiometer of an external volume pedal is connected to 5 V, ground
 * and the analog input at PB2. The two push buttons SW1 and SW2 for choice of
 * the Program Change Message are connected to PB0 and PB1. In the following
 * schematic 'o' denotes conductive and '+' none-conductive connections.
 * The resistor of 1k as an external pullup at input pin #1 is necessary to
 * give enough current to the LED to lead to high state if SW1 is open. The
 * internal resistor is not strong enough.
 *
 *        Trinket
 *      ,---------,
 *   .--o Gnd  #0 o----------------------------------.
 *   |  |         |                                  |
 *   |  |      #1 o--------------------------.       |
 *   |  |         |                          |       |
 *   |  |      #2 o--------.                 |       |
 *   |  |         |        |                 |       |
 *   |  |      5V o----o---+-------.         |       |
 *   |  '---------'    |   |       |   .-----o       |
 *   |                 |   |      .-.  |     |       |
 *   |                .-.  |      | |  |     o       o
 *   |            10k | |<-'   1k | |  |  SW1 \   SW2 \
 *   |            log | |         '-'  |     o       o
 *   |                '-'          |   |     |       |
 *   |                 |           '---'     |       |
 *   |                 |                     |       |
 *  ===               ===                   ===     ===
 *  Gnd               Gnd                   Gnd     Gnd
 *
 * The program was tested using a normal Trinket (not pro version).
 * BTW:
 * For a real application there is a way to avoid the  initial bootloader delay
 * on power up for the Trinket-mini 3V/5V versions by flashing an alternate
 * bootloader. For advice see here:
 * http://forums.adafruit.com/viewtopic.php?f=52&t=78016&p=420552#p420552
*/

#include <avr/power.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>

#include "usbconfig.h"
#include "usbdrv/usbdrv.h"

#include "vusbmidi.h"   // V-USB MIDI definitions & routines
#include "trinketusb.h" // Trinket oscillator calibration & USB init
#include "usbmidi.h"    // usbmidi*() functions

#define MAXPROGCHANGE 30  // adapt to number of different program change messages

void SetupADC(void);

int main(void) {
    uint8_t  key1down=0;
    uint8_t  key2down=0;
    uint8_t  volume=0, old_volume=0;
    uint8_t  count=0;
    uint8_t  ProgChange = MAXPROGCHANGE;
    uint8_t  debounce=0;
    bool     UP=true;
    uint16_t adc_cumulate=0;


    wdt_enable(WDTO_1S);
    trinketUsbBegin();

    PORTB |= _BV(PB0); // Pullup on button PB0
    PORTB |= _BV(PB1); // Pullup on button PB1

    SetupADC();
    while(1) {
        wdt_reset();
        usbPoll();

        //******************* handle ADC readings ******************************
        while(ADCSRA & (1<<ADSC)); // wait until conversion is complete
        adc_cumulate += ADCH;      // sum up the read ADC values
        ADCSRA |= (1<<ADSC);       // start next ADC conversion
        count++;
        if (count == 64)           // building an average out of 64 AD readings
        {
          // assign new ADC value and scale it into range 0 .. 127;
          volume = adc_cumulate / 128;
          adc_cumulate = 0;
          count = 0;

          // for hysteresis reason we decide if slider is moving up or down,
          // at least two counts difference are necessary to define UP state.
          // Same for the DOWN state (= !UP state). Using that method we ensure
          // to not oscillating between two consecutive readings
          // (eg. 15 - 16 - 15 - 16 - ...)
          if (volume > old_volume+1) UP = true;
          if (volume+1 < old_volume) UP = false;

          // if slider is moving UP and we have at least 2 counts difference
          // to the last reading of volume we send a new volume value to device
          if ((volume > old_volume) && UP)
          {
            // controller number 7 is overall volume
            usbmidiControlChange(7, volume);
            old_volume = volume;
          }
          // if slider is moving DOWN and we have at least 2 counts difference
          // to the last reading of volume we send a new volume value to device
          if ((volume < old_volume) && !UP)
          {
            usbmidiControlChange(7, volume);
            old_volume = volume;
          }
        }

        //*********** handle reading of buttons at PB0 and PB1 *****************
        if(debounce) debounce--;
        else
        {
          if(!(PINB & _BV(PB0)) != key1down) { // button 1 state change
            key1down = !key1down;              // key1down reflect current state
            if(key1down) {
              ProgChange += 1;
              if (ProgChange > MAXPROGCHANGE-1) ProgChange = 0;
              usbmidiControlChange(0xC0, ProgChange);
            }
          }

          if(!(PINB & _BV(PB1)) != key2down) { // button 2 state change
            key2down = !key2down;              // key2down reflect current state
            if(key2down) {
              if (ProgChange == 0) ProgChange = MAXPROGCHANGE;
              ProgChange -= 1;
              usbmidiControlChange(0xC0, ProgChange);
            }
          }
          debounce = 100;
        }

        usbmidiSend();
    }

    return 0;
}

void SetupADC(void)
{
  // Idea taken mostly from: www.mikrocontroller.net/topic/400750#4630234
  //ADC Multiplexer Selection Register, see p. 134 datasheet
  ADMUX |= (1<<ADLAR)| //left shift result (for 8 bit)
           (0<<REFS1)| //sets ref. voltage to VCC, p. 134 datasheet
           (0<<REFS0)| //sets ref. voltage to VCC
           (0<<MUX3)|  //use ADC1 for input (PB2), MUX bit 3, p. 135 datasheet
           (0<<MUX2)|  //use ADC1 for input (PB2), MUX bit 2
           (0<<MUX1)|  //use ADC1 for input (PB2), MUX bit 1
           (1<<MUX0);  //use ADC1 for input (PB2), MUX bit 0

  //ADC Control and Status Register A, see p. 136 datasheet
  ADCSRA |= (1<<ADEN)  | //enable ADC
            (1<<ADPS2) | //set prescaler to 128, bit 2, p. 136 datasheet
            (1<<ADPS1) | //set prescaler to 128, bit 1
            (1<<ADPS0);  //set prescaler to 128, bit 0
            //prescaler set to 128 - 125 kHz @ 16 MHz

  ADCSRA |= (1<<ADSC); //start first ADC measurement
}
