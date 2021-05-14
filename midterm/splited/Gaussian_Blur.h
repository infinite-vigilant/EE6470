#ifndef SOBEL_FILTER_H_
#define SOBEL_FILTER_H_
#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

#include "filter_def.h"

class Gaussian_Blur : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_uint<8> >::in i_r;
  cynw_p2p< sc_dt::sc_uint<8> >::in i_g;
  cynw_p2p< sc_dt::sc_uint<8> >::in i_b;
	cynw_p2p< sc_dt::sc_uint<8> >::out o_result_r;
  cynw_p2p< sc_dt::sc_uint<8> >::out o_result_g;
  cynw_p2p< sc_dt::sc_uint<8> >::out o_result_b;
#else
	sc_fifo_in< sc_dt::sc_uint<8> >::in i_r;
  sc_fifo_in< sc_dt::sc_uint<8> >::in i_g;
  sc_fifo_in< sc_dt::sc_uint<8> >::in i_b;
  sc_fifo_out< sc_dt::sc_uint<8> >::out o_result_r;
  sc_fifo_out< sc_dt::sc_uint<8> >::out o_result_g;
  sc_fifo_out< sc_dt::sc_uint<8> >::out o_result_b;
#endif
  

  SC_HAS_PROCESS(Gaussian_Blur);
  Gaussian_Blur(sc_module_name n);
  ~Gaussian_Blur();

private:
  void do_filter();
  sc_dt::sc_uint<32> red;
  sc_dt::sc_uint<32> green;
  sc_dt::sc_uint<32> blue;
  sc_dt::sc_uint<8> r;
  sc_dt::sc_uint<8> g;
  sc_dt::sc_uint<8> b;
};
#endif
