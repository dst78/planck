# Planck - Auduino Modular Synth Concept

granular-esque VCO based on the Auduino in the eurorack format

## general outline
- two grains as in original Auduino
- 4 pots for grain frequency and decay
- 5th pot for V/Oct
- CV input for V/Oct, normalled to V/Oct pot
- audio out
- 6th pot for chaos amount
- chaos amount in, normalled to chaos pot
- chaos trigger in, maybe normalled to momentary switch

## chaos
- grain frequency CV inputs are digitally mixed with random values
- random values are sampled at every chaos trigger signal
- value of chaos amount pot determines by how much the original grain CV is offset (+-)

## software notes
- arduino code must have chromatic scale mapping to V/Oct for stepped notes *or* use map() to correctly map fluently
- how slow is random() ?

## hardware notes
- all input voltages must be mapped to 0-5V and [protected against overvoltage](http://www.doepfer.de/DIY/a100_diy.htm)
	
