# Planck - Auduino Modular Synth Concept

granular-esque VCO based on the [Auduino](https://code.google.com/archive/p/tinkerit/wikis/Auduino.wiki) for the eurorack format

## general outline
Planck processes two triangle waves to a grain. Both waves can be manipulated in terms of frequency and decay in order to achieve various harmonics. Note that manipulation of the grain frequencies does *not* affect the frequency of the output, only the harmonics.

The module allows for manual setting of an output frequency via potentiometer or CV input. 

What sets this module appart from the original Auduino is the addition of chaos.

## chaos
The triangle wave frequencies can be indirectly modulated (but not controlled) through external chaos triggers. Again, this only affects the grain harmonics, not the output frequency.

What is happening internally is that once a chaos trigger is received, one random value is generated for each triangle wave. This value is shifting the wave frequency up or down, resulting in different grain harmonics.

The chaos amount knob on the panel allows you to adjust how much chaos affects the grain wave frequencies, down to turning the effect off completely. The Planck VCO also comes with a CV input for external control of the chaos amount.

When no trigger signal is plugged into the trigger input, a new chaos trigger is generated internally whenever the V/Oct input changes rapidly by at least one semitone. The internal chaos trigger is normalled to the chaos trigger jack on the panel. When a trigger is plugged into the chaos trigger jack on the panel, the internal triggers have no effect and the generation of new chaos values is completely controlled externally.


## hardware notes
Since this module is Arduino based, all input/outputs have been voltage limited via Schottky diodes to 0-5V. 

**There is no protection against connecting in the power supply cable incorrecting.**

## A note on tuning
Due to variance in the frequency crystals of individual Arduinos it is likely that you have to tune your module before it is usable. You can do this by following the instructions outlined in the README inside the freqGen folder.

## What's in the name?
The module was named in honor of Max Planck and as a reference to the Planck-length. 
