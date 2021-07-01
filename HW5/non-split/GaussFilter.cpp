#include <cmath>
#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

#include "GaussFilter.h"

GaussFilter::GaussFilter( sc_module_name n ): sc_module( n )
{
#ifndef NATIVE_SYSTEMC
#endif
	SC_THREAD( do_filter );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);
        
#ifndef NATIVE_SYSTEMC
	input_rgb.clk_rst(i_clk, i_rst);
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
		input_rgb.reset();
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
				sc_dt::sc_uint<24> rgb;
#ifndef NATIVE_SYSTEMC
				{
					HLS_DEFINE_PROTOCOL("input");
					rgb = input_rgb.get();
					wait();
				}
#else
				rgb = input_rgb.read();
#endif
				{
					HLS_CONSTRAIN_LATENCY(0, 2, "lat01");
        			if ((rgb(7, 0) != 0) && (rgb(15, 8) != 0) && (rgb(23, 16) != 0)) total += mask[u][v];
					r += rgb(7, 0) * mask[u][v];
					g += rgb(15, 8) * mask[u][v];
					b += rgb(23, 16) * mask[u][v];
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
