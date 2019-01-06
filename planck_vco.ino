// ************************************************************************
// AUDUINO - MIDI Upgrade v1.1
//
// MIDI programming by David Benn http://www.notesandvolts.com
//
// Tutorial - http://www.notesandvolts.com/2015/05/auduino-synth-midi.html
// 
// ** Requires Arduino MIDI Library v4.2 or later **
//
// Based on the Auduino Synthesizer v5 by Peter Knight http://tinker.it
// ************************************************************************
// Version 1.0 - Initial release
// Version 1.1 - Fixed bug that caused note to hang with some DAWs
// ************************************************************************
// modifications by Anoikis Nomads
// Version 1.2 - added logic to read a single bit to determine MIDI-mode or free-running mode
// Version 1.3 - removed pentatonic note 0 and ultra-low ones. removed LED handling.
// ************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>

#include "noteTables.h"
#include "vOctTable.h"

// *******************************************************************************

uint16_t syncPhaseAcc;
uint16_t syncPhaseInc;

uint16_t grain1PhaseAcc;
uint16_t grain1PhaseInc;
uint16_t grain1Amp;
uint8_t  grain1Decay;

uint16_t grain2PhaseAcc;
uint16_t grain2PhaseInc;
uint16_t grain2Amp;
uint8_t  grain2Decay;

#define CHAOS_INTERPOLATION_STEPS 50
#define CHAOS_AMOUNT_MIN -150
#define CHAOS_AMOUNT_MAX 150

uint8_t chaosAmount;
int16_t chaosVal1, oldChaosVal1, newChaosVal1;
int16_t chaosVal2, oldChaosVal2, newChaosVal2;
uint8_t chaosInterpolationStep;
volatile byte ledState = LOW;
volatile boolean readChaos = false;

// Map analogue & digital channels
#define SYNC_CONTROL         A4
#define GRAIN1_FREQ_CONTROL  A0
#define GRAIN1_DECAY_CONTROL A2
#define GRAIN2_FREQ_CONTROL  A3
#define GRAIN2_DECAY_CONTROL A1

#define CHAOS_AMOUNT         A5
#define CHAOS_TRIG_INT       PD7
#define CHAOS_TRIG_EXT       PD2

#if defined(__AVR_ATmega8__)
//
// On old ATmega8 boards.
//    Output is on pin 11
//
#define LED_PIN       13
#define PWM_PIN       11
#define PWM_VALUE     OCR2
#define PWM_INTERRUPT TIMER2_OVF_vect
#elif defined(__AVR_ATmega1280__)
//
// On the Arduino Mega
//    Output is on pin 3
//
#define LED_PIN       13
#define PWM_PIN       3
#define PWM_VALUE     OCR3C
#define PWM_INTERRUPT TIMER3_OVF_vect
#else
//
// For modern ATmega168 and ATmega328 boards
//    Output is on pin 3
//
#define LED_PIN       13
#define PWM_PIN       3
#define PWM_VALUE     OCR2B
#define PWM_INTERRUPT TIMER2_OVF_vect
#endif

// Notemapping values are calculated as follows:
// Map = round(Hz * 65536 / 31250)
//
// 65536 is the width of the counter for the phase angle
// 31250 is the sample rate of the output in Hz
//
// Auduino Google Group post on the topic: https://groups.google.com/forum/#!topic/auduino/YWlt6Sd2i-8
// equal-tempered scale                  : http://pages.mtu.edu/~suits/notefreqs.html

// Post on CV inputs
// https://groups.google.com/forum/#!topic/auduino/0TXGT-vQSwI


//Global Varibles
int oldpot = 0;

void audioOn() {
#if defined(__AVR_ATmega8__)
  // ATmega8 has different registers
  TCCR2 = _BV(WGM20) | _BV(COM21) | _BV(CS20);
  TIMSK = _BV(TOIE2);
#elif defined(__AVR_ATmega1280__)
  TCCR3A = _BV(COM3C1) | _BV(WGM30);
  TCCR3B = _BV(CS30);
  TIMSK3 = _BV(TOIE3);
#else
  // Set up PWM to 31.25kHz, phase accurate
  TCCR2A = _BV(COM2B1) | _BV(WGM20);
  TCCR2B = _BV(CS20);
  TIMSK2 = _BV(TOIE2);
#endif
}

void setup() {
  randomSeed(analogRead(1));
  
  pinMode(PWM_PIN, OUTPUT);
  
  audioOn();
  oldpot = analogRead(SYNC_CONTROL);

  pinMode(LED_PIN, OUTPUT);

  pinMode(CHAOS_TRIG_INT, OUTPUT);
  pinMode(CHAOS_TRIG_EXT, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(CHAOS_TRIG_EXT), handleChaosTrigger, RISING);
  chaosVal1   = 0;
  chaosVal2   = 0;
  chaosAmount = 0;
}

