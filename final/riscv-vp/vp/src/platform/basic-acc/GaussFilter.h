#ifndef Gauss_FILTER_H_
#define Gauss_FILTER_H_
#include <systemc>
#include <cmath>
#include <iomanip>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "filter_def.h"

struct GaussFilter : public sc_module {
  tlm_utils::simple_target_socket<GaussFilter> tsock;

  sc_fifo<unsigned char> i_r;
  sc_fifo<unsigned char> i_g;
  sc_fifo<unsigned char> i_b;
  sc_fifo<int> o_result1;
  sc_fifo<int> o_result2;
  sc_fifo<int> o_result3;

  SC_HAS_PROCESS(GaussFilter);

  GaussFilter(sc_module_name n): 
    sc_module(n), 
    tsock("t_skt"), 
    base_offset(0) 
  {
    tsock.register_b_transport(this, &GaussFilter::blocking_transport);
    SC_THREAD(do_filter);
  }

  ~GaussFilter() {
	}

  int Red, Green, Blue;
  unsigned int base_offset;

  void do_filter(){
    { wait(CLOCK_PERIOD, SC_NS); }
    while (true) {
        Red = 0; Blue =0; Green = 0;
        wait(CLOCK_PERIOD, SC_NS);
	double total = 0;
      for (unsigned int v = 0; v < MASK_Y; ++v) {
        for (unsigned int u = 0; u < MASK_X; ++u) {
          //unsigned char grey = (i_r.read() + i_g.read() + i_b.read()) / 3;
          wait(CLOCK_PERIOD, SC_NS);

            Red += i_r.read()* G_mask[u][v];
          Green += i_g.read()* G_mask[u][v];
          Blue += i_b.read()* G_mask[u][v];
            wait(CLOCK_PERIOD, SC_NS);
            total+=G_mask[u][v];

        }
      }
      

      int result1 = (int)(Red/total);
    int result2 = (int)(Green/total);
    int result3 = (int)(Blue/total);
    o_result1.write(result1);
    o_result2.write(result2);
    o_result3.write(result3);
    }
  }

  void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay){
    wait(delay);
    // unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    // auto len = payload.get_data_length();
    tlm::tlm_command cmd = payload.get_command();
    sc_dt::uint64 addr = payload.get_address();
    unsigned char *data_ptr = payload.get_data_ptr();

    addr -= base_offset;


    // cout << (int)data_ptr[0] << endl;
    // cout << (int)data_ptr[1] << endl;
    // cout << (int)data_ptr[2] << endl;
    word buffer;

    switch (cmd) {
      case tlm::TLM_READ_COMMAND:
        // cout << "READ" << endl;
        switch (addr) {
          case Gauss_FILTER_RESULT_ADDR:
            buffer.uc[0] = (char)(o_result1.read());
            buffer.uc[1] = (char)(o_result2.read());
            buffer.uc[2] = (char)(o_result3.read());
            buffer.uc[3] = 0;
            break;
          default:
            std::cerr << "READ Error! GaussFilter::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
          }
        data_ptr[0] = buffer.uc[0];
        data_ptr[1] = buffer.uc[1];
        data_ptr[2] = buffer.uc[2];
        data_ptr[3] = buffer.uc[3];
        break;
      case tlm::TLM_WRITE_COMMAND:
        // cout << "WRITE" << endl;
        switch (addr) {
          case Gauss_FILTER_R_ADDR:
            i_r.write(data_ptr[0]);
            i_g.write(data_ptr[1]);
            i_b.write(data_ptr[2]);
            break;
          default:
            std::cerr << "WRITE Error! GaussFilter::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
        }
        break;
      case tlm::TLM_IGNORE_COMMAND:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
      default:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
      }
      payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
  }
};
#endif
