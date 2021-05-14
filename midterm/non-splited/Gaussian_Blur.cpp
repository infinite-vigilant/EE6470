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
	i_rgb.clk_rst(i_clk, i_rst);
  o_result.clk_rst(i_clk, i_rst);
#endif
}

Gaussian_Blur::~Gaussian_Blur() {}

// Gaussian_Blur
const sc_uint<8> filter[filterHeight][filterWidth] =
{
  1, 1, 1,
  1, -7, 1,
  1, 1, 1,
};
const sc_uint<8> factor = 1;

void Gaussian_Blur::do_filter() {
  {
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		i_rgb.reset();
		o_result.reset();
#endif
		wait();
	}
  while (true) {
    // HLS_CONSTRAIN_LATENCY(0, 1, "lat00");
    red = 0; green = 0; blue = 0;
    for (unsigned int v = 0; v < filterHeight; ++v) {
      for (unsigned int u = 0; u < filterWidth; ++u) {
        sc_uint<24> rgb;
#ifndef NATIVE_SYSTEMC
				{
					HLS_DEFINE_PROTOCOL("input");
					rgb = i_rgb.get();
					wait();
				}
#else
				rgb = i_rgb.read();
#endif
        sc_uint<8> p_red = rgb.range(7,0);
        sc_uint<8> p_green = rgb.range(15,8);
        sc_uint<8> p_blue = rgb.range(23,16);
        HLS_CONSTRAIN_LATENCY(0, 3, "lat01");
        
        red += p_red * filter[u][v] / factor;
        green += p_green * filter[u][v] / factor;
        blue += p_blue * filter[u][v] / factor;
      }
    }
    red = int(min(max(int(red),0),255));
    green = int(min(max(int(green),0),255));
    blue = int(min(max(int(blue),0),255));
    
    // HLS_CONSTRAIN_LATENCY(0, 1, "lat00");
    sc_uint<24> total = 0;
    // HLS_CONSTRAIN_LATENCY(0, 1, "lat01");
    total.range(7,0) = red;
    total.range(15,8) = green;
    total.range(23,16) = blue;
#ifndef NATIVE_SYSTEMC
		{
			HLS_DEFINE_PROTOCOL("output");
			o_result.put(total);
			wait();
		}
#else
    o_result.write(total);    
#endif
  }
}
