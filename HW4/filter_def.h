#ifndef FILTER_DEF_H_
#define FILTER_DEF_H_

// Gauss mask parameters
const int MASK_N = 2;
const int MASK_X = 3;
const int MASK_Y = 3;

// Gauss Filter inner transport addresses
// Used between blocking_transport() & do_filter()
const int Gauss_FILTER_R_ADDR = 0x00000000;
const int Gauss_FILTER_RESULT_ADDR = 0x00000004;

union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};

// Gauss mask
const int mask[MASK_N][MASK_X][MASK_Y] = {{{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}},

                                          {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}}};
#endif
