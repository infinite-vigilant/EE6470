#include <iostream>
#include <string>


using namespace std;

// Wall Clock Time Measurement
#include <sys/time.h>

#include "gaussfilter.h"
#include "testbench.h"

// TIMEVAL STRUCT IS Defined ctime
// use start_time and end_time variables to capture
// start of simulation and end of simulation
struct timeval start_time, end_time;

// int main(int argc, char *argv[])
int sc_main(int argc, char **argv) {
  if ((argc < 3) || (argc > 4)) {
    cout << "No arguments for the executable : " << argv[0] << endl;
    cout << "Usage : >" << argv[0] << " in_image_file_name out_image_file_name"
         << endl;
    return 0;
  }
  Testbench tb("tb");
  gaussfilter gauss_filter("gauss_filter");
  sc_clock clk("clk", CLOCK_PERIOD, SC_NS);
  sc_signal<bool> rst("rst");
  sc_fifo<double> r;
  sc_fifo<double> g;
  sc_fifo<double> b;
  sc_fifo<double> result_r;
  sc_fifo<double> result_g;
  sc_fifo<double> result_b;
  tb.i_clk(clk);
  tb.o_rst(rst);
  gauss_filter.i_clk(clk);
  gauss_filter.i_rst(rst);
  tb.o_r(r);
  tb.o_g(g);
  tb.o_b(b);
  tb.i_result_r(result_r);
  tb.i_result_g(result_g);
  tb.i_result_b(result_b);
  gauss_filter.i_r(r);
  gauss_filter.i_g(g);
  gauss_filter.i_b(b);
  gauss_filter.o_result_r(result_r);
  gauss_filter.o_result_g(result_g);
  gauss_filter.o_result_b(result_b);

  tb.read_bmp(argv[1]);
  sc_start();
  std::cout << "Simulated time == " << sc_core::sc_time_stamp() << std::endl;
  tb.write_bmp(argv[2]);

  return 0;
}

