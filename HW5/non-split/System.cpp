#include "System.h"
System::System( sc_module_name n, string input_bmp, string output_bmp ): sc_module( n ), 
	tb("tb"), gauss_filter("gauss_filter"), clk("clk", CLOCK_PERIOD, SC_NS), rst("rst"), _output_bmp(output_bmp)
{
	tb.i_clk(clk);
	tb.o_rst(rst);
	gauss_filter.i_clk(clk);
	gauss_filter.i_rst(rst);
	tb.o_rgb(rgb);
	tb.i_result_r(result_r);
	tb.i_result_g(result_g);
	tb.i_result_b(result_b);
	gauss_filter.i_rgb(rgb);
	gauss_filter.o_result_r(result_r);
	gauss_filter.o_result_g(result_g);
	gauss_filter.o_result_b(result_b);

  tb.read_bmp(input_bmp);
}

System::~System() {
  tb.write_bmp(_output_bmp);
}
