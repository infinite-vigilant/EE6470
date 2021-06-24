#include <cstdio>
#include <cstdlib>
#include "input_real.h"
#include "input_imag.h"
#define height 2000
using namespace std;

#include "Testbench.h"

unsigned char header[54] = {
    0x42,          // identity : B
    0x4d,          // identity : M
    0,    0, 0, 0, // file size
    0,    0,       // reserved1
    0,    0,       // reserved2
    54,   0, 0, 0, // RGB data offset
    40,   0, 0, 0, // struct BITMAPINFOHEADER size
    0,    0, 0, 0, // bmp width
    0,    0, 0, 0, // bmp height
    1,    0,       // planes
    24,   0,       // bit per pixel
    0,    0, 0, 0, // compression
    0,    0, 0, 0, // data size
    0,    0, 0, 0, // h resolution
    0,    0, 0, 0, // v resolution
    0,    0, 0, 0, // used colors
    0,    0, 0, 0  // important colors
};

Testbench::Testbench(sc_module_name n) : sc_module(n), output_rgb_raw_data_offset(54) {
  SC_THREAD(feed_rgb);
  sensitive << i_clk.pos();
  dont_initialize();
  
  SC_THREAD(fetch_result);
  sensitive << i_clk.pos();
  dont_initialize();
}

Testbench::~Testbench() {
	//cout<< "Max txn time = " << max_txn_time << endl;
	//cout<< "Min txn time = " << min_txn_time << endl;
	//cout<< "Avg txn time = " << total_txn_time/n_txn << endl;
	cout << "Total run time = " << total_run_time << endl;
}

void Testbench::feed_rgb() {

	n_txn = 0;
	max_txn_time = SC_ZERO_TIME;
	min_txn_time = SC_ZERO_TIME;
	total_txn_time = SC_ZERO_TIME;

#ifndef NATIVE_SYSTEMC
	o_r.reset();
  o_i.reset();
#endif
	o_rst.write(false);
	wait(5);
	o_rst.write(true);
	wait(1);
	total_start_time = sc_time_stamp();
  for (y = 0; y != height; ++y) {
         buffer_r = input_r[i];
         buffer_i = input_i[i];
         sc_dt::sc_uint<24> buffer_r;
         sc_dt::sc_uint<24> buffer_i;
#ifndef NATIVE_SYSTEMC
					o_r.put(buffer_r);
          o_i.put(buffer_i);
#else
					o_r.write(buffer_r);
          o_i.write(buffer_i);
#endif
         

    }
  }


void Testbench::fetch_result() {
int total;
int total_i[2000] = {0};
int total_r[2000] = {0};
#ifndef NATIVE_SYSTEMC
	i_r.reset();
  i_i.reset();
#endif
	wait(5);
	wait(1);
  for (y = 0; y != height; ++y) {
#ifndef NATIVE_SYSTEMC
			total_i[y] = i_result_r.get();
      total_r[y] = i_result_i.get();
#else
			total_i[y] = i_result_r.read();
      total_r[y] = i_result_i.read();
#endif
    }

  for (y = 0; y != height; ++y) {
			cout<<"total"<<total_i[y];
      cout<<"total"<<total_r[y];

    }
	total_run_time = sc_time_stamp() - total_start_time;
  sc_stop();
}
