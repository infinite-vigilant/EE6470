#include <systemc>
#include <sys/time.h>
#include "filter_def.h"
#include "weight/aux_ker.h"
#include "weight/lstm_ker.h"
#include "weight/lstm_rec.h"
#include "weight/lstm_bias.h"
#include "weight/main_out.h"
#include <cynw_p2p.h>

using namespace std;
using namespace sc_core;
using namespace sc_dt;

class lstm: public sc_module {
private:
    int N1;
public:

	sc_in_clk i_clk;
	sc_in < bool > i_rst;
    
  sc_fifo<int> i_1;//since the total input is flatten so at every even position real
  sc_fifo<int> i_2;//after every real is its corresponding imaginary(odd pos) output_1 and output2
  sc_fifo<int> o_result1;
  sc_fifo<int> o_result2;
    
    SC_HAS_PROCESS(lstm);
    
    lstm(sc_module_name n):
    sc_module(n),
    base_offset(0)
   
};

