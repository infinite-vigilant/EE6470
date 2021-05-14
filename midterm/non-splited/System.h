#ifndef SYSTEM_H_
#define SYSTEM_H_
#include <systemc>
using namespace sc_core;

#include "Testbench.h"
#ifndef NATIVE_SYSTEMC
#include "Gaussian_Blur_wrap.h"
#else
#include "Gaussian_Blur.h"
#endif

class System: public sc_module
{
public:
	SC_HAS_PROCESS( System );
	System( sc_module_name n, std::string input_bmp, std::string output_bmp );
	~System();
private:
  Testbench tb;
#ifndef NATIVE_SYSTEMC
	Gaussian_Blur_wrapper Gaussian_Blur;
#else
	Gaussian_Blur Gaussian_Blur;
#endif
	sc_clock clk;
	sc_signal<bool> rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_uint<24> > rgb;
	cynw_p2p< sc_dt::sc_uint<32> > result;
#else
	sc_fifo< sc_dt::sc_uint<24> > rgb;
	sc_fifo< sc_dt::sc_uint<32> > result;
#endif

	std::string _output_bmp;
};
#endif
