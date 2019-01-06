# Planck - Auduino Modular Synth Concept

granular-esque VCO based on the [Auduino](https://code.google.com/archive/p/tinkerit/wikis/Auduino.wiki) for the eurorack format

## general outline
Planck processes two triangle waves to a grain. Both waves can be manipulated in terms of frequency and decay in order to achieve various harmonics. 

The module allows for manual setting of a frequency via potentiometer or CV input. 

What sets this module appart from the original Auduino is the addition of chaos.

## chaos
The triangle wave frequencies can be modulated through external CV. 

What is happening internally is that once a chaos trigger is received one random value is generated for each triangle wave. This value is shifting the wave frequency up or down, resulting in different grain harmonics.

The module comes with a potentiometer and CV input pair for external control of the chaos amount. With this the effects of chaos can be completely switched off even when receiving chaos triggers.

## hardware notes
Since this module is Arduino based, all input/outputs have been voltage limited via Schottky diodes to 0-5V. 

## A note on tuning
Due to variance in the frequency crystals of individual Arduinos it is likely that you have to tune your module before it is usable. You can do this by following the instructions outlined in the README inside the freqGen folder.

## What's in the name?
The module was named in honor of Max Planck and as a reference to the Planck-length. 
