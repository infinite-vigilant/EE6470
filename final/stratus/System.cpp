#include "System.h"
System::System( sc_module_name n): sc_module( n ), 
	tb("tb"), lstm("lstm"), clk("clk", CLOCK_PERIOD, SC_NS), rst("rst")
{
	tb.i_clk(clk);
	tb.o_rst(rst);
	lstm.i_clk(clk);
	lstm.i_rst(rst);
	tb.o_r(buffer_r);
	tb.o_i(buffer_i);
  tb.i_result_r(result_r);
  tb.i_result_i(result_i);
	  lstm.i_r(buffer_r);
   lstm.i_i(buffer_i);
	lstm.o_result_r(result_r);
  lstm.o_result_i(result_i);


}


