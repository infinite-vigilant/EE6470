#ifndef GAUSS_FILTER_H_
#define GAUSS_FILTER_H_
#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

#include "filter_def.h"

class GaussFilter: public sc_module
{
public:
	sc_in_clk i_clk;
	sc_in < bool >  i_rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_uint<24> >::in input_rgb;
	cynw_p2p< sc_dt::sc_uint<8> >::out o_result_r;
	cynw_p2p< sc_dt::sc_uint<8> >::out o_result_g;
	cynw_p2p< sc_dt::sc_uint<8> >::out o_result_b;
#else
	sc_fifo_in< sc_dt::sc_uint<24> >::in input_rgb;
	sc_fifo_out< sc_dt::sc_uint<8> >::out o_result_r;
	sc_fifo_out< sc_dt::sc_uint<8> >::out o_result_g;
	sc_fifo_out< sc_dt::sc_uint<8> >::out o_result_b;	
#endif

	SC_HAS_PROCESS( GaussFilter );
	GaussFilter( sc_module_name n );
	~GaussFilter();
private:
	void do_filter();
	int r;
	int g;
	int b;
	unsigned int total;
};
#endif
