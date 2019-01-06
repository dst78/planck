# Frequency tuning
This file describes how to generate custom-tuning for your module. This is necessary when your Arduinos frequency quartz is slightly off or just to get a different frequency range.

The original code was found on [Ginko Synthese](http://www.ginkosynthese.com/product/grains/) but this version is highly adapted. The underlying formula remains the same however.

## Background
Due to variance in an individual Arduinos' frequency crystal it is somewhat likely that you will have to tune your Planck VCO.

I'm providing a solution to this by means of generating custom voltage-to-octave PWM frequency tables. You will need a C compiler on your computer that works from the commandline. Since Planck is an Arduino project, chances are that this is already the case. Note however that I won't be able to support you with setting this up, sorry.

## How you tune the Arduino
The V/Oct mapping relies on a base note and a base increment.

The base note defines what you hear when Planck receives exactly 0 volts via the V/Oct input jack.

*The default value for this is C1, or 32.7 Hertz.*

The base incement defines how much the frequency increases per voltage increment. With a perfect quartz crystal this would be one octave per volt.

*The underlying mathematical default for this is 2.0.*

### Step one: Test your tuning
The values in the vOctTable.h file are tuned to my personal quartz, they may or may not work with yours. (32.66 Hertz base note and 1.955 base increment)

To test the tuning of your Planck, send precisely known voltages to the V/Oct jack and check the generated note with a tuner or a guitar tuner app for your smartphone.

If your Planck VCO is out of tune, continue with...

### Step two: compile the custom tuner
- open the commandline and move to the freqGen subdirectory
- call 'make'
- you should have a genVOctTable executable now.

### Step three: Tune the base note C1

The 'clean' base note has a frequency of 32.7 Hertz. You can generate frequency tables with a different base note by calling

./genVOctTable -n [your_base_note]

So for a base note of 32.6 Hertz you'd call
./genVOctTable -n 32.6

...and the vOctTable.h filein the main directory would be rewritten with that value.

To find the right value for you is a bit of trial and error. In detail:

- Send a precise zero Volts signal to the V/Oct input.
- Call ./genVOctTable -n [your_base_note] as described above
- Recompile the Arduino project and upload the sketch.
- Test the tuning again.

Rinse and repeat until the base note is in tune. Due to the resolution of the PWM signal you may not get this perfect and the note might remain a few cents off.

Once the base note is tuned, see if other notes are in tune or not. If you experience frequency drift between zero and five volts then continue with...

### Step four: Tune the frequency compensation

The other parameter that genVOctTable can manipulate is what I call the base increment. The frequency-per-volt rises as a function of 2.0^v (two to the power of v) where v is the voltage, assuming a perfect frequency crystal.

If yours is slightly fast or slow, you need to adjust the 2.0 part of the equation. To do this:

- Call ./genVOctTable -n [your_base_note] -i [your_increment]
- Change [your_increment] from 2.0 to 1.95 or something small like that. The frequency table is very sensitive to small changes.
- Recompile the Arduino project and upload the sketch.

Now feed known voltages to Planck and see if the frequency drift improves.
Rinse and repeat until the frequency drift is eliminated.

Voila, you now have a tuned granular oscillator.
