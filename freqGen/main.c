#include <stdio.h>
#include <math.h>

int main() { 
	printf("const uint16_t freqTable[] PROGMEM = {\n");
	
  for (int c = 0; c < 1024; c++) {
    float f = 5 * c / 1024.0;
    
    printf("%.0f, ", floor(65536 * 32.70 * pow(2.0, f)) / 31250 + 0.5f); 
		
		if (c % 10 == 0) {
			printf("\n");
		}
  }
 
	printf("\n};");
	
  return 0;
} 