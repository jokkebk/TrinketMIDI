# TrinketMIDI

V-USB MIDI implementation for Adafruit Trinket.

# Building

1. Install `avr-gcc`, `avrdude` and some flavor of GNU `make`
2. Follow Adafruit instructions and test you can flash .hex files with avrdude
3. Enter command-line, change to TrinketMIDI directory and type `make all` to
   build everything.
4. Connect Trinket to your computer, wire a switch between pin #0 and GND
5. Push reset button on Trinket and type `make flash`
6. In a moment, you'll have a MIDI device and the button sends middle C note!

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
