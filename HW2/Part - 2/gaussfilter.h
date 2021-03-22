#ifndef GAUSS_FILTER_H_
#define GAUSS_FILTER_H_

#include <systemc>
using namespace sc_core;

#include "filter_def.h"

class gaussfilter : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
  
  sc_fifo_in<double> i_r;
  sc_fifo_in<double> i_g;
  sc_fifo_in<double> i_b;
  
  sc_fifo_out<double> o_result_r;
  sc_fifo_out<double> o_result_g;
  sc_fifo_out<double> o_result_b;
  SC_HAS_PROCESS(gaussfilter);
  gaussfilter(sc_module_name n);
  ~gaussfilter() = default;

private:
  void do_filter();
  //int val[MASK_N];
};
#endif
