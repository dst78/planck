# Planck - Auduino Modular Synth Concept

granular-esque VCO based on the [Auduino](https://code.google.com/archive/p/tinkerit/wikis/Auduino.wiki) in the eurorack format

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
- value of chaos amount pot determines by how much the original grain frequency CV is offset (+-)
