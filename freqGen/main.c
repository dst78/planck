#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
const float BASE_NOTE = 32.7; 
const float BASE_INCR = 2.0;  

int main(int argc, char *argv[]) { 
  int opt;
  float baseNote = BASE_NOTE;
  float baseIncr = BASE_INCR;
  char *fileName = "../vOctTable.h";
  char confirm[3];
  FILE *f;
    
  // parse command line arguments
  while((opt = getopt(argc, argv, "i:n:f:")) != -1) {  
    switch(opt) {  
      case 'n':  
        baseNote = atof(optarg);
        break;  
      case 'i':  
        baseIncr = atof(optarg); 
        break;  
      case 'f':
        fileName = optarg;
        break;
      case ':':  
        printf("option needs a value\n");
        return 1;
        break;  
      case '?':  
        printf("unknown option: %c\n", optopt); 
        return 1;
        break;  
    } 
  }
  
  // confirmation
  printf("\n");
  printf("will generate PWM values based on the follwing settings:\n");
  printf("base note: %.3f\tdefault is %.3f\n", baseNote, BASE_NOTE);  
  printf("base incr: %.4f\tdefault is %.4f\n", baseIncr, BASE_INCR); 
  printf("writing PWM values to\t%s\n", fileName);
  printf("\nContinue? [yN]: ");
  
  fflush(stdin);
  fgets(confirm, 2, stdin);
  
  if (confirm[0] != 'y' && confirm[0] != 'Y') {
    printf("exiting...");
    return 0;
  }
  
  f = fopen(fileName, "w");
  
  if (f == NULL) {
    printf("error writing to file!\n");
    return 1;
  }
  
  fprintf(f, "// custom tuning\n");
  fprintf(f, "// baseNote used: %.4f\n", baseNote);
  fprintf(f, "// baseIncr used: %.4f\n", baseIncr);
  fprintf(f, "const uint16_t freqTable[] PROGMEM = {\n");

  for (int c = 0; c < 1024; c++) {
    float freq = 5 * c / 1024.0;

    fprintf(f, "%.0f, ", floor((65536 * baseNote * pow(baseIncr, freq)) / 31250 + 0.5f)); 

    if (c % 10 == 0) {
      fprintf(f, "\n");
    }
  }

  fprintf(f, "\n};");
  fclose(f);

  return 0;
} 