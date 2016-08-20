# LanC Control #

The LanC control was developed by Sony to control camcorder functions from remote. It was adopted by other companies like Panasonic or Canon, even the cinema cameras from Blackmagic uses the simple remote protocol.

The protocol is a bit bizarre, because send and receive data is fitted in a strict timing, changing the read and write data in a single sequence. The advantage is the use of only three lines for power supply, send and receive. The standard plug for LanC is a 2,5mm stereo jack.

This code is written for the Arduino Micro and won't work on other Arduino types without adoption, because of a different timing. The three timing parameters
    int inTime = 60;
    int outTime = 85;
    int startBitTime = 95;

must be changed for correct operation.

## Functions ##
The code is stable, but not yet finished, because only the Rec/Stop functions are implemented. It is tested on a Canon XH A1.

The central routine is *scan()*, which performs a LanC cycle, sending the data handed over in *byte0* and *byte1*. The list of the valid data pairs can be found in the programs header.

The *scan()* routine returns the status of the camera, which can be displayed on the output pins *rec* and *pause* using the *set_tally()* routine. Four different stati can be displayed:
    eject (rec L, pause L)
    stop (rec L, pause H)
    rec pause (rec H, pause H)
    rec (rec H, pause L)

## Pinning ##
The input and output pins on the Aquino are defined in the port definition sequence:
    2 record light
    3 pause light
    4 = inverted 2
    5 = inverted 3
    6 data input
    7 data output
    8 debug output (flashing light)
    12 record button

The signals of pins 6 and 7 has to be connected by a simple transistor circuit.