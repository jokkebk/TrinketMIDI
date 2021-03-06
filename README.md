# TrinketMIDI

V-USB MIDI implementation for Adafruit Trinket. Also supports Adafruit Pro Trinket.

# Building

1. Install `avr-gcc`, `avrdude` and some flavor of GNU `make`
2. Follow [Adafruit
   instructions](https://learn.adafruit.com/introducing-trinket/programming-with-avrdude)
and test you can flash .hex files with avrdude
3. Modify Makefile if needed to change part and programmer definitions (based on step 2).
4. Enter command-line, change to TrinketMIDI directory and type `make all` to
   build everything.
5. Connect (Pro) Trinket to your computer, wire a switch between pin #2 and GND
6. Push reset button on your Trinket and type `make flash`
7. In a moment, you'll have a MIDI device and the button sends middle C note!

*NOTE*: Different part numbers and definitions are needed for Trinket and Pro Trinket! If you switch between the two,
make sure to force recompiling everything with `make clean` first!

Other demos (if any) are named `demo_xxx.c` and can be built with `make
demo_xxx.hex` and flashed with `make demo_xxx.flash`.

# Licensing

USB initialization and oscillator calibration methods in trinketusb.c are from
Adafruit TrinketKeyboard library and licensed under [LGPL v3](LICENSE_LGPL3)

V-USB MIDI device descriptors and V-USB skeletop functions in vusbmidi.c are from Martin
Homuth-Rosemann's V-USB MIDI project and licenced under [GPL v2](LICENSE_VUSBMIDI)

Original code licenced under [GPL v3](LICENSE) which should be
compatible with both of the above. Enjoy!

EVERYTHING COMES AS-IS, USE AT YOUR OWN RESPONSIBILITY! See source and licence
files for additional disclaimers.
