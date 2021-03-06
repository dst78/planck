// Stepped chromatic mapping
//
uint16_t midiTable[] = {
//C      C#     D      D#     E      F      F#     G      G#     A      A#     B
  0,     18,    19,    20,    22,    23,    24,    26,    27,    29,    31,    32,    // C-1 - B-1   12
  34,    36,    38,    41,    43,    46,    48,    51,    54,    58,    61,    65,    // C0 - B0     24
  69,    73,    77,    82,    86,    92,    97,    103,   109,   115,   122,   129,   // C1 - B1     36
  137,   145,   154,   163,   173,   183,   194,   206,   218,   231,   244,   259,   // C2 - B2     48
  274,   291,   308,   326,   346,   366,   388,   411,   435,   461,   489,   518,   // C3 - B3     60
  549,   581,   616,   652,   691,   732,   776,   822,   871,   923,   978,   1036,  // C4 - B4     72
  1097,  1163,  1232,  1305,  1383,  1465,  1552,  1644,  1742,  1845,  1955,  2071,  // C5 - B5     84
  2195,  2325,  2463,  2610,  2765,  2930,  3104,  3288,  3484,  3691,  3910,  4143,  // C6 - B6     96
  4389,  4650,  4927,  5220,  5530,  5859,  6207,  6577,  6968,  7382,  7821,  8286,  // C7 - B7    108
  8779,  9301,  9854,  10440, 11060, 11718, 12415, 13153, 13935, 14764, 15642, 16572, // C8 - B8    120
  17557, 18601, 19708, 20879, 22121, 23436, 24830, 26306                              // C9 - G9    128
};

/**
 * maps an input voltage to PWM frequencies for midi scale.
 */
uint16_t mapMidi(uint16_t input) {
  return (midiTable[input]);
}

uint16_t chromaticTable[60] = {
//C      C#     D      D#     E      F      F#     G      G#     A      A#     B
  137,   150,   154,   163,   173,   183,   194,   206,   218,   231,   244,   259,   // C2 - B2     12
  274,   291,   308,   326,   346,   366,   388,   411,   435,   461,   489,   518,   // C3 - B3     24
  549,   581,   616,   645,   691,   732,   776,   822,   871,   923,   978,   1036,  // C4 - B4     36
  1097,  1163,  1232,  1305,  1383,  1465,  1552,  1644,  1742,  1845,  1955,  2071,  // C5 - B5     48
  2195,  2325,  2463,  2610,  2765,  2930,  3104,  3288,  3484,  3691,  3910,  4143,  // C6 - B6     60  
};

/**
 * maps an input voltage to PWM frequencies for chromatic scale.
 */
uint16_t mapChromatic(uint16_t input) {
  uint8_t value = input / (1024 / 59);
  return (chromaticTable[value]);
}


// Stepped Pentatonic mapping
uint16_t pentatonicTable[45] = {
//D      E      G      A      B
  /*0, */
  19,    22,    26,    29,    32,     // D-1 - B-1
  38,    43,    51,    58,    65,     // D0 - B0
  77,    86,    103,   115,   129,    // D1 - B1
  154,   173,   206,   231,   259,    // D2 - B2
  308,   346,   411,   461,   518,    // D3 - B3
  616,   691,   822,   923,   1036,   // D4 - B4
  1232,  1383,  1644,  1845,  2071,   // D5 - B5
  2463,  2765,  3288,  3691,  4143,   // D6 - B6
  4927,  5530,  6577,  7382,  8286,   // D7 - B7
  //9854,  11060, 13153, 14764, 16572,  // D8 - B8
  //19708, 22121, 26306                 // D9 - G9
};

/**
 * maps an input voltage to PWM frequencies for pentatonic scale.
 */
uint16_t mapPentatonic(uint16_t input) {
  uint8_t value = input / (1024 / 45);
  return (pentatonicTable[value]);
}
