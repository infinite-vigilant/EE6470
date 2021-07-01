#include <cmath>
#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

#include "GaussFilter.h"

GaussFilter::GaussFilter( sc_module_name n ): sc_module( n )
{
#ifndef NATIVE_SYSTEMC
	// HLS_FLATTEN_ARRAY(val); (jieyun comment: since r, g, b are not array?)
#endif
	SC_THREAD( do_filter );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);
        
#ifndef NATIVE_SYSTEMC
	input_r.clk_rst(i_clk, i_rst);
	input_g.clk_rst(i_clk, i_rst);
	input_b.clk_rst(i_clk, i_rst);
  	o_result_r.clk_rst(i_clk, i_rst);
  	o_result_g.clk_rst(i_clk, i_rst);
  	o_result_b.clk_rst(i_clk, i_rst);
#endif
}

GaussFilter::~GaussFilter() {}

// gauss mask
const int mask[MASK_X][MASK_Y] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};

void GaussFilter::do_filter() {
	{
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		input_r.reset();
		input_g.reset();
		input_b.reset();
		o_result_r.reset();
		o_result_g.reset();
		o_result_b.reset();
#endif
		wait();
	}
	while (true) {
		{
			HLS_CONSTRAIN_LATENCY(0, 1, "lat00");
			r = 0;
			g = 0;
			b = 0;
			total = 0;
		}
		for (unsigned int v = 0; v < MASK_Y; ++v) {
			for (unsigned int u = 0; u < MASK_X; ++u) {
				sc_dt::sc_uint<8> r;
				sc_dt::sc_uint<8> g;
				sc_dt::sc_uint<8> b;
#ifndef NATIVE_SYSTEMC
				{
					HLS_DEFINE_PROTOCOL("input");
					r = input_r.get();
					g = input_g.get();
					b = input_b.get();
					wait();
				}
#else
				r = input_r.read();
				g = input_g.read();
				b = input_b.read();
#endif
				{
					HLS_CONSTRAIN_LATENCY(0, 2, "lat01");
        			if (r != 0 && g != 0 && b !=0 ) cnt += mask[u][v];
					r += r * mask[u][v];
					g += g * mask[u][v];
					b += b * mask[u][v];
				}
			}
		}
		{
			HLS_CONSTRAIN_LATENCY(0, 8, "lat02");
			r = r / total;
			g = g / total;
			b = b / total;
		}
#ifndef NATIVE_SYSTEMC
		{
			HLS_DEFINE_PROTOCOL("output");
			o_result_r.put(r);
			o_result_g.put(g);
			o_result_b.put(b);
			wait();
		}
#else
		o_result_r.write(r);
		o_result_g.write(g);
		o_result_b.write(b);
#endif
	}
}
