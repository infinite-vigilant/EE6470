#include "System.h"
System::System( sc_module_name n, string input_bmp, string output_bmp ): sc_module( n ), 
	tb("tb"), Gaussian_Blur("Gaussian_Blur"), clk("clk", CLOCK_PERIOD, SC_NS), rst("rst"), _output_bmp(output_bmp)
{
	tb.i_clk(clk);
	tb.o_rst(rst);
	Gaussian_Blur.i_clk(clk);
	Gaussian_Blur.i_rst(rst);
	tb.o_r(r);
	tb.o_g(g);
	tb.o_b(b);
	tb.i_result_r(result_r);
	tb.i_result_g(result_g);
	tb.i_result_b(result_b);
	Gaussian_Blur.i_r(r);
	Gaussian_Blur.i_g(g);
	Gaussian_Blur.i_b(b);
	Gaussian_Blur.o_result_r(result_r);
	Gaussian_Blur.o_result_g(result_g);
	Gaussian_Blur.o_result_b(result_b);

  tb.read_bmp(input_bmp);
}

System::~System() {
  tb.write_bmp(_output_bmp);
}
