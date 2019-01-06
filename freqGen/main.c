#include <stdio.h>
#include <math.h>

/**
 * This routine generates the PWM values to represent voltage-per-octave
 * notes. 
 *
 * The Arduino clock is driven by a 16MHz quartz crystal. Variance between
 * individual quartzes may make it necessary for you to tune your Auduino.
 *
 * You can use the compiled executable from this file to tune.
 *
 * Refer to the how to tune section in the readme for details.
 */
const float BASE_NOTE = 32.66; // C1 is 32.70 Hertz
const float BASE_INCR = 1.955; // set this to 2.0 as default

int main() { 
	printf("const uint16_t freqTable[] PROGMEM = {\n");
	
  for (int c = 0; c < 1024; c++) {
    float f = 5 * c / 1024.0;
    
    printf("%.0f, ", floor((65536 * BASE_NOTE * pow(BASE_INCR, f)) / 31250 + 0.5f)); 
		
		if (c % 10 == 0) {
			printf("\n");
		}
  }
 
	printf("\n};");
	
  return 0;
} 