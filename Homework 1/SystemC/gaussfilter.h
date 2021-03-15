#ifndef GAUSS_FILTER_H_
#define GAUSS_FILTER_H_
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <systemc>
using namespace sc_core;

#include "filter_def.h"

class gaussfilter : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
  sc_fifo_in<unsigned char> i_w;
  sc_fifo_in<unsigned char> i_r;
  sc_fifo_in<unsigned char> i_g;
  sc_fifo_in<unsigned char> i_b;
  sc_fifo_out<int> o_result;

  SC_HAS_PROCESS(gaussfilter);
  gaussfilter(sc_module_name n);
  ~gaussfilter() = default;

private:
  void do_filter();
  int val[MASK_N];
};
#endif
