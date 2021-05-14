#include <cmath>
#ifndef NATIVE_SYSTEMC
  #include "stratus_hls.h"
#endif

#include "Gaussian_Blur.h"
using namespace sc_dt;
using namespace std;

Gaussian_Blur::Gaussian_Blur(sc_module_name n) : sc_module(n) {
  SC_THREAD(do_filter);
  sensitive << i_clk.pos();
  dont_initialize();
  reset_signal_is(i_rst, false);

#ifndef NATIVE_SYSTEMC
	i_r.clk_rst(i_clk, i_rst);
  i_g.clk_rst(i_clk, i_rst);
  i_b.clk_rst(i_clk, i_rst);
  o_result_r.clk_rst(i_clk, i_rst);
  o_result_g.clk_rst(i_clk, i_rst);
  o_result_b.clk_rst(i_clk, i_rst);
#endif
}

Gaussian_Blur::~Gaussian_Blur() {}

// Gaussian_Blur
const int filter[filterHeight][filterWidth] =
{
  {1, 1, 1},
  {1, -7, 1},
  {1, 1, 1}
};
const int factor = 1;

void Gaussian_Blur::do_filter() {
  sc_uint<8> v, u;
  {
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		i_r.reset();
    i_g.reset();
    i_b.reset();
		o_result_r.reset();
    o_result_g.reset();
    o_result_b.reset();
#endif
		wait();
	}
  while (true) {
    red = 0; green = 0; blue = 0;
    for (v = 0; v != filterHeight; ++v) {
      for (u = 0; u != filterWidth; ++u) {
#ifndef NATIVE_SYSTEMC
				{
					HLS_DEFINE_PROTOCOL("input");
					r = i_r.get();
          g = i_g.get();
          b = i_b.get();
					wait();
				}
#else
				r = i_r.read();
        g = i_g.read();
        b = i_b.read();
#endif
        HLS_CONSTRAIN_LATENCY(0, 6, "lat01");
        red += r * filter[u][v];
        green += g * filter[u][v];
        blue += b * filter[u][v];
      }
    }
    red = int(min(max(int(red),0),255));
    green = int(min(max(int(green),0),255));
    blue = int(min(max(int(blue),0),255));
#ifndef NATIVE_SYSTEMC
		{
			HLS_DEFINE_PROTOCOL("output");
			o_result_r.put(red);
      o_result_g.put(green);
      o_result_b.put(blue);
			wait();
		}
#else
    o_result_r.write(red);
    o_result_g.write(green);
    o_result_b.write(blue);    
#endif
  }
}