/**
 * main loop
 */
void loop() {
  if (readChaos) {
    readChaos = false; // reset flag
    
    uint16_t val = analogRead(CHAOS_AMOUNT);
  
    // hysteresis for the potentiometer
    if (val > 100) {
      chaosInterpolationStep = 0;
      chaosAmount = map(val, 0, 1024, 0, 100);
      oldChaosVal1 = newChaosVal1;
      newChaosVal1 = (random(CHAOS_AMOUNT_MIN, CHAOS_AMOUNT_MAX) * chaosAmount) / 100;
      oldChaosVal2 = newChaosVal1;
      newChaosVal2 = (random(CHAOS_AMOUNT_MIN, CHAOS_AMOUNT_MAX) * chaosAmount) / 100;
      
    } else {
      chaosVal1 = 0;
      chaosVal2 = 0;
    }
  }

  if (chaosInterpolationStep < CHAOS_INTERPOLATION_STEPS) {
    chaosVal1 = map(chaosInterpolationStep, 0, CHAOS_INTERPOLATION_STEPS, oldChaosVal1, newChaosVal1);
    chaosVal2 = map(chaosInterpolationStep, 0, CHAOS_INTERPOLATION_STEPS, oldChaosVal2, newChaosVal2);
    chaosInterpolationStep++;
  }
  
  digitalWrite(LED_PIN, ledState);

  int analogNote = analogRead(SYNC_CONTROL);
  
  // syncPhaseInc is the increment of progression through a grain
  syncPhaseInc = mapFreq(analogNote);

  grain1PhaseInc = mapPhaseInc(constrain(chaosVal1 + analogRead(GRAIN1_FREQ_CONTROL), 0, 1023)) / 2;
  grain2PhaseInc = mapPhaseInc(constrain(chaosVal2 + analogRead(GRAIN2_FREQ_CONTROL), 0, 1023)) / 2;
  grain1Decay    = analogRead(GRAIN1_DECAY_CONTROL) / 8;
  grain2Decay    = analogRead(GRAIN2_DECAY_CONTROL) / 4;
}

SIGNAL(PWM_INTERRUPT) {
  uint8_t value;
  uint16_t output;

  syncPhaseAcc += syncPhaseInc;
  if (syncPhaseAcc < syncPhaseInc) {
    // Time to start the next grain
    // this is because syncPhaseAcc did overflow (16 bit = 66535)
    grain1PhaseAcc = 0;
    grain1Amp = 0x7fff;
    grain2PhaseAcc = 0;
    grain2Amp = 0x7fff;
  }

  // Increment the phase of the grain oscillators
  grain1PhaseAcc += grain1PhaseInc;
  grain2PhaseAcc += grain2PhaseInc;

  // Convert phase into a triangle wave
  value = (grain1PhaseAcc >> 7) & 0xff;
  if (grain1PhaseAcc & 0x8000) {
    value = ~value;
  }
  // Multiply by current grain amplitude to get sample
  output = value * (grain1Amp >> 8);

  // Repeat for second grain
  value = (grain2PhaseAcc >> 7) & 0xff;
  if (grain2PhaseAcc & 0x8000) {
    value = ~value;
  }
  output += value * (grain2Amp >> 8);

  // Make the grain amplitudes decay by a factor every sample (exponential decay)
  // this is exponential because of the binary bitshift right
  // grain1Decay and grain2Decay are values from potentiometers
  grain1Amp -= (grain1Amp >> 8) * grain1Decay;
  grain2Amp -= (grain2Amp >> 8) * grain2Decay;

  // Scale output to the available range, clipping if necessary
  output >>= 9;
  if (output > 255) {
    output = 255;
  }

  // Output to PWM (this is faster than using analogWrite)
  PWM_VALUE = output;
}

/**
 * linear interpolation
 */
float lerp(float min, float max, float val) {
  return min + val * (max - min);
}

/**
 * interrupt handler
 * 
 * calculates new grain frequency offset based on chaos value
 */
void handleChaosTrigger() {
  readChaos = true;
  ledState = !ledState;
}
//--------------------------------------------------------------
// various mapper functions to map volts to musical scales
// the frequency arrays can be found in noteTables.h
//--------------------------------------------------------------

uint16_t mapFreq(uint16_t input) {
  return pgm_read_word_near(freqTable + input);
}

uint16_t mapPhaseInc(uint16_t input) {
  return (antilogTable[input & 0x3f]) >> (input >> 6);
}

uint16_t mapMidi(uint16_t input) {
  return (midiTable[input]);
}

uint16_t mapChromatic(uint16_t input) {
  uint8_t value = input / (1024 / 59);
  return (chromaticTable[value]);
}

uint16_t mapPentatonic(uint16_t input) {
  uint8_t value = input / (1024 / 45);
  return (pentatonicTable[value]);
}
