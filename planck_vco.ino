// ************************************************************************
// Based on the Auduino Synthesizer v5 by Peter Knight http://tinker.it
// Help:      http://code.google.com/p/tinkerit/wiki/Auduino
// More help: http://groups.google.com/group/auduino
//
// ************************************************************************
// modifications by Anoikis Nomads
// version 1.0 - first release
// ************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>

#include "noteTables.h"
#include "vOctTable.h"

// *******************************************************************************
#define DEBUG false

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

// length of interpolation between chaos-corrected grain frequencies 
// feel free to increase this for a more gradual change (or connect to another pot / jack for CV control)
#define CHAOS_INTERPOLATION_STEPS 50  
// adjust this if you want more / less maximum chaos. minimum chaos is always 0
#define CHAOS_AMOUNT_MIN -150
#define CHAOS_AMOUNT_MAX 150

// current chaos amount as per potentiometer / CV
uint8_t chaosAmount;
int16_t chaosVal1, oldChaosVal1, newChaosVal1;
int16_t chaosVal2, oldChaosVal2, newChaosVal2;
// interpolates between old and new chaos values for a smoother experience
uint8_t chaosInterpolationStep; 
// interrupt routine flag
volatile boolean readChaos = false;
// for debugging
#if DEBUG
volatile byte ledState = LOW;
#endif

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

void setup() {
  randomSeed(analogRead(1));
  
  pinMode(PWM_PIN, OUTPUT);
  
  audioOn();

  #if DEBUG
  pinMode(LED_PIN, OUTPUT);
  #endif

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
  // new chaos trigger has been received, read chaos amount and process the new values
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

  #if DEBUG
  digitalWrite(LED_PIN, ledState);
  #endif

  int analogNote = analogRead(SYNC_CONTROL);
  
  // syncPhaseInc is the increment of progression through a grain
  syncPhaseInc = mapFreq(analogNote);

  grain1PhaseInc = mapPhaseInc(constrain(chaosVal1 + analogRead(GRAIN1_FREQ_CONTROL), 0, 1023)) / 2;
  grain2PhaseInc = mapPhaseInc(constrain(chaosVal2 + analogRead(GRAIN2_FREQ_CONTROL), 0, 1023)) / 2;
  grain1Decay    = analogRead(GRAIN1_DECAY_CONTROL) / 8;
  grain2Decay    = analogRead(GRAIN2_DECAY_CONTROL) / 4;
}

/**
 * initializes the timer interrupt for grain calculation
 */
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

/**
 * interrupt timer
 */
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
  #if DEBUG
  ledState = !ledState;
  #endif
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
